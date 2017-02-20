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
int const FORK_TAG_DIRTY = 3;
int const VALUE_TRUE = 1;
int const VALUE_FALSE = 1;

int randomNumber = 1;

enum state { eating, thinking, hungry };

state currentState = thinking;

int id;
int totalNodes;

//contains the ids of the reachable nodes
vector<int> reachableNodes;

bool* holdForks;
bool* dirtyForks;
bool* holdRequestTokenForks;

MPI_Request rule3Request;
MPI_Request rule4Request;

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
    //the first time you start you are bound to get hungry
    rules();
    MPI_Finalize();
}
void startGettingHungry(){
    if(currentState == hungry) return;

    int random;
    //making every seed unique from a process to another using id
    srand (time(NULL) + id+2);
    random = rand() % 10 + 1;
    //cout << "random value " << random <<endl;

    if(random < 6) { currentState = hungry; cout << "hungry philosopher at node: " << id << endl; }
    else cout << "thinking philosopher at node: " << id << endl;
}
void rules(){
    bool firstTime = true;
	while(1){

        //random hungry
        if(!firstTime) startGettingHungry(); 
        else firstTime = false;

        if(rule1() || rule2()){
            //cout << "process: "<< id << " satisfies rule 1 or rule 2" <<endl;
            continue;
        }
        rule3();
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
            for(int i=0; i < reachableNodes.size(); i++){
                dirtyForks[i] = true;
            }
            currentState = thinking;
            cout << "philosopher at node " << id << " is thinking" << endl;
        }
	}
}

void initializeForksAndRequests(){

    int size = reachableNodes.size();
    dirtyForks = new bool [size];
    holdForks = new bool [size];
    holdRequestTokenForks = new bool [size];
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
                MPI_Send(&VALUE_TRUE,1,MPI_INT,reachableNodes[i],REQUEST_TOKEN_TAG, MPI_COMM_WORLD);
                //cout << "sending request token to process: " << reachableNodes[i] <<endl;
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
                //cout << "process: " << id << " sending fork to process: " << reachableNodes[i]<<endl; 
                //set the boolean of having fork to false and dirty to false
                MPI_Send(&VALUE_TRUE,1,MPI_INT,reachableNodes[i], FORK_TAG, MPI_COMM_WORLD);
                MPI_Send(&VALUE_FALSE,1,MPI_INT,reachableNodes[i], FORK_TAG_DIRTY, MPI_COMM_WORLD);
                dirtyForks[i] = false;
                holdForks[i] = false;
                sendHappened = true;
            }
        }
    }
    return sendHappened;
}
void rule3(){
    //waiting to receive request token from f
    //once that is received 
    for(int i=0; i < reachableNodes.size(); i++){ 
        //cout << "process: "<< id << " is wating in rule3 on fork: "<< reachableNodes[i]  <<endl;
        MPI_Irecv(&holdRequestTokenForks[i], 1, MPI_INT, reachableNodes[i], REQUEST_TOKEN_TAG, MPI_COMM_WORLD,
        &rule3Request);
        //received request token
        holdRequestTokenForks[i] = true;
        //cout << "process: "<< id << " received token " << "from: "<< reachableNodes[i] << endl;
    }
}
void rule4(){
    //wait to receive a fork f
    //cout << "process: "<< id << " is wating in rule4" <<endl;
    for (int i=0; i < reachableNodes.size(); i++){
        MPI_Irecv(&holdForks[i], 1, MPI_INT, reachableNodes[i], FORK_TAG, MPI_COMM_WORLD, &rule4Request);
        MPI_Irecv(&dirtyForks[i], 1, MPI_INT, reachableNodes[i], FORK_TAG_DIRTY, MPI_COMM_WORLD, &rule4Request);

        //those two values are filled asynchronously
        //holdForks[i] = true;
        //dirtyForks[i] = false;
    }
}

