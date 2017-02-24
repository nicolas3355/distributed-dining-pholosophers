#include <iostream>
#include <mpi.h>
#include <unistd.h>     /* sleep */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "main.hpp"
#include "Graph.hpp"
#include <fstream>
using namespace std;


/**
** Possible tags
**/
int const REQUEST_TOKEN_TAG = 1;
int const FORK_TAG = 2;

int randomNumber = 1;

enum state { initValue, eating, thinking, hungry };

state currentState = initValue;

int id;
int totalNodes;

//number of milliseconds, this will be multiplied a random number between 1 and 10 to decide how much
//the task will take time
const int waitMilli= 500;  

//contains the ids of the reachable nodes
vector<int> reachableNodes;

bool* holdForks;
bool* dirtyForks;
bool* holdRequestTokenForks;

ofstream output;

MPI_Request request;
MPI_Status status;

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
        simulateHungryCycle();
        if(rule1() || rule2()){
            output << "process: "<< id << " satisfies rule 1 or rule 2" <<endl;
            continue;
        }
        if(currentState == eating) continue;

        MPI_Recv( &randomNumber, 1 ,MPI_INT,MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD , &status);
        int senderIndex = getSenderIndex(status.MPI_SOURCE);
        if(status.MPI_TAG == REQUEST_TOKEN_TAG){
            holdRequestTokenForks[senderIndex] = true;
        } else if (status.MPI_TAG == FORK_TAG){
            holdForks[senderIndex] = true;
            dirtyForks[senderIndex] = false;
        }

    }
}
int getSenderIndex(int idOfSender){
    for(int i=0;i<reachableNodes.size();i++){
        if(idOfSender == reachableNodes[i]) return i; 
    }
    return -1;
}
//returns a random number between 1 and 10
int getRandomNumber(){
    int random;
    //making every seed unique from a process to another using id
    srand (time(NULL) + id+2);
    random = rand() % 10 + 1;
    return random;
}
void simulateHungryCycle(){
    if(currentState == initValue){
        currentState = thinking;
        output << "philosopher at node " << id << " is thinking" << endl;

    } else if(currentState == thinking){
        getHungry();
    } else if (currentState == eating){
        think();
    } else if ( currentState == hungry ){
        eat();
    }
}
void think(){
    currentState = thinking;
    output << "philosopher at node " << id << " is thinking" << endl;
    usleep(getRandomNumber()*waitMilli);
    
}
void getHungry(){
    currentState = hungry;
    output << "philosopher at node " << id << " is hungry" << endl;
}
void eat(){
    if (canEat()){
        currentState = eating;
        output << "philosopher at node " << id << " is eating" << endl;
        usleep(getRandomNumber()*waitMilli);
        for(int i=0; i < reachableNodes.size(); i++){
            dirtyForks[i] = true;
        }
    }
}

bool canEat(){
    bool haveAllForks = true;
    for(int i=0; i < reachableNodes.size(); i++){
        //check that all forks are clean
        if(!holdForks[i]) haveAllForks = false;
    }
    return haveAllForks;
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
    string fileName = to_string(id)+"_log.txt";
    output.open(fileName.c_str());
}


bool rule1(){
    bool sendHappened = false;
    if (currentState == hungry){
        for(int i=0; i<reachableNodes.size(); i++){
            if(holdRequestTokenForks[i] && !holdForks[i]){
                //send request token to the philosopher with whom this is shared
                MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i],REQUEST_TOKEN_TAG, MPI_COMM_WORLD);
                output << "sending request token to process: " << reachableNodes[i] <<endl;
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
                output << "process: " << id << " sending fork to process: " << reachableNodes[i]<<endl; 
                MPI_Send(&randomNumber,1,MPI_INT,reachableNodes[i], FORK_TAG, MPI_COMM_WORLD);
                dirtyForks[i] = false;
                holdForks[i] = false;
                sendHappened = true;
            }
        }
    }
    return sendHappened;
}


