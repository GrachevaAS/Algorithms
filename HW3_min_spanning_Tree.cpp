/* Boruvka algorithm of finding minimal spanning tree with the use of Graph_library and disconnected set union

Первая строка входного файла содержит два натуральных числа 𝑛 и 𝑚 — количество вершин
и ребер графа соответственно. Следующие 𝑚 строк содержат
описание ребер по одному на строке. Ребро номер 𝑖 описывается тремя натуральными числами - номераvb концов ребра и его весjv соответственно.
Граф является связным.
*/

#include <iostream>
#include <assert.h>
#include <vector>
#include <fstream>
#include <climits>
#include <stack>
#include <unordered_set>

class IGraph_dist {
public:
	IGraph_dist() {}
	virtual ~IGraph_dist() {}
	virtual void AddEdge(int from, int to, int dist) = 0;
	virtual int VerticesCount() const = 0;
};

class IGraph {
public:
	IGraph() {}
	virtual ~IGraph() {}
	virtual void AddEdge(int from, int to) = 0;
	virtual int VerticesCount() const = 0;
};

struct Edge {
	Edge(int begin = 0, int end = 0, int distance = 0) : from(begin), to(end), dist(distance) {}
	Edge(const Edge& another) : from(another.from), to(another.to), dist(another.dist) {}
	~Edge() {}
	int from;
	int to;
	int dist;
	void operator= (const Edge& another) {
		from = another.from;
		to = another.to;
		dist = another.dist;
	}
	bool operator< (const Edge& another) {
		return (dist < another.dist || (dist == another.dist && from < another.from));
	}
};

class CSetGraph : public IGraph {
public:
	explicit CSetGraph(int vertexNum);
	~CSetGraph();
	int VerticesCount() const;
	void AddEdge(int from, int to);
	bool HasEdge(int from, int to);
private:
	std::unordered_set<int>* adjLists;
	std::unordered_set<int>* adjListsReverse;
	int vertexCount;
};

class CArcGraph_dist : public IGraph_dist {
public:
	explicit CArcGraph_dist(int vertexNum);
	~CArcGraph_dist() {}
	int VerticesCount() const;
	int EdgesCount() const;
	void AddEdge(int from, int to, int dist);
	void AddEdge(Edge edge);
	void GetEdges(std::vector<Edge>& edges) const;
private:
	std::vector<Edge> edgesList;
	int vertexCount;
};

CSetGraph::CSetGraph(int vertexNum) {
	vertexCount = vertexNum;
	adjLists = new std::unordered_set<int>[vertexNum];
	adjListsReverse = new std::unordered_set<int>[vertexNum];
}

CSetGraph::~CSetGraph() {
	delete[] adjLists;
	delete[] adjListsReverse;
}

int CSetGraph::VerticesCount() const {
	return vertexCount;
}

bool CSetGraph::HasEdge(int from, int to) {
	return !(adjLists[from].find(to) == adjLists[from].end());
}

void CSetGraph::AddEdge(int from, int to) {
	adjLists[from].insert(to);
	adjListsReverse[to].insert(from);
}

CArcGraph_dist::CArcGraph_dist(int vertexNum) {
	vertexCount = vertexNum;
}

int CArcGraph_dist::VerticesCount() const {
	return vertexCount;
}

int CArcGraph_dist::EdgesCount() const {
	return edgesList.size();
}

void CArcGraph_dist::AddEdge(int from, int to, int dist) {
	Edge temp(from, to, dist);
	edgesList.push_back(temp);
}

void CArcGraph_dist::AddEdge(Edge edge) {
	edgesList.push_back(edge);
}

void CArcGraph_dist::GetEdges(std::vector<Edge>& edges) const {
	for (unsigned int i = 0; i < edgesList.size(); i++) {
		edges[i] = edgesList[i];
	}
}

class DSU {
public:
	DSU(int count) : sets(count) {
		for (int i = 0; i < count; i++)
			sets[i] = i;
	}
	int Find(int value);
	void Unite(int first, int second);
private:
	std::vector<int> sets;
};

int DSU::Find(int value) {
	int current = sets[value];
	std::vector<int> way;
	while (current != sets[current]) {
		current = sets[current];
		way.push_back(current);
	}
	for (unsigned int i = 0; i < way.size(); i++)
		sets[way[i]] = current;
	return current;
}

void DSU::Unite(int first, int second) {
	first = Find(first);
	second = Find(second);
	if (rand() % 2 == 0)
		sets[first] = second;
	else sets[second] = first;
}

long long boruvka(const CArcGraph_dist& graph) {
	const int vertices = graph.VerticesCount();
	CSetGraph tree(vertices);
	int treeSize = 0;
	long long result = 0;
	Edge* compMinEdges = new Edge[vertices];
	DSU components(vertices);
	std::vector<Edge> edges(graph.EdgesCount());
	graph.GetEdges(edges);
	int INF = INT_MAX;
	while (treeSize < vertices - 1) {
		for (int i = 0; i < vertices; i++) {
			compMinEdges[i].dist = INF;
		}
		for (unsigned int i = 0; i < edges.size(); i++) {
			if (components.Find(edges[i].from) != components.Find(edges[i].to)) {
				if (edges[i] < compMinEdges[components.Find(edges[i].from)]) {
					compMinEdges[components.Find(edges[i].from)] = edges[i];
				}
				if (edges[i] < compMinEdges[components.Find(edges[i].to)])
					compMinEdges[components.Find(edges[i].to)] = edges[i];
			}
		}
		bool flag = false;
		for (int i = 0; i < vertices; i++) {
			if (compMinEdges[i].dist < INF && !tree.HasEdge(compMinEdges[i].from, compMinEdges[i].to)) {
				if (!tree.HasEdge(compMinEdges[i].from, compMinEdges[i].to)) {
					tree.AddEdge(compMinEdges[i].from, compMinEdges[i].to);
					tree.AddEdge(compMinEdges[i].to, compMinEdges[i].from);
					components.Unite(compMinEdges[i].from, compMinEdges[i].to);
					result += compMinEdges[i].dist;
					treeSize++;
					flag = true;
				}
			}
		}
		if (!flag)
			break;
	}
	delete[] compMinEdges;
	return result;
}

int main() {
	std::ifstream fin;
	fin.open("kruskal.in");
	std::ofstream fout;
	fout.open("kruskal.out");
	int n, m = 0;
	int from, to, dist = 0;
	fin >> n;
	fin >> m;
	CArcGraph_dist graph(n);
	for (int i = 0; i < m; i++) {
		fin >> from >> to >> dist;
		graph.AddEdge(from - 1, to - 1, dist);
		graph.AddEdge(to - 1, from - 1, dist);
	}
	fout << boruvka(graph);
	return 0;
}
