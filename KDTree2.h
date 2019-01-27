#ifndef __KDTREE_TREE_H_INCLUDED
#define __KDTREE_TREE_H_INCLUDED

#include <time.h>
#include <cassert>
#include <climits>
#include <vector>
#include "Box.h"
#include "OrderedList.h"
#include "PolyObject.h"
#include "ray.h"

namespace RayTracerxx {

/**
 * @brief      K-Dimensional (KD) Tree implementation
 *
 * @details    Accelerated Data Structure for pruning set of primitives to be
 *             tested for intersection
 *
 * @reference  Wald, I., and HAvran, V.  2006. On building fast kd-trees for
 *             ray tracing, and on doing that in O(N log N).
 */
class KDTree {
private:
        /*
         *                                 Structs
         */

        /**
         * @brief      Split plane
         */
        struct Plane {
                typedef enum { LEFT = -1, RIGHT = 1, BOTH = 0 } Side;
                short    lane;  // splitting dimension
                Number_t oint;  // splitting point
                Side     side;
                Plane(short plane, Number_t point) {
                        lane = plane;
                        oint = point;
                        side = BOTH;
                }

                bool operator==(const Plane &sp) const {
                        return (lane == sp.lane && oint == sp.oint);
                }
        };

        /**
         * @brief      Internal representation of primitives in the KDTree
         */
        struct Object {
                Triangle *  tri;
                Plane::Side side;

                // constructor
                Object(Triangle *p) {
                        tri  = p;
                        side = Plane::BOTH;
                }

                // constructor with all fields
                Object(Triangle *p, Plane::Side s) {
                        tri  = p;
                        side = s;
                }
        };

        /**
         * @brief      Abstract Node Class
         */
        struct Node {
                virtual ~Node() {}
                virtual void traverse(Ray &, Number_t, Number_t) = 0;
                virtual int  depth(int d) const                  = 0;
        };

        /**
         * @brief      Inner Node Class
         */
        struct InnerNode : public Node {
                Plane p;
                Node *left, *right;

                InnerNode(const Plane &p0, const Box &V0, Node *lc, Node *rc)
                    : p(p0), left(lc), right(rc) {
                        (void)V0;
                }

                /**
                 * @brief      Traverses the tree  "in-order".
                 *
                 *
                 * @details    Determines where the ray traverses the area
                 *             befere and after the split plane.
                 *             Recursively explores those two regions
                 *
                 * @param      ray    The ray
                 * @param[in]  t_min  The t minimum
                 * @param[in]  t_max  The t maximum
                 */
                virtual void traverse(Ray &ray, Number_t t_min,
                                      Number_t t_max) {
                        // finds when ray intersects split plane
                        Number_t t_split =
                            (p.oint - ray.origin[p.lane]) * ray.inv(p.lane);

                        // finds the plane closest to ray origin
                        Node *near, *far;
                        near = (ray.origin[p.lane] < p.oint) ? left : right;
                        far  = (ray.origin[p.lane] < p.oint) ? right : left;

                        // only traverse near if ray exits before hitting far
                        if (t_split > t_max or t_split < 0)
                                near->traverse(ray, t_min, t_max);
                        // only traverse far if ray exits before hitting near
                        else if (t_split < t_min)
                                far->traverse(ray, t_min, t_max);
                        else {
                                // tries near, then goes far if no hit
                                near->traverse(ray, t_min, t_split);
                                // Stop if ray hits something before reaching
                                // far
                                if (ray.t < t_split)
                                        return;
                                far->traverse(ray, t_split, t_max);
                        }
                }

                virtual int depth(int d) const {
                        return std::max(left->depth(d + 1),
                                        right->depth(d + 1));
                }

                virtual ~InnerNode() {
                        delete left;
                        delete right;
                };
        };

        /**
         * @brief      Leaf Node class.
         *
         * @details    Holds a list of triangles
         */
        struct LeafNode : public Node {
                std::vector<Triangle *> T;
                Box                     V;

                LeafNode(std::vector<Triangle *> &T0, const Box &V0)
                    : T(T0), V(V0) {}

                LeafNode(const std::vector<Object *> &O, const Box &V0)
                    : V(V0) {
                        T.reserve(O.size());
                        for (Object *a : O)
                                T.push_back(a->tri);
                }

                /**
                 * @brief      Iterates through all triangles and performs
                 *             intersection test
                 *
                 * @param      ray    The ray
                 * @param[in]  t_min  The t minimum
                 * @param[in]  t_max  The t maximum
                 */
                virtual void traverse(Ray &ray, Number_t t_min,
                                      Number_t t_max) {
                        (void)t_min;
                        (void)t_max;
                        for (Triangle *tri : T)
                                tri->Intersect(ray);
                }

