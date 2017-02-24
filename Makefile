.PHONY: compile
compile:
	mpic++  main.cpp Graph.cpp -o out

.PHONY: run
run:
	mpirun -np 3 out 
