#include "KDTree2.h"
#include <cassert>
#include <climits>
#include <iterator>
#include <limits>
#include <unordered_set>
#include <vector>
#include "Box.h"
#include "OrderedList.h"
#include "PolyObject.h"
#include "ray.h"
namespace RayTracerxx {

KDTree::KDTree(Box sceneBox, TriList triangles) {
        bbox      = sceneBox;
        num_nodes = 0;
        root      = buildTree(triangles, sceneBox);
        std::cout << "Num nodes " << num_nodes << "\n";
}

bool KDTree::Intersect(Ray &ray) {
        std::pair<Number_t, Number_t> t     = bbox.Intersect(ray);
        Number_t                      infty = Ray::Infinity;

        if (t != std::make_pair(infty, infty))
                root->traverse(ray, t.first, t.second);

        return not(ray.hit == NULL);
}

/**
 * @brief      Initializes the building process by generating lists of events
 *             and objects. Starts building KDTree.
 */
KDTree::Node *KDTree::buildTree(TriList &tris, const Box &V) {
        std::vector<Object> objPool;  // contiguous memory for all Object
        ObjectList            objects;  // list of pointers to objPool
        EventList             events;

        objects.reserve(tris.size());
        objPool.reserve(tris.size());

        // Generate object list
        for (Triangle *t : tris) {
                objPool.emplace_back(t);
                objects.push_back(&objPool.back());
        }

        // std::cout << "objects.size() = " << objects.size() << "\n";
        // std::cout << "Events.size() = " << events.size() << "\n";

        // Generate events for candidate planes (sides of bounding boxes)
        for (Object *tmp : objects) {
                for (unsigned k = X; k <= Z; k++) {
                        Box bounds = clipTriangleToBox(tmp->tri, V);
                        generateEvent(bounds, k, tmp, events);
                }
        }

        // sort the events
        std::sort(events.begin(), events.end());
        // std::cout << "Events.size() = " << events.size() << "\n";

        return buildTree(objects, events, V);
}


/**
  * @brief      Tries to split current node into two smaller nodes if a good
  *             split can be found.
  */
KDTree::Node *KDTree::buildTree(ObjectList &objs, EventList &events,
                                const Box &V) {
        Plane              sp(-1, std::numeric_limits<Number_t>::max());
        constexpr unsigned minTris = 5;
        num_nodes++;

        // Make leaf if good split isn't possible, or there are few triangles
        if (objs.size() < minTris || !findSplit(objs.size(), V, events, sp))
                return new LeafNode(objs, V);

        EventList EL, ER; // left and right event lists for children
        generateChildList(events, sp, V, EL, ER);

        Box left_box, right_box;
        splitBox(V, sp, left_box, right_box);

        // split the objs into lists for left and right
        ObjectList left_objects, right_objects;
        partitionObjects(objs, left_objects, right_objects);

        Number_t PL = hit_prob(left_box, V), PR = hit_prob(right_box, V);
        Number_t CP = C(PL, PR, left_objects.size(), right_objects.size());

        if (shouldStop(objs.size(), CP)) {
                return new LeafNode(objs, V);
        } else
                return new InnerNode(sp, V,
                                     buildTree(left_objects, EL, left_box),
                                     buildTree(right_objects, ER, right_box));
}


inline Number_t KDTree::hit_prob(const Box &sub, const Box &V) const {
        return (sub.area() / V.area());
}


inline Number_t KDTree::lambda(int numL, int numR, Number_t PL,
                               Number_t PR) const {
        if ((numL == 0 or numR == 0) and not(PL == 1 or PR == 1))
                return 0.8f;
        return 1.0f;
}

inline Number_t KDTree::C(Number_t PL, Number_t PR, int numL, int numR) const {
        return (lambda(numL, numR, PL, PR) *
                (kt + ki * (PL * numL + PR * numR)));
}

void KDTree::splitBox(const Box &V, const Plane &p, Box &subLeft,
                      Box &subRight) const {
        subLeft  = V;
        subRight = V;
        subLeft.setMax(p.lane, p.oint);
        subRight.setMin(p.lane, p.oint);

        assert(V.contains(subLeft) and V.contains(subRight));
}

/**
 * @brief      Computes the Surface Area Heuristic (SAH) cost of a given
 *             split plane
 */
float KDTree::SAHcost(const Box &V, Plane *p, const int NL, const int NR,
                      const int NP) {
        Number_t CP = Ray::Infinity; // SAH split cost
        Box      subLeft, subRight;
        splitBox(V, *p, subLeft, subRight);

        Number_t PL = hit_prob(subLeft, V), PR = hit_prob(subRight, V);

        Number_t CPL = C(PL, PR, NP + NL, NR); //add planar triangles to left
        Number_t CPR = C(PL, PR, NL, NP + NR); //add planar triangles to right

        CP      = (CPL < CPR) ? CPL : CPR;
        p->side = (CPL < CPR) ? Plane::LEFT : Plane::RIGHT;
        return CP;
}

/**
 * @brief      Checks whether the build tree termination criteria has
 *             been met
 */
inline bool KDTree::shouldStop(int numTris, Number_t splitCost) const {
        return (splitCost > ki * numTris);
}

/**
 * @brief      Computes the intersection of triangles bounding box and the
 *             box in question
 */
Box KDTree::clipTriangleToBox(const Triangle *t, const Box &V) const {
        Box b = t->CalcBounds();

        for (int k = 0; k < 3; k++) {
                if (V.low[k] > b.low[k])
                        b.low[k] = V.low[k];

                if (V.hi[k] < b.hi[k])
                        b.hi[k] = V.hi[k];
        }
        assert(V.contains(b));
        return b;
}



/**
  * @brief      To avoid resorting the entire eventlist for each new node,
  *             we can instead divide a parent's event list into two sorted
  *             sublists. We must also incorporate newly generated events, but
  *             the number of those is small compared to the partioned sublists
  *             Those smaller lists can be sorted and merged with the partioned
  *             list. (See reference)
  */
void KDTree::generateChildList(EventList &events, const Plane &sp,
                               const Box &V, EventList &EL, EventList &ER) {
        // Classify the Triangles
        classify(events, sp);

        // partion events into two sorted sublists
        EventList sortedEL;
        EventList sortedER;
        partitionEvents(events, sortedEL, sortedER);

        // Generate new unsorted event lists created by triangles that overlap
        // the split plane
        EventList unsortedEL;
        EventList unsortedER;
        generateNewEvents(events, V, sp, unsortedEL, unsortedER);

        // merge the four lists to EL and ER
        EL.reserve(sortedEL.size() + unsortedEL.size());
        ER.reserve(sortedER.size() + unsortedER.size());
        mergeEventList(sortedEL, unsortedEL, EL);
        mergeEventList(sortedER, unsortedER, ER);
}

/**
  * @brief      Finds the splitplane with the minimal cost in one sweep of the
  *             eventlist by keeping track of triangles to the left, right,
  *             and planar to a given candidate (done in all dimensions)
  */
bool KDTree::findSplit(unsigned numObjects, const Box &V, EventList &events,
                       Plane &result) {
        unsigned numLeft[3]   = {0, 0, 0};
        unsigned numPlanar[3] = {0, 0, 0};
        unsigned numRight[3]  = {numObjects, numObjects, numObjects};
        unsigned p_start, p_end, p_planar;
        bool     foundBest = false;  // whether result has been updated
        Number_t minCost   = std::numeric_limits<Number_t>::max();

        // iterate over all split candidates
        unsigned i = 0;
        while (i < events.size()) {
                Plane &p = events[i].p;
                p_start = p_end = p_planar = 0;

                if (p.oint == V.low[p.lane] || p.oint == V.hi[p.lane]) {
                        i++;
                        continue;
                }

                // count types of events lying on current plane
                for (; i < events.size() && events[i].is_in(p); i++) {
                        switch (events[i].type) {
                                case Event::endsOnPlane: p_end++; break;
                                case Event::liesOnPlane: p_planar++; break;
                                case Event::startsOnPlane: p_start++; break;
                                default: break;
                        }
                }
                // update the counts planar and right
                numPlanar[p.lane] = p_planar;
                numRight[p.lane] -= p_planar + p_end;

                // calculate the costs for the split plane
                float cost = SAHcost(V, &p, numLeft[p.lane], numRight[p.lane],
                                     numPlanar[p.lane]);

                // update the numbers for left
                numLeft[p.lane] += p_start + p_planar;
                // reset planar counts for next iteration
                numPlanar[p.lane] = 0;

                if (cost < minCost) {
                        minCost   = cost;
                        result    = p;
                        foundBest = true;
                }
        }

        return foundBest;
}

/**
  * @brief      Sets the side field of the Object, classifying them into
  *             LEFT, RIGHT, or BOTH
  */
void KDTree::classify(EventList &events, const Plane &p) {
        // set all 'side' fields to 'both'
        for (Event &e : events)
                e.obj->side = Plane::BOTH;

        for (Event &e : events) {
                if (e.p.lane != p.lane)
                        continue;

                if (e.type == Event::endsOnPlane && e.p.oint <= p.oint)
                        e.obj->side = Plane::LEFT;
                else if (e.type == Event::startsOnPlane && e.p.oint >= p.oint)
                        e.obj->side = Plane::RIGHT;
                else if (e.type == Event::liesOnPlane) {
                        if (e.p.oint < p.oint)
                                e.obj->side = Plane::LEFT;
                        else if (e.p.oint > p.oint)
                                e.obj->side = Plane::RIGHT;
                        else // (e.p.oint == p.oint)
                                e.obj->side = p.side;
                }
        }
}



/**
  * @brief      Generates new events for objects overlapping a split plane
  */
void KDTree::generateNewEvents(const EventList &events, const Box &V,
                               const Plane &p, EventList &EBL,
                               EventList &EBR) {
        Box lbox, rbox;
        splitBox(V, p, lbox, rbox);

        // iterate over the events and split those which lie on both sides
        for (const Event &e : events) {
                if (e.obj->side == Plane::BOTH) {
                        Box lclip = clipTriangleToBox(e.obj->tri, lbox);
                        Box rclip = clipTriangleToBox(e.obj->tri, rbox);
                        generateEvent(lclip, e.p.lane, e.obj, EBL);
                        generateEvent(rclip, e.p.lane, e.obj, EBR);
                }
        }
}

void KDTree::mergeEventList(EventList &sorted, EventList &unsorted,
                            EventList &output) {
        std::sort(unsorted.begin(), unsorted.end());
        std::merge(sorted.begin(), sorted.end(), unsorted.begin(),
                   unsorted.end(), std::back_inserter(output));
}



/**
  * @brief      Partitions events into two sublists
  */
void KDTree::partitionEvents(const EventList &events, EventList &EL,
                             EventList &ER) {
        for (const Event &e : events) {
                switch (e.obj->side) {
                        case Plane::LEFT: EL.push_back(e); break;
                        case Plane::RIGHT: ER.push_back(e); break;
                        default: break;  // Plane::BOTH events are ignored
                }
        }
}


/**
  * @brief      Partitions objects into two sublists
  */
void KDTree::partitionObjects(const ObjectList &objects, ObjectList &left_list,
                              ObjectList &right_list) {
        for (Object *tmp : objects) {
                switch (tmp->side) {
                        case Plane::LEFT: left_list.push_back(tmp); break;
                        case Plane::RIGHT: right_list.push_back(tmp); break;
                        case Plane::BOTH:
                                left_list.push_back(tmp);
                                right_list.push_back(tmp);
                                break;
                        default: break;
                }
        }
}


/**
  * @brief      Generates candidate split plane events
  */
void KDTree::generateEvent(const Box &box, unsigned k, Object *obj,
                           EventList &list) {
        if (box.isPlanar(k)) {
                list.emplace_back(k, box.low[k], Event::liesOnPlane, obj);
        } else {
                list.emplace_back(k, box.low[k], Event::startsOnPlane, obj);
                list.emplace_back(k, box.hi[k], Event::endsOnPlane, obj);
        }
}
}  // namespace RayTracerxx