                virtual int depth(int d) const { return d; }

                virtual ~LeafNode(){};
        };

        /**
         * @brief      Representation of a split plane candidate
         *
         * @details    Each event is characterized by the plane it represents,
         *             the object that generated that event, and how the
         *             event was generated (type:starts on the plane,
         *             ends on the plane, or lies completely on the plane)
         */
        struct Event {
                typedef enum {
                        endsOnPlane = 0,
                        liesOnPlane,
                        startsOnPlane
                } EventType;

                Object *  obj;
                Plane     p;
                EventType type;

                Event(int k, Number_t point, EventType newType)
                    : obj(NULL), p(k, point), type(newType) {}

                Event(int k, Number_t point, EventType newType, Object *ob)
                    : Event(k, point, newType) {
                        obj = ob;
                }

                /**
                 * @brief      Comparison as defined by reference paper
                 */
                bool operator<(const Event &e) const {
                        return ((p.oint < e.p.oint) or
                                (p.oint == e.p.oint && type < e.type));
                }

                /**
                 * @brief      Checks whether an event originated on given
                 *             plane
                 *
                 * @return     True if in, False otherwise.
                 */
                bool is_in(const Plane &toTest) const { return p == toTest; }
        };

        /*
         *                                  Typedefs
         */
        typedef std::vector<Event>      EventList;
        typedef std::vector<Triangle *> TriList;
        typedef std::vector<Object *>   ObjectList;
        typedef enum { X = 0, Y, Z } Dimension;

        /*
         *                                   Methods
         */

        /**
         * @brief      Builds a tree.
         *
         * @details    Helper function for the recursive version
         *
         * @param      tris  The triangles
         * @param      V     The bounding box for all the triangles
         *
         * @return     The root of the KDTree
         */
        Node *buildTree(TriList &tris, const Box &V);

        /**
         * @brief      Builds a tree.
         *
         * @details    Recursively constructs nodes of the tree
         *
         * @param      objects  The objects
         * @param      events   The events
         * @param[in]  V        Bounding box for current subtree
         *
         * @return     The root of the sub-KDTree.
         */
        Node *buildTree(ObjectList &objects, EventList &events, const Box &V);

        /**
         * @brief      Checks whether the build tree termination criteria has
         *             been met
         *
         * @param[in]  numTris    The number tris
         * @param[in]  splitCost  The split cost
         *
         * @return     Whether the recursively building the KDTree
         */
        bool shouldStop(int numTris, Number_t splitCost) const;

        /**
         * @brief      Checks whether an event is in list of events, and lies
         *             on given plane
         *
         * @param[in]  i       index of event
         * @param[in]  events  The list of events
         * @param[in]  p       The plane
         *
         * @return     True if valid event and same plane, False otherwise.
         */
        inline bool isSamePlane(const unsigned int i, const EventList &events,
                                const Plane &p);

        /**
         * @brief      Computes the Surface Area Heuristic (SAH) cost
         *
         * @param[in]  V     Box to split
         * @param      p     Split plane
         * @param[in]  NL    Number of triangles in left box
         * @param[in]  NR    Number of triangles in right box
         * @param[in]  NP    Number of triangles that are along the plane
         *
         * @return     The sah costs.
         */
        float SAHcost(const Box &V, Plane *p, const int NL, const int NR,
                      const int NP);

        /**
         * @brief      Probability of hitting a box given that we've hit the
         *             box that contains it
         *
         * @param[in]  sub   The sub box
         * @param[in]  V     The container box
         *
         * @return     Number_t The hit probability
         */
        Number_t hit_prob(const Box &sub, const Box &V) const;

        /**
         * @brief      Bias for the cost function. Computed as described in
         *             the paper
         *
         * @param[in]  numL  The number of triangles on left
         * @param[in]  numR  The number of triangles on right
         * @param[in]  PL    Left probability
         * @param[in]  PR    Right probability
         *
         * @return     Number_t Cost bias
         */
        Number_t lambda(int numL, int numR, Number_t PL, Number_t PR) const;

        /**
         * @brief      Cost function for creating a new subtree.
         *             Computed as described in the paper.
         *
         * @param[in]  PL    Left probability
         * @param[in]  PR    Right probability
         * @param[in]  numL  The number of triangles on left
         * @param[in]  numR  The number of triangles on right
         *
         * @return     Estimated cost of creating a new subtree
         */
        Number_t C(Number_t PL, Number_t PR, int numL, int numR) const;

