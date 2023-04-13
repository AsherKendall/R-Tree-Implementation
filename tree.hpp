//http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf

#include <iostream>
#include <list>
#include <vector>
#include <assert.h>
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

struct MBR;
struct Point {
    float x,y;
    MBR GetMBR() {return MBR{Point{x,y},Point{x,y}};}
    bool operator==(Point rhs) { return x==rhs.x&&y==rhs.y;}
};

struct MBR {
    Point tl,br; // top left point, bottom right point.
    int Area(){ return abs(br.x-tl.x) * abs(br.y-tl.y); }
    
    bool Contains(MBR object) { return object.tl.x>=tl.x && object.br.x<=br.x && object.tl.y>=tl.y && object.br.y<=br.y; }

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
    MBR GetMBR() {
        assert(entries.size() > 0);
        MBR new_mbr = entries[0].box;
        for( int i = 1;i < entries.size(); i++ ) {
            new_mbr = new_mbr.If_Engulf(entries[i].box);
        }
        return new_mbr;
    }
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
    MBR root_box;
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
        int indexi;
        int indexy;
        for (int i = 0; i < leaf->entries.size(); i++)
        {
            for (int y = i+1; y < leaf->entries.size(); y++)
            {
                float xdiff = abs(leaf->entries[i].box.br.x - leaf->entries[y].box.tl.x);
                if (xdiff > axis)
                {
                    axis = xdiff;
                    indexi = i;
                    indexy = y;
                }
                float ydiff = abs(leaf->entries[i].box.br.y - leaf->entries[y].box.tl.y);
                if (ydiff > axis)
                {
                    axis = ydiff;
                    indexi = i;
                    indexy = y;
                }
            }
        }
        //Make two new nodes with found boxes
        NodeEntry* node1 = new NodeEntry();
        node1->box = leaf->entries[indexi].box;
        node1->data.child = new Node();
        NodeEntry* node2 = new NodeEntry();
        node2->box = leaf->entries[indexy].box;
        node2->data.child = new Node();
        

        //Remove from old leaf
        leaf->entries.erase(leaf->entries.begin() + indexi);
        leaf->entries.erase(leaf->entries.begin() + indexy);

        
        //
        for (int i = 0; i < leaf->entries.size(); i++)
        {
            MBR newBox1 = node1->box.If_Engulf(leaf->entries[i].box);
            MBR newBox2 = node2->box.If_Engulf(leaf->entries[i].box);
            if (newBox1.Area() - node1->box.Area() > newBox2.Area() - node2->box.Area())
            {
                node1->data.child->entries.push_back(leaf->entries[i]);
            }else
            {
                node2->data.child->entries.push_back(leaf->entries[i]);
            }
            

        }

        parent->entries.push_back(*node1);
        parent->entries.push_back(*node2);

        //Remove old node
        leaf->entries.clear();
        delete leaf;
    }

    void RecursiveInsert(Node* node , Point point) {
        // if(node->is_leaf == true) {return node; }
        
        // Picks a leaf node to add 'point' into.
        unsigned int min_diff = 0;
        Node* target_node = nullptr;
        
        // Find the node who's area will increase the least.
        for(NodeEntry child : node->entries) {
            

            int area_before = child.box.Area();

            MBR new_box = child.box.If_Engulf( point.GetMBR() );
            int new_box_area = new_box.Area();

            int diff = new_box_area - area_before;
            assert(diff >= 0);
            
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
        RecursiveInsert(root ,point);
    }

    void PrintTree() {

    }

    // Returns whether the object is in this node path.
    bool RecursiveDelete(Node* node, Point point, vector<Point>* to_be_deleted) {
        bool ret = false;
        vector<size_t> entries_to_die;
        // Find the node containing this item.
        for( size_t i = 0;i < node->entries.size(); i++ ) {
            NodeEntry entry = node->entries[i];

            if( !entry.box.Contains( point.GetMBR() ) ) {
                continue;
            }
            // Entries are either more nodes or points.
            if(node->is_leaf == false){
                // all entries are nodes.
                bool found_leaf = RecursiveDelete(entry.data.child, point, to_be_deleted);
                if(!found_leaf) {
                    continue; // We don't care about this node path.
                }
                ret = true; // We will return true in the future.
                entry.box = entry.data.child->GetMBR();
                // The object had a deletion! Check if it is too small.
                if(entry.data.child->entries.size() < M/2) {

                    if(entry.data.child->is_leaf) {
                        // Copy their points (objects) into to_be_deleted.
                        for( NodeEntry leaf_entry : entry.data.child->entries) {
                            to_be_deleted->push_back(leaf_entry.data.point);
                        }
                    }
                    
                    // KILL
                    entries_to_die.push_back(i);
                }

            }else if(point == entry.data.point) {
                // all entries are points.
                // So that means that this leaf node contains this point, which is this entry! Remove it.
                node->entries.erase( node->entries.begin() + i );
                return true; // Return true because we found the entry to be removed.
            }
        }

        // Destroy too-small entries.
        for(size_t entry : entries_to_die) {
            node->entries.erase(node->entries.begin() + entry);
        }

        if(node == root && node->entries.size() == 1) {
            root = node->entries[0].data.child;
            node->entries.clear();
            delete node;
        }

        return ret;
    }

    void Delete(Point point) {
        vector<Point> to_be_deleted;
        RecursiveDelete(root, point, &to_be_deleted);
        for(Point point : to_be_deleted) {
            Insert(point);
        }
    }

    void ClosestNeighbor(Point point) {

    }

    vector<Point> InRadius(Point point , float radius) {

    }

};
