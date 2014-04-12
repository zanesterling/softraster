GCC=g++
FLAGS=-I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

triangle_fill: triangle_fill.cpp renderer.o matrix.o transform.o
	$(GCC) -o triangle_fill triangle_fill.cpp renderer.o matrix.o transform.o $(FLAGS)

parser: parser.cpp renderer.o matrix.o transform.o
	$(GCC) -o parser parser.cpp renderer.o matrix.o transform.o $(FLAGS)

demo: demo.cpp renderer.o matrix.o transform.o
	$(GCC) -o demo demo.cpp renderer.o matrix.o $(FLAGS)

renderer.o: renderer.cpp headers/renderer.h
	$(GCC) -c renderer.cpp $(FLAGS)

matrix.o: matrix.cpp headers/matrix.h
	$(GCC) -c matrix.cpp $(FLAGS)

transform.o: transform.cpp headers/transform.h
	$(GCC) -c transform.cpp $(FLAGS)

clean:
	rm -f *.o
	rm -f demo
	rm -f parser
	rm -f triangle_fill
