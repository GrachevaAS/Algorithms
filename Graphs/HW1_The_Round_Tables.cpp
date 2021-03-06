/*
Единственный способ попасть в Зал Круглых Столов – пройти через Колонный Коридор. 
Стены Коридора изображаются на карте прямыми линиями, которые параллельны оси OY системы координат. 
Вход в Коридор находится снизу, а выход из Коридора в Зал – сверху. 
В Коридоре есть цилиндрические (на карте круглые) Колонны одинакового радиуса R. 
Разработайте алгоритм, который по информации о размерах Коридора, и размещения Колонн определяет диаметр наибольшего из Круглых Столов, который можно пронести через такой Коридор, сохраняя поверхность Стола горизонтальной.

Формат ввода
В первой строке задано два целых числа XL и XR - x-координаты левой и правой стен Коридора. 
Во второй строке находится целое число R (1≤R≤1000000)(1≤R≤1000000) - радиус всех Колон. 
В третьей - целое число N (1≤N≤200)(1≤N≤200), задающее количество Колон. 
Далее идут N строк, в каждой из которых по два целых числа – x- и y-координаты центра соответствующей Колоны. 
Все входные координаты – целые числа, не превышающие по модулю 1000000.

Формат вывода
Вывести одно число - искомый диаметр наибольшего Стола. 
Диаметр нужно выводить с точностью 3 знака после десятичной точки (даже в случае, если он окажется целым). 
Если нельзя пронести ни одного Стола, то ответ должен быть: 0.000 
Точность 3 знака после точки.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stack>
#include <cmath>

class IGraph {
public:
    IGraph() {}
    virtual ~IGraph() {}
	virtual void AddEdge(int from, int to, double distance) = 0;
	virtual int VerticesCount() const = 0;
};

struct Point {
    Point(int xInput, int yInput): x(0), y(0) {
        x = xInput;
        y = yInput;
    }
    int x;
    int y;
};

class CMatrixGraph : public IGraph {
public:
    explicit CMatrixGraph(int vertexNum);
    ~CMatrixGraph();
    void AddEdge(int from, int to, double distance);
	int VerticesCount() const;
	double getDist(int from, int to) const;
private:
    double** Matrix;
	int vertexCount;
};

void initGraph(CMatrixGraph* graph, const std::vector<Point>& inputArray, int leftWall, int rightWall, int R);
bool hasWay_maxDist(const IGraph& graph, double size);
double maxTableSearch(CMatrixGraph* graph, double minTable, double maxTable);

CMatrixGraph::CMatrixGraph(int vertexNum) {
    vertexCount = vertexNum;
    Matrix = new double*[vertexCount];
    Matrix[0] = new double[vertexCount * vertexCount];
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

void CMatrixGraph::AddEdge(int from, int to, double distance) {
    Matrix[from][to] = distance;
}

int CMatrixGraph::VerticesCount() const {
	return vertexCount;
}

double CMatrixGraph::getDist(int from, int to) const {
	return Matrix[from][to];
}

void initGraph(CMatrixGraph* graph, const std::vector<Point>& inputArray, int leftWall, int rightWall, int R) {
    double maxTable = rightWall - leftWall;
    double dist = 0;
    int columns = inputArray.size();
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < columns; j++) {
            if (i != j) {
                dist = sqrt(pow(inputArray[i].x - inputArray[j].x, 2) + pow(inputArray[i].y - inputArray[j].y, 2));
                if (dist > 2 * R) {
                    graph->AddEdge(i, j, dist - 2 * R);
                    graph->AddEdge(j, i, dist - 2 * R);
                }
            }
        }
    }

    for (int i = 0; i < columns; i++) {
        if (inputArray[i].x - leftWall > R) {
            graph->AddEdge(i, columns, inputArray[i].x - leftWall - R);
            graph->AddEdge(columns, i, inputArray[i].x - leftWall - R);
        }
        if (rightWall - inputArray[i].x > R) {
            graph->AddEdge(i, columns + 1, rightWall - inputArray[i].x - R);
            graph->AddEdge(columns + 1, i, rightWall - inputArray[i].x - R);
        }
    }
    graph->AddEdge(columns, columns + 1, maxTable);
    graph->AddEdge(columns + 1, columns, maxTable);
}

bool hasWay_maxDist(const CMatrixGraph* graph, double size) {
	std::stack<int> dfsStack;
	int vertexCount = graph->VerticesCount();
	int leftWall = vertexCount - 2;
	int rightWall = vertexCount - 1;
	bool* visited = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
		visited[i] = false;
	dfsStack.push(leftWall);
	while (!dfsStack.empty()) {
		int current = dfsStack.top();
		visited[current] = true;
		dfsStack.pop();
		for (int i = 0; i < vertexCount; i++) {
			if (graph->getDist(current, i) < size && !visited[i]) {
				if (i == rightWall)
					return true;
				dfsStack.push(i);
			}
		}
	}
	delete[] visited;
	return false;
}

double maxTableSearch(CMatrixGraph* graph, double minTable, double maxTable) {
    double left = minTable;
    double right = maxTable;
    double mid = 0;
    while (right - left > 0.0000001) {
        mid = (right + left) / 2;
        if (hasWay_maxDist(graph, mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}

int main() {
    std::ifstream fin;
    fin.open("input.txt");

    int leftWall, rightWall = 0;
    int R = 0;
    int columns = 0;
    int x, y = 0;
    fin >> leftWall >> rightWall;
    fin >> R;
    fin >> columns;
    std::vector<Point> inputArray;
    for (int i = 0; i < columns; i++) {
        fin >> x >> y;
        if (leftWall <= x && x <= rightWall) {
            Point temp(x, y);
            inputArray.push_back(temp);
        }
    }
    columns = inputArray.size();
    CMatrixGraph* graph = new CMatrixGraph(columns + 2);
    initGraph(graph, inputArray, leftWall, rightWall, R);
    int maxTable = rightWall - leftWall;
    std::cout << std::fixed << std::setprecision(3) << round(maxTableSearch(graph, 0, maxTable) * 1000) / 1000;
    delete graph;
}
