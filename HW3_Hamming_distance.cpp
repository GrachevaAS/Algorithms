/*
Расстояние Хэмминга между двумя строками равной длины — это количество индексов, в которых символы этих строк не равны.
Определим расстояние от шаблона p до строки s как суммарное расстояние Хэмминга от p до всех подстрок s, имеющих длину |p|. 
В строке и в шаблоне некоторые символы стёрты. Нужно восстановить стёртые символы так, чтобы расстояние от p до s было минимально возможным.
Формат ввода
В первой строке дана строка s, во второй — шаблон p. Обе строки непусты, имеют длину не более 1000 и состоят из символов `0', `1' и `?'.
`?' обозначает стёртые символы, которые нужно восстановить. Длина строки p не превосходит длины строки s.
Формат вывода
В первой строке выведите минимальное расстояние от p до s после восстановления всех стёртых символов. 
Во второй и третьей строках соответственно выведите s и p, в которых символы `?' заменены на `0' и `1'.
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <climits>
#include <stack>
#include <algorithm>

struct Edge {
	Edge(int _to, long long _flow, long long _capacity, unsigned int rev) :
		to(_to), flow(_flow), capacity(_capacity), reverse_pos(rev) {}

	Edge(const Edge& another) :
		to(another.to), flow(another.flow), capacity(another.capacity), reverse_pos(another.reverse_pos) {}

	int to;
	long long flow;
	long long capacity;
	unsigned int reverse_pos;
};

class FlowInGraph {
public:
	explicit FlowInGraph(int _vertices) : level(_vertices, -2), next(_vertices + 1, 0) {
		vertices = _vertices;
		flow_from = 0;
		flow_to = _vertices - 1;
		adjLists = new std::vector<Edge>[_vertices];
	}

	~FlowInGraph() {
		delete[] adjLists;
	}

	void addEdge(int _from, int _to, int _capacity) {
		Edge a(_to, 0, _capacity, adjLists[_to].size());
		Edge b(_from, 0, 0, adjLists[_from].size());
		adjLists[_from].push_back(a);
		adjLists[_to].push_back(b); // reverse
	}
	int VerticesCount() {
		return vertices;
	}
	long long DinicMaxflow();
	void markComponent(int from, std::vector<bool>& result);
	void printGraph();
private:
	int vertices;
	int flow_from;
	int flow_to;
	std::vector<int> level;
	std::vector<unsigned int> next;
	std::vector<Edge> *adjLists;
	bool bfs();
	long long sendFlow(int from, long long flow);
};

bool FlowInGraph::bfs() {
	for (int i = 0; i < vertices; i++)
		level[i] = -2;
	level[flow_from] = 0;
	std::queue<int> queue;
	queue.push(flow_from);
	while (!queue.empty()) {
		int current = queue.front();
		queue.pop();
		for (std::vector<Edge>::iterator it = adjLists[current].begin(); it != adjLists[current].end(); it++) {
			if (level[(*it).to] == -2 && (*it).flow < (*it).capacity) {
				level[(*it).to] = level[current] + 1;
				queue.push((*it).to);
			}
		}
	}
	return level[flow_to] < 0 ? false : true;
}

long long FlowInGraph::sendFlow(int from, long long flow) {
	if (from == flow_to)
		return flow;
	for (; next[from] < adjLists[from].size(); next[from]++) {
		Edge& edge = adjLists[from][next[from]];
		if (level[edge.to] == level[from] + 1 && edge.flow < edge.capacity) {
			long long curr_flow = std::min(flow, edge.capacity - edge.flow);
			long long temp_flow = sendFlow(edge.to, curr_flow);
			if (temp_flow > 0) {
				edge.flow += temp_flow;
				adjLists[edge.to][edge.reverse_pos].flow -= temp_flow;
				return temp_flow;
			}
		}
	}

	return 0;
}

long long FlowInGraph::DinicMaxflow() {
	if (flow_from == flow_to)
		return -1;
	long long total = 0;
	while (bfs()) {
		for (int i = 0; i < vertices; i++)
			next[i] = 0;
		long long flow = 0;
		do {
			flow = sendFlow(flow_from, LLONG_MAX);
			total += flow;
		} while (flow > 0);
	}
	return total;
}

void FlowInGraph::markComponent(int from, std::vector<bool>& result) {
	std::stack<int> dfsStack;
	bool* visited = new bool[vertices];
	for (int i = 0; i < vertices; i++)
		visited[i] = false;
	dfsStack.push(from);
	while (!dfsStack.empty()) {
		int current = dfsStack.top();
		visited[current] = true;
		result[current] = false;
		dfsStack.pop();
		for (std::vector<Edge>::iterator it = adjLists[current].begin(); it != adjLists[current].end(); it++)
			if (!visited[(*it).to] && (*it).capacity - (*it).flow > 0)
				dfsStack.push((*it).to);
	}
	delete[] visited;
}

class workWithStrings {
public:
	workWithStrings(const std::string& str, const std::string& templ) :
		v_str(str.length(), 2), v_templ(templ.length(), 2) {
		lenS = str.length();
		lenT = templ.length();
		for (int i = 0; i < lenS; i++) {
			if (str[i] != '?')
				v_str[i] = str[i] - '0';
			else q_str.push_back(i);
		}
		for (int i = 0; i < lenT; i++) {
			if (templ[i] != '?')
				v_templ[i] = templ[i] - '0';
			else q_templ.push_back(i);
		}
		countQS = q_str.size();
		countQT = q_templ.size();
		result = 0;
		createGraph();
	}

	int getResult() {
		return result;
	}
	void getData(std::vector<int>& first, std::vector<int>& second) {
		first = v_str;
		second = v_templ;
	}

private:
	void createGraph();
	void solve(FlowInGraph& graph);
	int lenS;
	int lenT;
	std::vector<int> v_str;
	std::vector<int> v_templ;
	std::vector<int> q_str;
	std::vector<int> q_templ;
	int countQS;
	int countQT;
	int result;
};

void workWithStrings::createGraph() {
	std::vector<std::pair<int, int> > count_str(countQS, std::make_pair(0, 0));
	std::vector<std::pair<int, int> > count_templ(countQT, std::make_pair(0, 0));
	for (int i = 0; i < countQS; i++)
		for (int j = std::max(0, lenT - lenS + q_str[i]);
			j < std::min(lenT, q_str[i] + 1); j++) {
		if (v_templ[j] == 1)
			count_str[i].second++;
		if (v_templ[j] == 0)
			count_str[i].first++;
	}
	for (int i = 0; i < countQT; i++)
		for (int j = q_templ[i]; j < lenS - lenT + q_templ[i] + 1; j++) {
			if (v_str[j] == 1)
				count_templ[i].second++;
			if (v_str[j] == 0)
				count_templ[i].first++;
		}
	int vertices = 2 + countQS + countQT;
	FlowInGraph graph(vertices);
	for (int i = 0; i < countQT; i++) {
		if (count_templ[i].first != 0) {
			graph.addEdge(0, i + 1, count_templ[i].first);
		}
		if (count_templ[i].second != 0) {
			graph.addEdge(i + 1, vertices - 1, count_templ[i].second);
		}
	}
	for (int i = 0; i < countQS; i++) {
		if (count_str[i].first != 0)
			graph.addEdge(0, i + countQT + 1, count_str[i].first);
		if (count_str[i].second != 0)
			graph.addEdge(i + countQT + 1, vertices - 1, count_str[i].second);
	}
	for (int i = 0; i < countQS; i++) {
		for (int j = 0; j < countQT; j++) {
			if (q_str[i] >= q_templ[j] && (lenS - q_str[i]) >= (lenT - q_templ[j])) {
				graph.addEdge(i + countQT + 1, j + 1, 1);
				graph.addEdge(j + 1, i + countQT + 1, 1);
			}
		}
	}
	solve(graph);
}

void workWithStrings::solve(FlowInGraph& graph) {
	long long flow = graph.DinicMaxflow();
	std::vector<bool> getResult(graph.VerticesCount(), true);
	graph.markComponent(0, getResult);
	for (int i = 1; i < countQT + 1; i++)
		v_templ[q_templ[i - 1]] = (getResult[i]) ? 1 : 0;
	for (int i = countQT + 1; i < graph.VerticesCount() - 1; i++)
		v_str[q_str[i - countQT - 1]] = (getResult[i]) ? 1 : 0;
	for (int i = 0; i < lenS - lenT + 1; i++)
		for (int j = 0; j < lenT; j++)
			if (v_str[i + j] != v_templ[j]) 
				result++;
}
int main() {
	std::ifstream fin;
	fin.open("input.txt");
	std::ofstream fout;
	fout.open("output.txt");
	std::string str;
	getline(fin, str);
	std::string templ;
	getline(fin, templ);
	workWithStrings solution(str, templ);
	std::cout << solution.getResult() << "\n";
	std::vector<int> v_str(str.length());
	std::vector<int> v_templ(templ.length());
	solution.getData(v_str, v_templ);
	for (unsigned int i = 0; i < v_str.size(); i++)
		std::cout << v_str[i];
	std::cout << "\n";
	for (unsigned int i = 0; i < v_templ.size(); i++)
		std::cout << v_templ[i];
	return 0;
}
