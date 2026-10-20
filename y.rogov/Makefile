all: build/1.out\
     build/2.out\
     build/3.out\
     build/4.out\
		 build/5.out\
		 build/6.out\
		 build/7.out\

build/%.out : %/main.c
	@mkdir -p build
	cc $< -o $@

clean: 
	rm -r build

