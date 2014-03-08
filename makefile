parser: parser.cpp surface.o renderer.o matrix.o
	g++ -o parser parser.cpp surface.o renderer.o matrix.o -I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

demo: demo.cpp surface.o renderer.o matrix.o
	g++ -o demo demo.cpp surface.o renderer.o matrix.o -I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

surface.o: surface.cpp surface.h
	g++ -c surface.cpp -I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

renderer.o: renderer.cpp renderer.h
	g++ -c renderer.cpp -I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

matrix.o: matrix.cpp matrix.h
	g++ -c matrix.cpp -I/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2

run: parser
	./parser

clean:
	rm -f *.o
	rm -f demo
	rm -f parser
