#include <iostream>
#include <mpi.h>
#include <unistd.h>     /* sleep */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "main.hpp"
#include "Graph.cpp"

using namespace std;


/**
** Possible tags
**/
int const REQUEST_TOKEN_TAG = 1;
int const FORK_TAG = 2;

int randomNumber = 1;

enum state { eating, thinking, hungry };

state currentState = thinking;

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
    Graph graph("h.conf");
    if(totalNodes != graph.getVertexCount()){
        cout << "the number of processes should match the number of philosophers" <<endl;
        cout << "the program will now close"<<endl;
        MPI_Finalize();
        exit(0);
    }
    reachableNodes = graph.getConnectedNodes(id);
    initializeForksAndRequests();
    rules();
    MPI_Finalize();
}
void rules(){
	while(1){
        //random hungry

        int random;
        //making every seed unique from a process to another using id
        srand (time(NULL) + id*2);
        random = rand() % 10 + 1;
        usleep(random * 200); 
        currentState = hungry;
        cout << "philosopher at node " << id << " hungry " << endl;
        
        if(rule1() || rule2()){
            cout << "process: "<< id << "satisfies rule 1 or rule 2" <<endl;
            continue;
        }
        if(rule3()) {
            cout<< "process: "<< id << "satisfies rule 3" <<endl;
           // continue;
            
        }
        rule4();		
        //if i am in this state
        bool allForksClean = true;
        for(int i=0; i < reachableNodes.size(); i++){
            //check that all forks are clean
            if(dirtyForks[i]) allForksClean = false;
        }
        if(allForksClean){
            currentState = eating;
            cout << "philosopher at node " << id << " is eating" << endl;
            sleep(1);
            for(int i=0; i < dirtyForks.size(); i++){
                dirtyForks[i] = true;
            }
            currentState = thinking;
            sleep(1);
        }else {
            cout<< "error this should never reach this state"<<endl;
        }

	}
}

void initializeForksAndRequests(){

    int size = reachableNodes.size();
    dirtyForks.reserve(size);
    holdForks.reserve(size);
    holdRequestTokenForks.reserve(size);
    for(int i =0 ;i< reachableNodes.size();i++){
        dirtyForks[i] = true;
        if(id > reachableNodes[i]){ 
            holdForks[i] = true;
            holdRequestTokenForks[i] = false;

        }
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
bool rule1(){
    bool sendHappened = false;
    if (currentState == hungry){
        for(int i=0; i<reachableNodes.size(); i++){
            if(holdRequestTokenForks[i] && !holdForks[i]){
                //send request token to the philosopher with whom this is shared
                MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i],REQUEST_TOKEN_TAG, MPI_COMM_WORLD);
                cout << "sending request token to process: " << reachableNodes[i] <<endl;
                holdRequestTokenForks[i] = false;
                sendHappened = true;
            }
        }
    }
    return sendHappened;

}

bool rule2(){
    bool sendHappened = false;
    if(currentState != eating) {
        for(int i=0;i<reachableNodes.size();i++){
            if(holdRequestTokenForks[i] && dirtyForks[i]){
                //send fork f to the philosopher with whom fork f is shared
                MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i], FORK_TAG, MPI_COMM_WORLD);
                dirtyForks[i] = false;
                holdForks[i] = false;
                sendHappened = true;
            }
        }
    }
    return sendHappened;
}
bool rule3(){
    //waiting to receive request token from f
    //once that is received 
    bool receiveHappened = false;
    for(int i=0; i < reachableNodes.size(); i++){ 
        if(!holdRequestTokenForks[i]){
            cout << "process: "<< id << "is wating in rule3"<< "on fork: "<< i  <<endl;
            MPI_Recv(&randomNumber, 1, MPI_INT, reachableNodes[i], REQUEST_TOKEN_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //received request token
            holdRequestTokenForks[i] = true;
            cout << "i must go to outer" << endl;
            receiveHappened = true;
        }
    }
    return receiveHappened;
}
void rule4(){
    //wait to receive a fork f
    cout << "process: "<< id << "is wating in rule4" <<endl;
    for (int i=0; i < reachableNodes.size(); i++){
        if(!holdForks[i]){
            MPI_Recv(&randomNumber, 1, MPI_INT, reachableNodes[i], FORK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //fork received , it should be clean , every fork sent must be clean
            holdForks[i] = true;
            dirtyForks[i] = false;
        }
    }
}

