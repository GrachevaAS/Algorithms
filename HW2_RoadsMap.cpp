
/*Требуется отыскать самый выгодный маршрут между городами. Из города может выходить дорога, которая возвращается в этот же город.
Требуемое время работы O((N + M)log N), где N – количество городов, M – известных дорог между ними.
Формат ввода

Первая строка содержит число N – количество городов.
Вторая строка содержит число M - количество дорог.
Каждая следующая строка содержит описание дороги (откуда, куда, время в пути).
Последняя строка содержит маршрут (откуда и куда нужно доехать).
Формат вывода

Вывести длину самого выгодного маршрута.
*/

#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <fstream>
#include <climits>

template <typename T>
class IGraph_dist {
public:
	IGraph_dist() {}
	virtual ~IGraph_dist() {}

	virtual void AddEdge(int from, int to, long long dist) = 0;
	virtual int VerticesCount() const = 0;
	virtual void GetNextVertices(int vertex, std::vector<T>& vertices) const = 0;
};

struct dist_to {
	dist_to(long long Distance, int Vertex) : dist(Distance), to(Vertex) {}
	dist_to() : dist(0), to(0) {}
	dist_to(const dist_to& another) : dist(another.dist), to(another.to) {}
	long long dist;
	int to;
	void operator = (const dist_to& another) {
		dist = another.dist;
		to = another.to;
	}
	bool operator > (const dist_to& another) const {
		return this->dist > another.dist;
	}
};

class CListGraph_dist : public IGraph_dist<dist_to> {
public:
	explicit CListGraph_dist(int vertexNum);
	~CListGraph_dist();
	void AddEdge(int from, int to, long long dist);
	int VerticesCount() const;
	void GetNextVertices(int vertex, std::vector<dist_to>& vertices) const;
private:
	int vertexCount;
	std::list<dist_to>* adjLists;
};

CListGraph_dist::CListGraph_dist(int vertexNum) {
	vertexCount = vertexNum;
	adjLists = new std::list<dist_to>[vertexCount];
}

CListGraph_dist::~CListGraph_dist() {
	delete[] adjLists;
}

int CListGraph_dist::VerticesCount() const {
	return vertexCount;
}

void CListGraph_dist::AddEdge(int from, int to, long long dist) {
	adjLists[from].emplace(adjLists[from].end(), dist, to);
}

void CListGraph_dist::GetNextVertices(int vertex, std::vector<dist_to>& vertices) const {
	for (std::list<dist_to>::const_iterator it = adjLists[vertex].begin(); it != adjLists[vertex].end(); it++) {
		vertices.push_back(*it);
	}
}

class Heap {
public:
	Heap(int size) {
		Size = size;
		HeapArray = new dist_to[Size];
		Realsize = 0;
	}
	~Heap() {
		delete[] HeapArray;
	}
	void SiftDown(int start);
	void SiftUp(int start);
	void Add(long long dist, int to);
	void Delete(int dist);
	void Top(dist_to& value);
	bool Empty();

private:
	int Size;
	int Realsize;
	dist_to* HeapArray;

};

void Heap::Add(long long dist, int to) {
	dist_to value(dist, to);
	HeapArray[Realsize] = value;
	SiftUp(Realsize);
	Realsize++;
}

void Heap::Delete(int dist) {
	for (int i = 0; i < Realsize; i++) {
		if (HeapArray[i].to == dist) {
			HeapArray[i] = HeapArray[Realsize - 1];
			Realsize--;
			SiftDown(i);
			return;
		}
	}
}

bool Heap::Empty() {
	return Realsize == 0;
}

void Heap::Top(dist_to& value) {
	value.to = HeapArray[0].to;
	value.dist = HeapArray[0].dist;
	HeapArray[0] = HeapArray[Realsize - 1];
	Realsize--;
	SiftDown(0);
}

void Heap::SiftDown(int start) {
	int i = start;
	int left = 0;
	int right = 0;
	int max = 0;
	while (2 * i + 1 < Realsize) {
		left = 2 * i + 1;
		right = 2 * i + 2;
		max = i;
		if (left < Realsize && HeapArray[left] > HeapArray[max]) {
			max = left;
		}
		if (right < Realsize && HeapArray[right] > HeapArray[max]) {
			max = right;
		}
		if (max != i) {
			std::swap(HeapArray[i], HeapArray[max]);
			i = max;
		}
		else {
			return;
		}
	}
}

void Heap::SiftUp(int start) {
	int i = start;
	while (i > 0) {
		if (!(HeapArray[i] > HeapArray[(i - 1) / 2])) {
			return;
		}
		std::swap(HeapArray[i], HeapArray[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

long long shortestWaySearch(const CListGraph_dist& graph, int from, int to) {
	const int vertices = graph.VerticesCount();
	const long long infinity = LLONG_MAX;
	long long* dist = new long long[vertices];
	bool* in_queue = new bool[vertices];
	for (int i = 0; i < vertices; i++) {
		dist[i] = infinity;
		in_queue[i] = false;
	}
	dist[from] = 0;
	Heap queue(vertices);
	queue.Add(0, from);
	in_queue[0] = true;
	while (!queue.Empty()) {
		dist_to current;
		queue.Top(current);
		long long current_dist = current.dist;
		int current_to = current.to;
		if (dist[current_to] < current_dist)
			current_dist = dist[current_to];
		in_queue[current_to] = false;
		std::vector<dist_to> nextVertices;
		graph.GetNextVertices(current_to, nextVertices);
		for (unsigned int i = 0; i < nextVertices.size(); i++) {
			long long next_dist = nextVertices[i].dist;
			int next_to = nextVertices[i].to;
			if (dist[current_to] + next_dist < dist[next_to]) {
				dist[next_to] = dist[current_to] + next_dist;
				if (in_queue[next_to] == false) {
					queue.Add(dist[next_to], next_to);
					in_queue[next_to] = true;
				}
				else {
					queue.Delete(next_to);
					queue.Add(dist[next_to], next_to);
				}
			}
		}
	}
	long long result = dist[to];
	delete[] dist;
	delete[] in_queue;
	return result;
}

int main() {
	std::ifstream fin;
	fin.open("input.txt");
	int towns = 0;
	int roads = 0;
	int from, to, time = 0;
	fin >> towns;
	fin >> roads;
	CListGraph_dist roads_map(towns);
	for (int i = 0; i < roads; i++) {
		fin >> from >> to >> time;
		if (from != to) {
			roads_map.AddEdge(from, to, time);
			roads_map.AddEdge(to, from, time);
		}
	}
	fin >> from >> to;
	std::cout << shortestWaySearch(roads_map, from, to);
	return 0;
}