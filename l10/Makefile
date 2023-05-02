all: q1 q2 q3

q1:
	g++ -fsanitize=float-divide-by-zero q1.cpp -o div0float_checked.o
	g++ q1.cpp -o div0float_unchecked.o

q2:
	g++ -O0 -fsanitize=bounds q2.cpp -o bounds_checked.o
	g++ -O0 q2.cpp -o bounds_unchecked.o

q3:
	g++ -w q3.cpp -o static_unchecked.o
	g++ -Wall -Werror -o static_checked.o

clean: 
		rm *.o