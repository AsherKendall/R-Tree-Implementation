#include "tree.hpp"

int main() {
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

    tree.PrintTree();
    std::cout << "\n";
    return 0;
}