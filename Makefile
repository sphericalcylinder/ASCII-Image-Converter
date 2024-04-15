main: main.cpp
	clang++ main.cpp -std=c++20 -o main `pkg-config gtkmm-4.0 --cflags --libs` gui.cc worker.cc extras.cc
