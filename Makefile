.PHONY: compile
compile:
	mpic++  main.cpp -o out

.PHONY: run
run:
	mpirun -np 10 out 
