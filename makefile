all:
	gcc single_cycle_simulator.cpp -o simulator -std=c++11 -lstdc++

clean:
	rm -f ./a.out
