//http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf

#include <iostream>
#include <list>
#include <vector>
#include <tuple>
using namespace std;

class Point {
    float x,y;
};

class AABB {
    float x,y,w,h;
};

class Node {
    // Parent stores children's boxes and pointers.
    list<tuple<AABB,Node>> children;
    int height;

    bool is_leaf;
    vector<Point> points;
};

class RTree {
    Node root;
    int M; // Maximum number of points per node.
    
    RTree(int M) {
        this->M = M;
    }
};
