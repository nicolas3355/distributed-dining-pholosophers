#include <iostream>
#include <mpi.h>
#include "main.hpp"
#include "Graph.cpp"
using namespace std;


/**
** Possible tags
**/
int const REQUEST_TOKEN_TAG = 1;
int const FORK_TAG = 2;

int randomNumber = 1;

bool eating = false;
bool thinking = true;
bool hungry = false;
/*bool holdFork = false;
bool holdRequestTokenFork = false;
bool forkIsDirty = false;*/

int id;
int totalNodes;

//contains the ids of the reachable nodes
vector<int> reachableNodes;

vector<bool> holdForks;
vector<bool> dirtyForks;
vector<bool> holdRequestTokenForks;


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
    initializeForksAndRequests();
    MPI_Finalize();
}
void rules(int processNumber){
	outer:while(1){
	    if(processNumber == 1){
            cout << "hello from" << processNumber << "\n";
        }
        for (int i=0 ;i<reachableNodes.size();i++){
            if(rule1(i) || rule2(i)){
                goto outer;
            }
            rule3(i);
            rule4(i);		
        }
        //if i am in this state
        for(int i=0;i<reachableNodes.size();i++){
            //check that all forks are clean
        }
        //if clean then start eating
        //once done eating
        //eating false
        //hungry false
        //thinking true
	}
}

void initializeForksAndRequests(){

    int size = reachableNodes.size();
    dirtyForks.reserve(size);
    holdForks.reserve(size);
    holdRequestTokenForks.reserve(size);

    for(int i =0 ;i< reachableNodes.size();i++){
        dirtyForks[i] = true;
        if(id > reachableNodes[i]) holdForks[i] = true;
        else { 
            holdForks[i] = false;
            holdRequestTokenForks[i] = true;
        }
    }

}


/**
*   i represent the index of the fork , and represent the index of the philosopher
*
**/
bool rule1(int i){
    if (hungry && holdRequestTokenForks[i] && !holdForks[i]){
        //send request token to the philosopher with whom this is shared
        MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i],REQUEST_TOKEN_TAG, MPI_COMM_WORLD);
        cout << "sending request token to x";
        holdRequestTokenForks[i] = false;
        return true;
    }
    return false;

}

bool rule2(int i){
    if(!eating && holdRequestTokenForks[i] && dirtyForks[i]){
        //send fork f to the philosopher with whom fork f is shared
        MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i], FORK_TAG, MPI_COMM_WORLD);
        dirtyForks[i] = false;
        holdForks[i] = false;
        return true;
    }
    return false;
}
void rule3(int i){
    //waiting to receive request token from f
    //once that is received 
    MPI_Recv(&randomNumber, 1, MPI_INT, reachableNodes[i], REQUEST_TOKEN_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    holdRequestTokenForks[i] = true;
}
void rule4(int i){
    //wait to receive a fork f
    MPI_Recv(&randomNumber, 1, MPI_INT, reachableNodes[i], FORK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    holdForks[i] = true;
    dirtyForks[i] = false;
    //fork should not be dirty
    //start eating and do shit

}

