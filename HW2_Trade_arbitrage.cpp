/*
Необходимо написать торгового советника для поиска арбитража.
Арбитраж - это торговля по цепочке различных валют в надежде заработать на небольших различиях в коэффициентах.

Формат ввода
Первая строка содержит число N – количество возможных валют (определяет размер таблицы котировок). 
Далее следует построчное представление таблицы. Диагональные элементы (i, i) пропущены (подразумевается, что курс валюты к себе же 1.0). 
В элементе таблицы (i, j) содержится обменный курс i->j. 
Если обмен в данном направлении не производится, то -1.

Формат вывода
Выведите YES, если арбитраж есть, и NO, иначе.
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cfloat>

struct Edge {
	Edge(int begin, int end, double distance) : from(begin), to(end), dist(distance) {}
	int from;
	int to;
	double dist;
};

class IGraph {
public:
	IGraph() {}
	virtual ~IGraph() {}

	virtual void AddEdge(int from, int to, double index) = 0;
	virtual int VerticesCount() const = 0;
	virtual void GetNextVertices(int vertex, std::vector<int>& vertices) const = 0;
};

class CArcGraph : public IGraph {
public:
	explicit CArcGraph(int vertexNum);
	CArcGraph(const IGraph* graph);
	~CArcGraph() {};
	int VerticesCount() const;
	void AddEdge(int from, int to, double index);
	void GetNextVertices(int vertex, std::vector<int>& vertices) const;
	void GetEdgesList(std::vector<Edge>& edges) const;
private:
	int vertexCount;
	std::vector<Edge> edgesList;
};

CArcGraph::CArcGraph(int vertexNum) {
	vertexCount = vertexNum;
}

int CArcGraph::VerticesCount() const {
	return vertexCount;
}

void CArcGraph::AddEdge(int from, int to, double index) {
	Edge temp(from, to, index);
	edgesList.push_back(temp);
}

void CArcGraph::GetNextVertices(int vertex, std::vector<int>& vertices) const {
	for (int i = 0; i < edgesList.size(); i++) {
		if (edgesList[i].from == vertex)
			vertices.push_back(edgesList[i].to);
	}
}

void CArcGraph::GetEdgesList(std::vector<Edge>& edges) const {
	for (int i = 0; i < edgesList.size(); i++) {
		edges.push_back(edgesList[i]);
}

bool has_arbitrage(CArcGraph& graph) {
	int vertices = graph.VerticesCount() + 1;
	for (int i = 0; i < vertices - 1; i++) {
		graph.AddEdge(vertices - 1, i, 0);
	}
	const double INF = DBL_MAX;
	double* Dist = new double[vertices];
	for (int i = 0; i < vertices; i++) {
		Dist[i] = INF;
	}
	Dist[vertices - 1] = 0;
	bool flag = true;
	std::vector<Edge> edges;
	graph.GetEdgesList(edges);
	for (int i = 0; i < vertices && flag; i++) {
		flag = false;
		for (unsigned int j = 0; j < edges.size(); j++) {
			int curr_from = edges[j].from;
			int curr_to = edges[j].to;
			if (Dist[curr_from] < INF && Dist[curr_from] + edges[j].dist < Dist[curr_to]) {
				Dist[curr_to] = Dist[curr_from] + edges[j].dist;
				flag = true;
			}
		}
	}
	delete[] Dist;
	return flag;
}

int main() {
	std::ifstream fin;
	fin.open("input.txt");
	int currency = 0;
	std::cin >> currency;
	std::vector<Edge> exchangeList;
	double rate = 0;
	for (int i = 0; i < currency; i++) {
		for (int j = 0; j < currency; j++) {
			if (i != j) {
				std::cin >> rate;
				if (rate != -1) {
					rate = -log(rate);
					exchangeList.emplace(exchangeList.end(), i, j, rate);
				}
			}
		}
	}
	if (has_arbitrage(exchangeList, currency)) {
		std::cout << "YES";
	}
	else {
		std::cout << "NO";
	}
	return 0;
}