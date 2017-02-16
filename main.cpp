#include <iostream>
//#include <mpi.h>
#include "main.hpp"
#include "Graph.cpp"
using namespace std;


bool eating = false;
bool thinking = false;
bool hungry = false;
bool holdFork = false;
bool holdRequestTokenFork = false;
bool forkIsDirty = false;

int main(int argc, char ** argv)
{
    int mynode, totalnodes;
    /*MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
    cout << "Hello world from process " << mynode;
    cout << " of " << totalnodes << endl;*/
    //rules(mynode);
    //Graph* graph= new Graph("h.conf");
    Graph graph("h.conf");
    cout << "0,0" << graph.isEdge(0,0)<<endl;
    cout << "0,1" << graph.isEdge(0,1)<<endl;
    //MPI_Finalize();
}

void rules(int processNumber){
	while(1){
	    if(processNumber == 1){
            cout << "hello from" << processNumber << "\n";
        }
		if(rule1){
            continue;
		}
        if(rule2){
            continue;
        }
        rule3();
	    rule4();		
	}
}

bool rule1(){
    if (hungry && holdRequestTokenFork && !holdFork){
        //send request token to the philosopher with whom this is shared
        cout << "sending request token to x";
        holdRequestTokenFork = false;
        return true;
    }
    return false;

}

bool rule2(){
    if(!eating && holdRequestTokenFork && forkIsDirty){
        //send fork f to the philosopher with whom fork f is shared
        forkIsDirty = false;
        holdFork = false;
        return true;
    }
    return false;
}
void rule3(){
    //waiting to receive request token from f
    //once that is received 
    holdRequestTokenFork = true;
}
void rule4(){
    //wait to receive a fork f
    holdFork = true;
    forkIsDirty = false;
    //fork should not be dirty
    cout << "is the fork dirty? "<< forkIsDirty <<"\n it should be not dirty , it should be false!!!!!!!"; 
    //start eating and do shit

}

