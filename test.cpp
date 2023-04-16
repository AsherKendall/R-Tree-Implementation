#include <iostream>
#include <cstdlib>
#include <time.h>

#include "tree.hpp"
#include "raylib/include/raylib.h"

using namespace std;

const int window_x = 1000;
const int window_y = 800;
const int margin = 50;
const int SCALE = window_x/10;

Color RandomColor(){
    return Color{(unsigned char)(rand()%256),(unsigned char)(rand()%256),(unsigned char)(rand()%256),255};
}

void DrawNode(NodeEntry entry){
    Color color = RandomColor();
    DrawRectangleLines(entry.box.tl.x * SCALE,entry.box.tl.y * SCALE, (entry.box.br.x-entry.box.tl.x) * SCALE,(entry.box.br.y-entry.box.tl.y)*SCALE,color);

    for(NodeEntry child_entry : entry.data.child->entries){
        if(entry.data.child->is_leaf == true) {
            DrawCircle(child_entry.data.point.x*SCALE,child_entry.data.point.y*SCALE,3, color);
        }else{
            DrawNode(child_entry);
        }
    }
}

int main() {
    srand(time(NULL));

    

    InitWindow(window_x,window_y, "R-Trees are fun");
    SetTargetFPS(60);

    RTree tree(4);

    // This example works.
    // tree.Insert(Point{1,1});
    // tree.Insert(Point{2,2});
    // tree.Insert(Point{7,5});
    // tree.Insert(Point{8,2});
    // tree.Insert(Point{3,6});


    tree.Insert(Point{1,1});
    tree.Insert(Point{1,2});
    tree.Insert(Point{2,1});
    tree.Insert(Point{2,2});

    tree.Insert(Point{6,1});
    tree.Insert(Point{7,1});
    tree.Insert(Point{6,2});
    tree.Insert(Point{7,2});

    tree.Insert(Point{1,6});
    tree.Insert(Point{1,7});
    tree.Insert(Point{2,6});
    tree.Insert(Point{2,7});

    tree.Insert(Point{6,6});
    tree.Insert(Point{6,7});
    tree.Insert(Point{7,6});
    tree.Insert(Point{7,7});

    tree.Insert(Point{7,9});

    // vector<Point> queue = {Point{1,1},Point{1,2}};
    // size_t next = 0;
    // for(int i =0; i < 999; i++) {
    //     tree.Insert(Point{float(margin + rand() % (window_x - margin*2)), 25.0f+ float(margin + rand() % (window_y - margin*2))});
    // }
    // tree.Delete(Point{1,1});

    // tree.PrintTree();
    // std::cout << "\n";
    tree.CheckHealth();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawNode(tree.root);
    EndDrawing();
    while(!WindowShouldClose()) {
        BeginDrawing();
        // if(IsKeyPressed(KEY_RIGHT) && next < queue.size()){
        //     tree.Insert(queue[next++]);
        //     DrawNode(tree.root);
        // }else if(IsKeyPressed(KEY_LEFT) && next > 0){
        //     tree.Delete(queue[next-1]);
        //     next -= 1;
        //     DrawNode(tree.root);
        // }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}