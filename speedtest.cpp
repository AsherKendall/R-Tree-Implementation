#include <iostream>
#include <cstdlib>
#include <time.h>
#include<vector>
#include <chrono>

#include "tree.hpp"

using namespace std;

const int window_x = 1000;
const int window_y = 800;
const int margin = 25;
const float SCALE = 1;//window_x/12;
const int box_margin = 5;


void TreeSetVector(RTree& tree, vector<Point>& data){
    srand(0);
    delete tree.root.data.child;
    bool better_split = tree.better_split;
    tree = RTree(4);
    tree.better_split = better_split;
    
    for(auto point : data) {
        tree.Insert(point);
    }
}

int main() {
    srand(time(NULL));

    
    using chrono::high_resolution_clock;

    RTree tree(16);
    tree.better_split = true;

    // This example works.
    vector<Point> random_data = {
    // Point{1,1},
    // Point{2,2},
    // Point{7,5},
    // Point{8,2},
    // Point{3,6},
    
    // Point{1,1},
    // Point{1,2},
    // Point{2,1},
    // Point{2,2},
    // Point{6,1},
    // Point{7,1},
    // Point{6,2},
    // Point{7,2},
    // Point{1,6},
    // Point{1,7},
    // Point{2,6},
    // Point{2,7},
    // Point{6,6},
    // Point{6,7},
    // Point{7,6},
    // Point{7,7},
    // Point{7,9},
    };
    vector<Point> queue;
    for(int i =0; i < 25; i++) {
        Point temp = Point{float(margin + rand() % (window_x - margin*2)),float(margin + rand() % (window_y - margin*2))};
        random_data.push_back(temp);
        queue.insert(queue.begin(),temp);
    }

    Point temp = queue[12];


    auto t1 = high_resolution_clock::now();
    for (size_t i = 0; i < queue.size(); i++)
    {
        if (temp == queue[i])
        {
            
            break;
        }
        
    }
    auto t2 = high_resolution_clock::now();

    cout << "Vector Search Time" << t2-t1;


    tree.FindLeaf(temp,tree);



    TreeSetVector(tree,random_data);

    tree.CheckHealth();


    return 0;
}