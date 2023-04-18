all : tree
tree : test.cpp tree.hpp
	g++ -o tree -g test.cpp ./raylib/lib/libraylib.a -I./raylib/include -I./raygui/src -ldl -lpthread -lm 
