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

struct Point {
    float x,y;
    // MBR GetMBR() {return MBR{Point{x,y},Point{x,y}};}
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

MBR GetMBR(Point point) {
    float margin = 1.0f;
    return MBR{Point{point.x-margin,point.y-margin},Point{point.x+margin,point.y+margin}};
}



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

    ~Node() {
        for(NodeEntry entry : entries ) {
            if(!is_leaf) {
                delete entry.data.child;
            }
        }
        entries.clear();
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
    NodeEntry root;
    int M; // Maximum number of entries per node. An entry is a child node or a point.
    
    // Given a node and a point, returns index of child of node that will change its area the least by engulfing it.
    size_t GetLeastAreaChange(Node* node, Point point) {
        assert(node->is_leaf == false);

        unsigned int min_diff = 0;
        size_t target_node = -1; // max integer
        for(int i = 0; i < node->entries.size(); i++) {
            NodeEntry* child = &node->entries[i];

            int area_before = child->box.Area();

            MBR new_box = child->box.If_Engulf( GetMBR(point) );
            int new_box_area = new_box.Area();

            int diff = new_box_area - area_before;
            assert(diff >= 0);
            
            if(target_node == -1) {
                target_node = i; // This is fine because of the node->is_leaf check above.
                min_diff = diff;
                continue;
            }else if(min_diff > diff) { // TODO: prefer nodes with less entries.
                min_diff = diff;
                target_node = i;
            }
        }
        return target_node;
    }

    void LinearSplit(size_t leaf_index,Node* parent)
    {
        Node* leaf = parent->entries[leaf_index].data.child;
        //Find extremes on x-axis & y-axis
        float axis = 0;
        int indexi;
        int indexy;
        for (int i = 0; i < leaf->entries.size()-1; i++)
        {
            for (int y = i+1; y < leaf->entries.size(); y++)
            {
                float xdiff = max(
                    abs(leaf->entries[i].box.br.x - leaf->entries[y].box.tl.x),
                    abs(leaf->entries[i].box.tl.x - leaf->entries[y].box.br.x)
                );
                if (xdiff > axis)
                {
                    axis = xdiff;
                    indexi = i;
                    indexy = y;
                }
                float ydiff = max(
                    abs(leaf->entries[i].box.br.y - leaf->entries[y].box.tl.y),
                    abs(leaf->entries[i].box.tl.y - leaf->entries[y].box.br.y)
                );
                if (ydiff > axis)
                {
                    axis = ydiff;
                    indexi = i;
                    indexy = y;
                }
            }
        }
        //Make two new nodes with found boxes
        NodeEntry node1 = NodeEntry();
        node1.box = leaf->entries[indexi].box;
        node1.data.child = new Node();
        node1.data.child->is_leaf = leaf->is_leaf;
        NodeEntry node2 = NodeEntry();
        node2.box = leaf->entries[indexy].box;
        node2.data.child = new Node();
        node2.data.child->is_leaf = leaf->is_leaf;
        
        //Remove from old leaf
        node1.data.child->entries.push_back(leaf->entries[indexi]);
        node2.data.child->entries.push_back(leaf->entries[indexy]);
        leaf->entries.erase(leaf->entries.begin() + indexi);
        //                  compensates for the array shift   v
        leaf->entries.erase(leaf->entries.begin() + (indexi<indexy?indexy-1:indexy) );

        for (int i = 0; i < leaf->entries.size(); i++)
        {
            MBR new_box1 = node1.box.If_Engulf(leaf->entries[i].box);
            MBR new_box2 = node2.box.If_Engulf(leaf->entries[i].box);
            int diff1 = new_box1.Area() - node1.box.Area();
            int diff2 = new_box2.Area() - node2.box.Area();
            if ( diff1 <  diff2)
            {
                node1.data.child->entries.push_back(leaf->entries[i]);
                node1.box = new_box1;
            }else
            {
                node2.data.child->entries.push_back(leaf->entries[i]);
                node2.box = new_box2;
            }
            

        }

        parent->entries.erase(parent->entries.begin() + leaf_index);
        parent->entries.push_back(node1);
        parent->entries.push_back(node2);

        //Remove old node
        leaf->entries.clear();
        delete leaf;
    }

    void RecursiveInsert(Node* node , Point point) {
        // Picks a leaf node to add 'point' into.
        NodeEntry* target_node = nullptr;
        size_t target_node_index = 0;
        // Find the node who's area will increase the least.
        if(node->is_leaf == false) {
            target_node_index = GetLeastAreaChange(node, point);
            target_node = &node->entries[target_node_index] ;
        }else{
            assert(node == root.data.child);
            target_node = &root;
        }
        assert(target_node != nullptr);

        // Adjust Node
        if(target_node->data.child->is_leaf == false){
            RecursiveInsert(target_node->data.child, point);
        }
        else {
            // Add the point to the child.
            target_node->data.child->entries.push_back( NodeEntry{ GetMBR(point), {.point = point} } );
            target_node->box = target_node->data.child->GetMBR();
        }
        // If there is not room, split it.
        if(target_node->data.child->entries.size() > M) {
            // split
            if(target_node != &root) {
                LinearSplit(target_node_index, node);
            }

            if(target_node == &root || node == root.data.child) {
                NodeEntry new_root;
                new_root.data.child = new Node;
                new_root.data.child->is_leaf = false;
                new_root.data.child->entries.push_back(root);
                LinearSplit(0, new_root.data.child);
                root = new_root;
                root.box = root.data.child->GetMBR();
            }
        }
    }

    
    void RecursivePrintTree(Node* node) {
        cout << "[ ";
        for(NodeEntry entry : node->entries) {
            if(node->is_leaf) {
                cout << "(" << entry.data.point.x << ", " << entry.data.point.y << ") ";
            }else {
                RecursivePrintTree(entry.data.child);
            }
        }
        // cout << node->data << " ";  
        // RecursivePrintTree(node->left);
        // cout << ", ";
        // RecursivePrintTree(node->right);
        cout << " ]";
    }

    void RecursiveGetAllData(Node* node, vector<Point>* to_be_deleted) {

        for( NodeEntry leaf_entry : node->entries) {
            if(node->is_leaf == true) {
                to_be_deleted->push_back(leaf_entry.data.point);
            }else{
                RecursiveGetAllData(leaf_entry.data.child, to_be_deleted);
            }
        }
    }

    // Returns whether the object is in this node path.
    bool RecursiveDelete(Node* node, Point point, vector<Point>* to_be_deleted) {
        bool ret = false;

        bool removed_entry = false;
        size_t entry_to_die;
        // Find the node containing this item.
        for( size_t i = 0;i < node->entries.size(); i++ ) {
            NodeEntry entry = node->entries[i];

            if( !entry.box.Contains( GetMBR(point) ) ) {
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

                    // if(entry.data.child->is_leaf) {
                        // Copy their points (objects) into to_be_deleted.
                        // for( NodeEntry leaf_entry : entry.data.child->entries) {
                        //     to_be_deleted->push_back(leaf_entry.data.point);
                        // }
                    RecursiveGetAllData(entry.data.child, to_be_deleted);
                    // }
                    
                    // KILL
                    entry_to_die = i;
                    removed_entry = true;
                    break;
                }

            }else if(point == entry.data.point) {
                // all entries are points.
                // So that means that this leaf node contains this point, which is this entry! Remove it.
                node->entries.erase( node->entries.begin() + i );
                return true; // Return true because we found the entry to be removed.
            }
        }

        // Destroy too-small entries.
        // for(size_t entry : entries_to_die) {
        if(removed_entry == true) {
            auto entrys_iter = node->entries.begin() + entry_to_die;
            delete entrys_iter->data.child;
            node->entries.erase(entrys_iter);
        }

        if(node == root.data.child && node->entries.size() == 1) {
            root = node->entries[0];
            node->entries.clear();
            delete node;
        }

        return ret;
    }

public:

    void Insert(Point point) {
        RecursiveInsert(root.data.child ,point);
        root.box = root.data.child->GetMBR();
    }

    void Delete(Point point) {
        vector<Point> to_be_deleted;
        RecursiveDelete(root.data.child, point, &to_be_deleted);
        root.box = root.data.child->GetMBR();
        for(Point point : to_be_deleted) {
            Insert(point);
        }
    }

    // void ClosestNeighbor(Point point) {

    // }

    // vector<Point> InRadius(Point point , float radius) {

    // }
    void PrintTree() {
        RecursivePrintTree(root.data.child);
    }

    RTree(int M) {
        this->M = M;
        root.data.child = new Node;
        root.box = MBR{Point{-1,-1},Point{1,1}};
        root.data.child->is_leaf = true;
        // root = new Node;
        // root->is_leaf = true;
    }
};
