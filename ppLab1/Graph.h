#pragma once

#include <vector>
#include <mpi.h>
	

using std::vector;
using std::string;

class Graph
{
public:
	using GraphWeights = vector<unsigned>;
	using GraphTableRow = vector<bool>;
	using GraphTable = vector<GraphTableRow>;

	Graph();
	~Graph();

	bool loadFromFile(string filepath) noexcept;
	void loadFromRaw(const unsigned graphsize, const unsigned* weightData, const char *tableData);
	void generateGraph(const size_t size) noexcept;
	bool saveGraphToFile(const string &filepath) const noexcept;

	const bool getEdge(size_t fVertex, size_t sVertex) const noexcept;
	const unsigned getWeight(size_t VertexNumber) const noexcept;
	

	const unsigned expectedResult() const noexcept;
	const unsigned calculate() const noexcept;
	const unsigned calculateVertex(const unsigned &vert) const noexcept;
private:
	const unsigned predict(vector<size_t> candidates,vector<size_t> added) const noexcept;

	unsigned m_expectedResult;
	GraphWeights m_weights;
	GraphTable m_table;
};

