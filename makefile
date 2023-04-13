all : tree
tree : test.cpp tree.hpp
	g++ -o tree -g test.cpp