        /**
         * @brief      Finds the optimal split plane
         *
         * @param[in]  numObjects  The number objects
         * @param[in]  V           The bounding box
         * @param      events      The events
         * @param      result      The result (output parameter)
         *
         * @return     Whether the optimal split plane could be found
         */
        bool findSplit(unsigned numObjects, const Box &V, EventList &events,
                       Plane &result);

        /**
         * @brief      Divides a box into two subboxes along a split plane
         *
         * @param[in]  V         Box to split
         * @param[in]  p         Split plane
         * @param      subLeft   Left  sub box (output parameter)
         * @param      subRight  Right sub box (output parameter)
         */
        void splitBox(const Box &V, const Plane &p, Box &subLeft,
                      Box &subRight) const;

        /**
         * @brief      Computes the intersection of triangles bounding box and
         *             the box in question
         *             Note: A non empty intersection is assumed
         *
         * @param      t     Triangle
         * @param[in]  V     Containing box
         *
         * @return     Non empty intersection of the two boxes
         */
        Box clipTriangleToBox(const Triangle *t, const Box &V) const;

        /**
         * @brief      Classifies the each event's Object into LEFT, RIGHT
         *             or BOTH
         *
         * @param      events  The events
         * @param[in]  p       The split plane
         */
        void classify(EventList &events, const Plane &p);

        /**
         * @brief      Generate events in the k-th dimension using obj's
         *             bounding box, and adds them to the event list
         *
         * @param[in]  box   The bounding box
         * @param[in]  k     The dimension
         * @param      obj   The object
         * @param      list  The event list (output parameter )
         */
        void generateEvent(const Box &box, unsigned k, Object *obj,
                           EventList &list);

        /**
         * @brief      Generates new lists to be added to children's event
         *             lists
         *
         * @param      events  The events
         * @param[in]  V       The bounding box
         * @param[in]  p       The split plane
         * @param      EBL     The ebl (output parameter)
         * @param      EBR     The ebr (output parameter)
         */
        void generateNewEvents(const EventList &events, const Box &V,
                               const Plane &p, EventList &EBL, EventList &EBR);

        /**
         * @brief      Merges a sorted and unsorted event list into a larger
         *             event list
         *
         * @param      sorted    The sorted event list
         * @param      unsorted  The unsorted event list
         * @param      output    The output event list (output parameter)
         */
        void mergeEventList(EventList &sorted, EventList &unsorted,
                            EventList &output);

        /**
         * @brief      Partitions objects into a left object list and a right
         *             object list
         *
         * @param[in]  objects     The object list
         * @param      left_list   The left object list  (output parameter)
         * @param      right_list  The right object list (output parameter)
         */
        void partitionObjects(const ObjectList &objects, ObjectList &left_list,
                              ObjectList &right_list);

        /**
         * @brief      Partitions events into a left event list and a right
         *             event list
         *
         * @param[in]  events  The event list
         * @param      ELO     The left event list (output parameter)
         * @param      ERO     The right event list (output parameter)
         */
        void partitionEvents(const EventList &events, EventList &ELO,
                             EventList &ERO);

        /**
         * @brief      Generates left and right event lists from a larger
         *             event list
         *
         * @param      events  The events
         * @param      sp      The split plane
         * @param[in]  V       The bounding box
         * @param      EL      The left event list (output parameter)
         * @param      ER      The right event list (output parameter)
         */
        void  generateChildList(EventList &events, const Plane &sp,
                                const Box &V, EventList &EL, EventList &ER);
        Node *root;
        int   num_nodes;
        Box   bbox;
        static constexpr Number_t ki = 1.0;  // triangle  intersection cost
        static constexpr Number_t kt = 1.5;  // traversal cost

public:
        KDTree() : root(NULL) {}

        /**
         * @brief      Builds a KDTree using the provided triangles and the
         *             bounding box around all the triangles
         *
         * @param[in]  sceneBox   The scene bounding box
         * @param[in]  triangles  The triangles
         */
        KDTree(Box sceneBox, std::vector<Triangle *> triangles);

        /**
         * @brief      Destroys the object.
         */
        ~KDTree() { delete root; }

        /**
         * @brief      Intersects the ray with the triangles in the scene
         *
         * @param      ray   The ray
         *
         * @return     Whether there was an intersection
         */
        bool Intersect(Ray &ray);
};
}  // namespace RayTracerxx

#endif