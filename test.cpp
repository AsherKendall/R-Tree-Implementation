#include <iostream>
#include <cstdlib>
#include <time.h>

#include "tree.hpp"
#include "raylib/include/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui/src/raygui.h"

using namespace std;

const int window_x = 1000;
const int window_y = 800;
const int margin = 25;
const float SCALE = 1;//window_x/12;
const int box_margin = 5;


Color cast_color(NotColor color) {return Color{color.r,color.g,color.b,color.a};}
// Returns the height of the node.
int DrawNode(NodeEntry entry){
    // Color color = RandomColor();
    Color color = cast_color(entry.color);
    int height = 0;
    for(NodeEntry child_entry : entry.data.child->entries){
        if(entry.data.child->is_leaf == true) {
            DrawCircle(child_entry.data.point.x*SCALE,child_entry.data.point.y*SCALE,3, color);
        }else{
            height = DrawNode(child_entry) + 1;
        }
    }
    int height_magin = box_margin*height;
    DrawRectangleLines(
        (entry.box.tl.x) * SCALE - height_magin,// X
        (entry.box.tl.y) * SCALE - height_magin, // Y
        (entry.box.br.x-entry.box.tl.x) * SCALE + height_magin*2, // width
        (entry.box.br.y-entry.box.tl.y) * SCALE + height_magin*2, // height
        color);
    return height;
}

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

    

    InitWindow(window_x,window_y, "R-Trees are fun");
    SetTargetFPS(60);

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
    for(int i =0; i < 25; i++) {
        random_data.push_back(Point{float(margin + rand() % (window_x - margin*2)),float(margin + rand() % (window_y - margin*2))});
    }

    TreeSetVector(tree,random_data);

    tree.CheckHealth();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawNode(tree.root);
    EndDrawing();
    while(!WindowShouldClose()) {
        BeginDrawing();

        // GuiPanel(Rectangle{25,108,200,200}, "This is a window");
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        if(GuiButton(Rectangle{0,0,100,40}, TextFormat("Better splitting : %d", tree.better_split))){
            tree.better_split = !tree.better_split;
            TreeSetVector(tree,random_data);
            ClearBackground(BLACK);
            DrawNode(tree.root);
        }else

        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
            Point new_point = Point{float(GetMouseX())/SCALE, float(GetMouseY())/SCALE};
            random_data.push_back(new_point);
            tree.Insert(new_point);
            ClearBackground(BLACK);
            DrawNode(tree.root);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}