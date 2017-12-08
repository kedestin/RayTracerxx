#include "OrderedList.h"
#include "ray.h"
#include "PolyObject.h"
#include "Box.h"
#include <cassert>
#include <vector>
#include <climits>
#include "KDTree.h"



KDTree::KDTree(Box sceneBox, vector<Triangle *> triangles){
        kt = 1.0;
        ki = 1.5;
        bbox = sceneBox;
        maxdepth = 0;
        num_nodes = 0;
        root = buildTree(triangles, sceneBox, 0, Plane());
}

bool KDTree::Intersect(Ray &ray) {
        std::pair<float, float> t = bbox.Intersect(ray);
        root->traverse(ray, t.first, t.second);
        return not (ray.hit == NULL);
}       
        
// surface area of a box

inline float KDTree::area(const Box& V) const {
        return 2*V.dX()*V.dY() + 2*V.dX()*V.dZ() + 2*V.dY()*V.dZ();
}

// probability of hitting the subbox
inline float KDTree::collision_prob(const Box& sub, const Box& V) const {
        return( area(sub) / area(V) );
}

// bias for the cost function; it is reduced if numL or numR becomes zero
inline float KDTree::lambda(int numL, int numR, float PL, float PR) const {
        if((numL == 0 or numR == 0) and not (PL == 1 or PR == 1))
                return 0.8f;
        return 1.0f;
}

// estimated cost of creating a new subtree
inline float KDTree::C(float PL, float PR, int numL, int numR) const {
        return(lambda(numL, numR, PL, PR)*(kt + ki * (PL * numL + PR * numR)));
}

// split a voxel V using a plane p
void KDTree::splitBox(const Box& V, const Plane& p, Box& subLeft, Box& subRight)
const {
        subLeft = V;
        subRight = V;
        subLeft.setMax(p.lane, p.oint);
        subRight.setMin(p.lane, p.oint);

        assert(V.contains(subLeft) and V.contains(subRight));
}

typedef enum { LEFT=-1, RIGHT=1, UNKNOWN=0 } PlaneSide;

// SAH heuristic for computing the cost of splitting a Box using a plane p
void KDTree::SAH(const Plane& p, const Box& V, int numL, int numR, int numP,
                       float& CP, PlaneSide& pside)
const {
        CP = Infinity;
        Box subLeft, subRight;
        splitBox(V, p, subLeft, subRight);
        float PL, PR;
        PL = collision_prob(subLeft, V);
        PR = collision_prob(subRight, V);

        if(PL == 0 or PR == 0 or V.d(p.lane) == 0)
                return;

        
        float CPL, CPR;
        CPL = C(PL, PR, numL + numP, numR);  //left cost 
        CPR = C(PL, PR, numL, numP + numR ); //right cost

        CP    = (CPL < CPR) ? CPL : CPR;
        pside = (CPL < CPR) ? LEFT : RIGHT;
}

// terminates when node traversal becomes cheaper than node creation
inline bool KDTree::shouldStop(int numTris, float splitCost) const {
       
        return(splitCost > ki*numTris);
}

// ggets portion of triangles bounding box that's within containing box
Box KDTree::clipTriangleToBox(Triangle* t, const Box& V) const {
        Box b = t->CalcBounds();

        for(int k=0; k<3; k++) {
        
                if(V.min[k] > b.min[k])
                        b.min[k] = V.min[k];
                
                if(V.max[k] < b.max[k])
                        b.max[k] = V.max[k];
        }
        assert(V.contains(b));
        return b;
}

// best spliting plane using SAH heuristic
void KDTree::findPlane(const Triangles& T, const Box& V, int depth,
                       Plane& p_est, float& C_est, PlaneSide& pside_est) const {
        (void) depth;
        C_est = Infinity; //Estimated cost
        for(int k=0; k<3; ++k) {
                vector<Event> events;
                events.reserve(T.size()*2);
                for(size_t i = 0; i < T.size(); i++) {
                        Triangle* tri = T[i];
                        Box B = clipTriangleToBox(tri, V);
                        if(B.isPlanar()) {
                                events.push_back(Event(tri, k, B.min[k],
                                                       Event::liesOnPlane));
                        } else {
                                events.push_back(Event(tri, k, B.min[k],
                                                       Event::startsOnPlane));
                                events.push_back(Event(tri, k, B.max[k],
                                                       Event::endsOnPlane));
                        }
                }
                sort(events.begin(), events.end());
                int numL = 0, numP = 0, numR = T.size();
                for(vector<Event>::size_type Ei = 0; Ei < events.size(); ++Ei) {
                        findMinCost(Ei, events, numL, numP, numR, C_est, p_est,
                                    V, pside_est);
                }
        }

}


void KDTree::findMinCost(size_t& Ei, vector<Event>& events, int& numL,int& numP,
                         int& numR, float& C_est, Plane& p_est, const Box& V,
                         PlaneSide& pside_est) const
{
        const Plane& p = events[Ei].p;
        int pLyingOnPlane = 0, pStartingOnPlane = 0, pEndingOnPlane = 0;
        while(Ei < events.size() and events[Ei].p.oint == p.oint 
                                 and events[Ei].type == Event::endsOnPlane) {
                ++pEndingOnPlane;
                Ei++;
        }
        while(Ei < events.size() and events[Ei].p.oint == p.oint 
                                 and events[Ei].type == Event::liesOnPlane) {
                ++pLyingOnPlane;
                Ei++;
        }
        while(Ei < events.size() and events[Ei].p.oint == p.oint 
                                 and events[Ei].type == Event::startsOnPlane) {
                ++pStartingOnPlane;
                Ei++;
        }
        float C;
        PlaneSide pside = UNKNOWN;
        numP = pLyingOnPlane;
        numR -= (pLyingOnPlane + pEndingOnPlane);

        SAH(p, V, numL, numR, numP, C, pside);
        numL += (pStartingOnPlane + pLyingOnPlane);
        numP = 0;

        if(C < C_est) {
                C_est     = C;
                p_est     = p;
                pside_est = pside;
        }
}
// sort triangles into left and right voxels
void KDTree::partitionTriangles(const Triangles& T, const Plane& p,
                                const PlaneSide& pside, Triangles& TL,
                                Triangles& TR)
 const {
        for(size_t i = 0; i < T.size(); i++) {
                Triangle* t = T[i];
                Box tbox = t->CalcBounds();
                if(tbox.min[p.lane] == p.oint and tbox.max[p.lane] == p.oint) {
                        if(pside == LEFT)
                                TL.push_back(t);
                        else if(pside == RIGHT)
                                TR.push_back(t);
                        else
                                assert(false); // wrong pside
                } else {
                        if(tbox.min[p.lane] < p.oint)
                                TL.push_back(t);
                        if(tbox.max[p.lane] > p.oint)
                                TR.push_back(t);
                }
        }
}

KDTree::Node* KDTree::buildTree(Triangles T, Box &V, int depth,
                                const Plane& prev_p){
        assert(depth < 100);

        ++num_nodes;

        if(depth > maxdepth) 
                maxdepth = depth;

        Plane p;
        float Cp;
        PlaneSide pside;
        findPlane(T, V, depth, p, Cp, pside);
        if(shouldStop(T.size(), Cp) or p == prev_p)
                return new LeafNode(T, V);

        Box subLeft, subRight;
        splitBox(V, p, subLeft, subRight);
        Triangles TL, TR;
        partitionTriangles(T, p, pside, TL, TR);
        return new InnerNode(p,
                             V,
                             buildTree(TL, subLeft, depth+1, p),
                             buildTree(TR, subRight, depth+1, p));
}
