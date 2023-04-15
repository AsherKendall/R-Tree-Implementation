#include "tree.hpp"

int main() {
    RTree tree(4);
    tree.Insert(Point{1,1});
    tree.Insert(Point{2,2});
    tree.Insert(Point{7,5});
    tree.Insert(Point{8,2});
    tree.Insert(Point{3,6});

    tree.PrintTree();
    std::cout << "\n";
    return 0;
}