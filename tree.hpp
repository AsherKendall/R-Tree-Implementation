//http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf

#include <iostream>
#include <list>
#include <vector>
using namespace std;

struct Point {
    float x,y;
};

struct MBR {
    Point tl,br; // top left point, bottom right point.
    int Area(){ return abs(br.x-tl.x) * abs(br.y-tl.y); }
    
    // Returns a new Minimum Bounding Rectangle if that box also has 'point' in it.
    MBR If_Engulf(Point point) { return MBR{
        Point{min(tl.x,point.x),min(tl.y,point.y)},
        Point{max(br.x,point.x),max(br.y,point.y)}
    }; }
};

struct Node;
struct BranchEntry {
    MBR box;

    // This entry contains either a child or a point.
    // This is determined by the Node holding it.
    union {
        Node* child;
        Point point;
    } data;
};

struct Node {
    // Parent stores children's boxes and pointers.
    bool is_leaf; // Whether entries' data union is either 'child' or 'point'.
    vector<BranchEntry> entries;
};


// Simplified requirements for an R-Tree.
/*
    * Every leaf node contains between m and M points unless it is the root.
    * Every non-leaf node has between m and M children unless it is the root.
    * Every entry in a non-leaf node is (I,ptr), where I is the Minimum Bounding Box
        of its children, and ptr is a pointer to the child.
    * The root node has at least two children unless it is a leaf.
    * All leaves appear on the same level.
*/
class RTree {
    Node* root;
    int M; // Maximum number of entries per node. An entry is a child node or a point.
    
    RTree(int M) {
        this->M = M;
        root = new Node;
        root->is_leaf = true;
    }

    void RecursiveInsert(Node* node, Node* parent , Point point) {
        // if(node->is_leaf == true) {return node; }
        
        // Picks a leaf node to add 'point' into.
        unsigned int min_diff = 0;
        Node* target_node = nullptr;
        
        // Find the node who's area will increase the least.
        for(BranchEntry child : node->entries) {
            

            int area_before = child.box.Area();

            MBR new_box = child.box.If_Engulf(point);
            int new_box_area = new_box.Area();

            int diff = new_box_area - area_before;
            
            if(target_node == nullptr) {
                target_node = child.data.child; // This is fine because of the node->is_leaf check above.
                min_diff = diff;
                continue;
            }else if(min_diff > diff) { // TODO: prefer nodes with less entries.
                min_diff = diff;
            }
        }

        // Adjust Node
        
        if (target_node == nullptr || target_node->is_leaf) {
            // If there is room, add the point.
            //   Else, split and add the point to one of them.
            if(target_node->entries.size()+1 <= M){
                target_node->entries.push_back( BranchEntry{ MBR{point,point}, {.point = point} } );
            }else {
                // split
            }
        }
        // adjust this node. (Potentially split)
        if( node->entries.size() > M) {
            // Child split, time to split.
        }
    }

    void Insert(Point point) {
        RecursiveInsert(root, nullptr ,point);
    }
};
