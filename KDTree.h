#ifndef __KDTREE_TREE_H_INCLUDED
#define __KDTREE_TREE_H_INCLUDED

#include "OrderedList.h"
#include "ray.h"
#include "PolyObject.h"
#include "Box.h"
#include <time.h>
#include <cassert>
#include <vector>
#include <climits>

class KDTree{
private: 
	struct Plane {		
		short lane;	// splitting dimension		
		float oint;	// splitting point
		
		Plane(short plane=-1, float point=0) {
			lane = plane;
			oint = point;
		}
		
		bool operator==(const Plane& sp) {
			return(lane == sp.lane && oint == sp.oint);
		}		
	};
		
	struct Node{
                virtual ~Node() {}
		virtual void traverse(Ray &ray, float t_min, float t_max) = 0;
		virtual int depth(int d) const = 0;
	};

	struct InnerNode : public Node
	{
		Plane p;		
		Node *left, *right;
		
		InnerNode(const Plane& p0, const Box& V0, Node* lc, Node* rc) :
			p(p0), left(lc), right(rc) {
			(void) V0;
		}
		
		virtual void traverse(Ray &ray, float t_min, float t_max){	
			// finds when ray intersects split plane
			float t_split = (p.oint - ray.origin[p.lane]) 
			                 * ray.inv(p.lane);
		 	                
			// finds the plane closest to ray origin
			Node *near, *far;
			if(ray.origin[p.lane] < p.oint) {
				near = left;
				far = right;
			} 
			else {
				near = right;
				far = left;
			}
			
			// only traverses near
			if( t_split > t_max or t_split < 0) {
			    	near->traverse(ray, t_min, t_max);
			}
			// only traverses far
			else if(t_split < t_min) {
			    	far->traverse(ray, t_min, t_max);
			}
			// tries near, then goes far
			else {
				near->traverse(ray, t_min, t_split);
			    	if(ray.t < t_split) 
                                        return;
			    	return far->traverse(ray, t_split, t_max);	
			}
		}
				
		
		virtual int depth(int d) const {
			return max(left->depth(d+1), right->depth(d+1));			
		}
		
		
		virtual ~InnerNode() {};
	};

	struct LeafNode : public Node{
		vector<Triangle *> T;
		Box V;

		LeafNode(vector<Triangle *>& T0, const Box& V0) : T(T0) {
			V = V0;
		}

		virtual void traverse(Ray &ray, float t_min, float t_max){
			(void) t_min;
			(void) t_max;
			for (size_t i = 0; i < T.size(); i++)	
				T[i]->Intersect(ray);
		}
		
		virtual int depth(int d) const {
			return d;
		}		

		virtual ~LeafNode() {};
	};



	struct Event {
		typedef enum { endsOnPlane=0, liesOnPlane=1, startsOnPlane=2  }
                        EventType;
		Triangle* eTri;
		Plane p;
		EventType type;
		Event(Triangle* newTri, int k, float point, EventType newType) :
			eTri(newTri), type(newType) {

			assert(type == endsOnPlane or 
                               type == liesOnPlane or
                               type == startsOnPlane);

			p = Plane(k, point);
		}
		inline bool operator<(const Event& e) const {
			return((p.oint < e.p.oint) or  
                               (p.oint == e.p.oint && type < e.type));
		}


	};

	typedef vector<Triangle*> Triangles;
	typedef enum { LEFT=-1, RIGHT=1, UNKNOWN=0 } PlaneSide;
	
        inline bool  shouldStop(int N, float minCv)                 const;
        inline float area(const Box& V)                             const;
	inline float collision_prob(const Box&, const Box&)         const;
	inline float lambda(int , int , float , float )             const;
	inline float C(float , float, int , int )                   const;
	       void  splitBox(const Box&, const Plane&, Box&, Box&) const;

	       void  SAH(const Plane& , const Box& , int , int ,
                               int, float& , PlaneSide& )           const;

	       Box   clipTriangleToBox(Triangle* , const Box& )     const;

	       void  findPlane(const Triangles& , const Box& , 
			       int,  Plane& , float&, PlaneSide& )  const;

	       void  partitionTriangles(const Triangles& , const Plane& ,
	                                      const PlaneSide& , Triangles& ,
	                                           Triangles& )     const;

	       Node *buildTree(Triangles T, Box &, int, const Plane& );

               void  findMinCost(size_t& Ei, vector<Event>& events, int& numL,
                                 int& numP, int& numR, float& C_est, 
                                 Plane& p_est, const Box& V, 
                                 PlaneSide& pside_est)              const;
	Node *root;
	Box bbox;
	float ki; // triangle intersection cost
	float kt; // traversal cost
	int maxdepth; 
	int num_nodes; 


public:
	KDTree() {}
	KDTree(Box topBox, vector<Triangle *> triangles);
	bool Intersect(Ray &ray);
	
};

	

#endif