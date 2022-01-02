test: main.cpp engine.h
	g++ -g -Wall -Werror $< -o $@ -lncursesw
