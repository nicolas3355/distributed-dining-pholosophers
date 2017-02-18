#include <iostream>
#include <mpi.h>
#include "main.hpp"
#include "Graph.cpp"
using namespace std;


bool eating = false;
bool thinking = false;
bool hungry = false;
bool holdFork = false;
bool holdRequestTokenFork = false;
bool forkIsDirty = false;

int id;
int totalNodes;
std::vector<int> reachableNodes;

int main(int argc, char ** argv)
{
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    //cout << "Hello world from process " << mynode;
    //cout << " of " << totalnodes << endl;
    //rules(mynode);
    //Graph* graph= new Graph("h.conf");
    Graph graph("h.conf");
    if(totalNodes != graph.getVertexCount()){
        cout << "the number of processes should match the number of philosophers" <<endl;
        cout << "the program will now close";
        exit(0);
    }
    reachableNodes = graph.getConnectedNodes(id);
    for (int i=0;i<reachableNodes.size();i++){
        cout<< "element"<< id<< "has edges to " << reachableNodes[i] <<endl;
    }
    MPI_Finalize();
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

