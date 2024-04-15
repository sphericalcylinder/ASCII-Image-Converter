ascii: ascii.cpp
	clang++ ascii.cpp -std=c++20 -o ascii `pkg-config gtkmm-4.0 --cflags --libs` gui.cc worker.cc extras.cc
