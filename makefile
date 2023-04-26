all : tree

raylib :
	git clone https://github.com/raysan5/raylib.git
raylib/build/raylib/libraylib.a : raylib
	cd raylib && git checkout tags/4.5.0 && mkdir build && cd build && cmake ../ && make

raygui :
	git clone https://github.com/raysan5/raygui.git

tree : test.cpp tree.hpp raygui raylib/build/raylib/libraylib.a
	g++ -o tree -g test.cpp ./raylib/build/raylib/libraylib.a -I./raylib/src -I./raygui/src -ldl -lpthread -lm 
