//http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf

#include <iostream>
#include <list>
#include <vector>
using namespace std;

/*
        The Coordinate Space

      -y
      |
-x ---*-------------------- +x
      |
      |
      |
      |
  
      +y


*/

struct Point {
    float x,y;
};

struct MBR {
    Point tl,br; // top left point, bottom right point.
    int Area(){ return abs(br.x-tl.x) * abs(br.y-tl.y); }
    
    bool Contains(Point point) { return point.x>=tl.x && point.x<=br.x && point.y>=tl.y && point.y<=br.y; }

    // Returns a new Minimum Bounding Rectangle if that box also has 'object' in it.
    MBR If_Engulf(MBR object) { return MBR{
        Point{min(tl.x,object.tl.x),min(tl.y,object.tl.y)},
        Point{max(br.x,object.br.x),max(br.y,object.br.y)}
    }; }

};

struct Node;


struct NodeEntry {
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
    vector<NodeEntry> entries;
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



    void LinearSplit(Node* leaf,Node* parent)
    {
        //Find extremes on x-axis & y-axis
        float axis = 0;
        MBR box1;
        MBR box2;
        for (int i = 0; i < leaf->entries.size(); i++)
        {
            for (int y = i+1; y < leaf->entries.size(); y++)
            {
                float xdiff = abs(leaf->entries[i].box.br.x - leaf->entries[y].box.tl.x);
                if (xdiff > axis)
                {
                    axis = xdiff;
                    box1 = leaf->entries[i].box;
                    box2 = leaf->entries[y].box;
                }
                float ydiff = abs(leaf->entries[i].box.br.y - leaf->entries[y].box.tl.y);
                if (ydiff > axis)
                {
                    axis = ydiff;
                    box1 = leaf->entries[i].box;
                    box2 = leaf->entries[y].box;
                }
            }
        }
        //Make two new nodes with found boxes & remove from old node
    }

    void RecursiveInsert(Node* node, Node* parent , Point point) {
        // if(node->is_leaf == true) {return node; }
        
        // Picks a leaf node to add 'point' into.
        unsigned int min_diff = 0;
        Node* target_node = nullptr;
        
        // Find the node who's area will increase the least.
        for(NodeEntry child : node->entries) {
            

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
                target_node->entries.push_back( NodeEntry{ MBR{point,point}, {.point = point} } );
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

    void RecursiveDelete(Node* node, Point point) {
        // Find the node containing this item.
        for( NodeEntry entry : node->entries ) {

        }
    }
    void Delete(Point point) {
        RecursiveDelete(root, point);
    }

    void ClosestNeighbor(Point point) {

    }

    vector<Point> InRadius(Point point , float radius) {

    }

};
