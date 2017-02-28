.PHONY: compile
compile:
	mpic++  main.cpp Graph.cpp -o out

.PHONY: run
run: compile
	mpirun -np 3 out 
