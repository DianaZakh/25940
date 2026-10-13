all: build/1.out\
     build/2.out\
     build/3.out\
     build/4.out\
		 build/5.out\
		 build/6.out\
		 build/7.out\

clean: 
	rm -r build
build/1.out: 1/main.c | build
	cc 1/main.c -o build/1.out
build/2.out: 2/ex_time.c | build
	cc 2/ex_time.c -o build/2.out
build/3.out: 3/main.c | build
	cc 3/main.c -o build/3.out
build/4.out: 4/main.c | build
	cc 4/main.c -o build/4.out
build/5.out: 5/main.c | build
	cc 5/main.c -o build/5.out
build/6.out: 6/main.c | build
	cc 6/main.c -o build/6.out
build/7.out: 7/main.c | build
	cc 7/main.c -o build/7.out
build:
	mkdir -p build
