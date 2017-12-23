#include "Graph.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <stdlib.h>
#include <ctime>

Graph::Graph()
{
}

Graph::~Graph()
{
}

bool Graph::loadFromFile(string filepath) noexcept
{
	std::ifstream in{ filepath };

	if (!in) 
		return false;

	size_t size;
	in >> size >> m_expectedResult;
	
	m_weights.resize(size);
	for (unsigned i = 0; i < size; i++)
		in >> m_weights[i];

	m_table.resize(size);
	for (auto &row : m_table) {
		row.resize(size);
		bool tmp;
		for (size_t i = 0; i < size; i++) {
			in >> tmp; row[i] = tmp;
		}
	}
	return true;
}

void Graph::loadFromRaw(const unsigned graphsize, const unsigned * weightData, const char * tableData)
{
	m_weights.resize(graphsize);
	m_table.resize(graphsize);
	for (unsigned i = 0; i < graphsize; i++)
	{
		m_table[i].resize(graphsize);
		m_weights[i] = weightData[i];
		for (unsigned j = 0; j < graphsize; j++)
		{
			m_table[i][j] = tableData[i*graphsize+j];
		}
	}
}

void Graph::generateGraph(const size_t size) noexcept
{
	m_weights.resize(size);
	for (unsigned& weight : m_weights)
		weight = rand() % 9 + 1;

	srand(static_cast<unsigned> (time(NULL)) );
	m_table.resize(size);
	for (size_t i = 0; i < size; i++)
	{
		m_table[i].resize(size);
		for (size_t j = 0; j < size; j++)
		{
			if (i < j)
				m_table[i][j] = ((rand() % 3) != 0);
			else
				m_table[i][j] = m_table[j][i];
		}

	}

}

bool Graph::saveGraphToFile(const string & filepath) const noexcept
{
	using namespace std;
	ofstream out(filepath);
	
	if (!out)
		return false;

	out << m_weights.size() << " " << 0 << endl;
	for (size_t val : m_weights)
		out << val << " ";
	out << endl;

	for (auto & row : m_table)
	{
		for (size_t val : row)
			out << val << " ";
		out << endl;
	}
	return true;
}

const bool Graph::getEdge(size_t fVertex, size_t sVertex) const noexcept
{
	return m_table[fVertex][sVertex];
}

const unsigned Graph::getWeight(size_t VertexNumber) const noexcept
{
	return m_weights[VertexNumber];
}

const unsigned Graph::expectedResult() const noexcept
{
	return m_expectedResult;
}

const unsigned Graph::calculate() const noexcept
{
	using namespace std;
	unsigned vmax = 0;
	// проходим все вершины
	for (size_t vert = 0; vert < m_weights.size(); vert++) {

		unsigned total = calculateVertex(vert);
		if (total > vmax) vmax = total;
		
	}

	return vmax;


}

const unsigned Graph::calculateVertex(const unsigned & vert) const noexcept
{
	vector<size_t> candidates;
	vector<size_t> added;
	// находим все вершины, не связанные с этой
	for (size_t cvert = 0; cvert < m_weights.size(); cvert++)
	{
		if (vert == cvert)
			continue;

		if (!getEdge(vert, cvert))
			candidates.push_back(cvert);
	}

	// среди них выбираем те, которые не связаные
	// ни с одной из других
	for (size_t cand : candidates)
	{
		bool isAcceptable = true;
		for (size_t ccand : candidates)
		{
			if (cand == ccand)
				continue;
			if (getEdge(cand, ccand)) {
				isAcceptable = false;
				break;
			}
		}
		if (isAcceptable)
			added.push_back(cand);
	}

	// Удаляем из кандидатов все вершины,
	// которые уже были добавлены
	for (size_t add : added)
		candidates.erase(remove(candidates.begin(), candidates.end(), add), candidates.end());



	// поочерёдно добавляем в подграф одну из конфликтующих вершин
	// и вычисляем максимальный вес при добавлении этой вершины
	unsigned max = 0;
	for (size_t cand : candidates)
	{
		added.push_back(cand);
		const unsigned val = predict(candidates, added);
		if (val > max) max = val;
		added.pop_back();

	}
	unsigned total = getWeight(vert) + max;
	return total;
}

const unsigned Graph::predict(vector<size_t> candidates, vector<size_t> added) const noexcept
{

	using namespace std;

	// удаление из кандидатов тех вершин, которые конфликтуют с последней добавленной
	candidates.erase(
		remove_if(candidates.begin(), candidates.end(),
			[&](const size_t val) {
				return (getEdge(added.back(),val) || (added.back() == val));
			} ),
		candidates.end());


	// поиск идеальных кандидатов
	// (которые не конфликтуют ни с одним другим кандидатом)
	for (size_t cand = 0; cand < candidates.size(); cand++)
	{
		bool isGood = true;
		for (size_t ccand : candidates)
		{
			if (getEdge(ccand, candidates[cand]))
			{
				isGood = false;
				break;
			}
		}
		if (isGood) {
			added.push_back(candidates[cand]);
			candidates.erase(candidates.begin() + cand);
			cand--;
		}
	}



	// если остались конфликтующие кандидаты, 
	// рекурсивно находим максимально жирный независимый подграф
	unsigned max = 0;
	for (size_t val : candidates)
	{
		added.push_back(val);

		unsigned res = predict(candidates, added);
		if (res > max) max = res;

		added.pop_back();
	}

	// если кандидатов не осталось, расчёты завершены,
	// высчитывается сумма всех вершин
	if (candidates.empty())
	{
		unsigned sum = 0;
		for (size_t vert : added)
			sum += getWeight(vert);
		return sum;
	}

	return max;


}
