#include <vector>

class Graph {

private:
    bool** adjacencyMatrix;
    int vertexCount;
    void readConfigFile(std::string configFile);
    void addEdge(int i, int j);
    void removeEdge(int i, int j);
    void checkUndirectedGraph();
public:
    Graph(std::string configFile);
    std::vector<int> getConnectedNodes(int node);
    bool isEdge(int i, int j);
    int getVertexCount();
    ~Graph(); 
};
