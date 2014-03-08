GCC=g++
FLAGS=-I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

parser: parser.cpp renderer.o matrix.o transform.o
	$(GCC) -o parser parser.cpp renderer.o matrix.o transform.o $(FLAGS)

demo: demo.cpp renderer.o matrix.o transform.o
	$(GCC) -o demo demo.cpp renderer.o matrix.o $(FLAGS)

renderer.o: renderer.cpp renderer.h
	$(GCC) -c renderer.cpp $(FLAGS)

matrix.o: matrix.cpp matrix.h
	$(GCC) -c matrix.cpp $(FLAGS)

transform.o: transform.cpp transform.h
	$(GCC) -c transform.cpp $(FLAGS)

run: parser
	./parser

clean:
	rm -f *.o
	rm -f demo
	rm -f parser
