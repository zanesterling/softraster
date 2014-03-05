demo: demo.cpp surface.o renderer.o matrix.o
	g++ -o demo demo.cpp surface.o renderer.o matrix.o -lSDL2

parser: parser.cpp surface.o renderer.o
	g++ -o parser parser.cpp surface.o renderer.o -lSDL2

surface.o: surface.cpp surface.h
	g++ -c surface.cpp -lSDL2

renderer.o: renderer.cpp renderer.h
	g++ -c renderer.cpp -lSDL2

matrix.o: matrix.cpp matrix.h
	g++ -c matrix.cpp -lSDL2

run: demo
	./demo
