#include "Graph.hpp"
#include <fstream>

/***
    Graph representation using an adjacency matrix
    suppose you have 3 nodes then we can represent edges between them 
    using a matrix of size 3 * 3, and boolean to signal an edge between them 
***/



Graph::Graph(std::string configFile){
    readConfigFile(configFile);    
}


void Graph::addEdge(int i, int j){
    std::cout << "vertex count:" << vertexCount <<std::endl;
    if (i >= 0 && i < vertexCount && j >= 0 && j < vertexCount) {
        adjacencyMatrix[i][j] = true;
    }
}

//undirected graph, we remove the edge from both sides;
void Graph::removeEdge(int i, int j) {
    if (i >= 0 && i < vertexCount && j > 0 && j < vertexCount) {
        adjacencyMatrix[i][j] = false;
        adjacencyMatrix[j][i] = false;
    }
}

bool Graph::isEdge(int i, int j) {
    if (i >= 0 && i < vertexCount && j > 0 && j < vertexCount)
        //by construction adjacencyMatrix[i][j] is the same as adjacencyMatrix[j][i]
        return adjacencyMatrix[i][j];
    else
        return false;
}

Graph::~Graph() {
    for (int i = 0; i < vertexCount; i++)
        delete[] adjacencyMatrix[i];

    delete[] adjacencyMatrix;
}

void Graph::readConfigFile(std::string fileName){
    std::ifstream file(fileName);
    int columns,rows;
    file >> columns >> rows;

    if(rows != columns || rows < 2){
        std::cout << "malformed config file"; 
        exit(0);
    }

    this -> vertexCount = rows;
    adjacencyMatrix = new bool*[vertexCount];

    int tmp = 0;
    for (int i = 0; i < vertexCount; i++) {
        adjacencyMatrix[i] = new bool[vertexCount];
        for (int j = 0; j < vertexCount; j++){
            adjacencyMatrix[i][j] = false;
            file >> tmp;
            
            std::cout << "tmp:" << tmp << std::endl;
            if(tmp != 0) addEdge(i,j);
        }
    }
    checkUndirectedGraph();
}
void Graph::checkUndirectedGraph(){
    for (int i=0;i<vertexCount;i++){
        for(int j=0;j<vertexCount;j++){
            if(adjacencyMatrix[i][j] != adjacencyMatrix[j][i]){
                std::cout << "please use a directed graph" << std::endl;
                std::cout << "directed graph spotted , quitting ..." << std::endl;
                exit(0);
            }
        }
    }
}

