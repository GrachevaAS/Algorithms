// Search of strongly connected components.

#include <iostream>
#include <fstream>
#include <list>
#include <stack>
#include <algorithm>
#include <assert.h>
#include <vector>

class IGraph {
public:
    IGraph() : vertexCount(0) {}
    virtual ~IGraph() {}
    virtual void AddEdge(int from, int to) = 0;
protected:
    int vertexCount;
};

class CListGraph : public IGraph {
public:
    explicit CListGraph(int vertexNum);
    ~CListGraph();
    void AddEdge(int from, int to);
    void GetNextVertices(int vertex, std::vector<int>& vertices) const;
    void GetPrevVertices(int vertex, std::vector<int>& vertices) const;
    int connectComponents();    // returns number of edges needed for strongly connected graph
private:
    std::list<int>* adjLists;
    std::list<int>* adjListsReverse;
    int* components;
    int componentsCount;
	void kosaraju();    // fills in array "components" and var. componentsCount
    void indirectDfs(std::vector<int>& order);
    void directDfsMarkingComponents(const std::vector<int>& order);
};

class CMatrixGraph : public IGraph {
public:
    explicit CMatrixGraph(int vertexNum);
    ~CMatrixGraph();
    void AddEdge(int from, int to);
    int countSourceVertices() const;
    int countSinkVertices() const;
private:
    bool** Matrix;
};

CListGraph::CListGraph(int vertexNum) {
    vertexCount = vertexNum;
    adjLists = new std::list<int>[vertexCount];
    adjListsReverse = new std::list<int>[vertexCount];
    componentsCount = 0;
    components = new int[vertexCount];
    for (int i = 0; i < vertexCount; i++) {
        components[i] = 0;
    }
}
CListGraph::~CListGraph() {
    delete[] adjLists;
    delete[] adjListsReverse;
    delete[] components;
}

void CListGraph::AddEdge(int from, int to) {
    adjLists[from].push_back(to);
    adjListsReverse[to].push_back(from);
}

void CListGraph::GetNextVertices(int vertex, std::vector<int>& vertices) const {
    for (std::list<int>::const_iterator it = adjLists[vertex].begin(); it != adjLists[vertex].end(); it++) {
        vertices.push_back(*it);
    }
}

void CListGraph::GetPrevVertices(int vertex, std::vector<int>& vertices) const {
    for (std::list<int>::const_iterator it = adjListsReverse[vertex].begin(); it != adjListsReverse[vertex].end(); it++) {
        vertices.push_back(*it);
    }
}

void CListGraph::indirectDfs(std::vector<int>& order) {
	std::stack<int> dfsStack;
	bool* visited = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
		visited[i] = false;
	for (int i = 0; i < vertexCount; i++) {
		if (!visited[i]) {
			dfsStack.push(i);
			while (!dfsStack.empty()) {
				int current = dfsStack.top();
				if (!visited[current]) {
					visited[current] = true;
					std::vector<int> tempArr;
					GetPrevVertices(current, tempArr);
					for (int j = 0; j < tempArr.size(); j++) {
						if (!visited[tempArr[j]]) {
							dfsStack.push(tempArr[j]);
						}
					}
				}
				if (current == dfsStack.top()) {
					order.push_back(current);
					dfsStack.pop();
				}
			}
		}
	}
	delete[] visited;
}

void CListGraph::directDfsMarkingComponents(const std::vector<int>& order) {
	std::stack<int> dfsStack;
	bool* visited = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
		visited[i] = false;
	componentsCount = 0;
	for (int ind = order.size() - 1; ind >= 0; ind--) {
		int i = order[ind];
		if (!visited[i]) {
			dfsStack.push(i);
			while (!dfsStack.empty()) {
				int current = dfsStack.top();
				visited[current] = true;
				components[current] = componentsCount;
				dfsStack.pop();
				std::vector<int> tempArr;
				GetNextVertices(current, tempArr);
				for (int j = 0; j < tempArr.size(); j++) {
					if (!visited[tempArr[j]])
						dfsStack.push(tempArr[j]);
				}
			}
			componentsCount++;
		}
	}
	delete[] visited;
}

void CListGraph::kosaraju() {
	std::vector<int> order;
	indirectDfs(order);
	directDfsMarkingComponents(order);
}

int CListGraph::connectComponents() {
	kosaraju();
	if (componentsCount < 2)
		return 0;
	CMatrixGraph* componentsGraph = new CMatrixGraph(componentsCount);
	for (int i = 0; i < vertexCount; i++) {
		for (std::list<int>::const_iterator it = adjLists[i].begin(); it != adjLists[i].end(); it++) {
			int j = (*it);
			if (components[i] != components[j])
				componentsGraph->AddEdge(components[i], components[j]);
		}
	}
	int sink = componentsGraph->countSinkVertices();
	int source = componentsGraph->countSourceVertices();
	int result = std::max(sink, source);
	delete componentsGraph;
	return result;
}

CMatrixGraph::CMatrixGraph(int vertexNum) {
    vertexCount = vertexNum;
    Matrix = new bool*[vertexCount];
    Matrix[0] = new bool[vertexCount * vertexCount];
    for (int i = 1; i < vertexCount; i++) {
        Matrix[i] = Matrix[i - 1] + vertexCount;
    }
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            Matrix[i][j] = false;
        }
    }
}

CMatrixGraph::~CMatrixGraph() {
    delete[] Matrix[0];
    delete[] Matrix;
}

void CMatrixGraph::AddEdge(int from, int to) {
    Matrix[from][to] = true;
}

int CMatrixGraph::countSourceVertices() const {
    int j = 0;
    int result = 0;
    for (int i = 0; i < vertexCount; i++) {
        j = 0;
        bool flag = true;
        while (j < vertexCount && flag) {
            if (Matrix[j][i] != 0)
                flag = false;
                j++;
        }
        if (flag)
            result++;
    }
    return result;
}

int CMatrixGraph::countSinkVertices() const {
    int j = 0;
    int result = 0;
    for (int i = 0; i < vertexCount; i++) {
        j = 0;
        bool flag = true;
        while (j < vertexCount && flag) {
            if (Matrix[i][j] != 0)
                flag = false;
                j++;
        }
        if (flag)
            result++;
    }
    return result;
}

int main() {
    std::ifstream fin;
    fin.open("input.txt");

    int vertices, edges = 0;
    fin >> vertices;
    fin >> edges;
    CListGraph* graph = new CListGraph(vertices);
    int from, to = 0;
    for (int i = 0; i < edges; i++) {
        fin >> from >> to;
        graph->AddEdge(from - 1, to - 1);
    }
    std::cout << graph->connectComponents();
    delete graph;
}
