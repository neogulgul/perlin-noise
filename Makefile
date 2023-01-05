name = o
sfml = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
args = -std=c++20 $(sfml)
srcs = $(wildcard ./src/*.cpp)

all:
	g++ $(srcs) $(args) -o $(name)
