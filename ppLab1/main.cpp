
#include "Graph.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <mpi.h>


int main(int argc, char* argv[])
{
	using namespace std;
	unsigned errcode;

	istringstream ss(argv[1]);
	unsigned graphsize; 
	if ( argc == 0 || !(ss >> graphsize) ) {
		cout << " graph size not specified,exit";
		return 0;
	}
	unsigned summary_time = clock();

	if ((errcode = MPI_Init(&argc, &argv)) != 0)
	{
		std::cout << std::endl << "Error while initializing mpi" << std::endl;
		std::cin.get();
		return errcode;
	}

	int myRank;
	int worldSize;
	
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);




	char *edgeTable = new char[graphsize*graphsize];
	unsigned *weightTable = new unsigned[graphsize];

	Graph graph;
	if (myRank == 0) {

		cout << "nodes count : " << worldSize;
		graph.generateGraph(graphsize);
		for (unsigned i = 0; i < graphsize; i++)
		{
			weightTable[i] = graph.getWeight(i);
			for (unsigned j = 0; j < graphsize; j++)
			{
				edgeTable[i*graphsize+j] = graph.getEdge(i, j);
			}
		}
	}

	MPI_Bcast(edgeTable, graphsize*graphsize, MPI_CHAR,0,MPI_COMM_WORLD);
	MPI_Bcast(weightTable, graphsize , MPI_UNSIGNED, 0, MPI_COMM_WORLD);



	if (myRank != 0)
	{
		graph.loadFromRaw(graphsize, weightTable, edgeTable);
	}


	
	const unsigned vertPerNode = graphsize / worldSize;

	unsigned localmax = 0;
	for (unsigned i = vertPerNode * myRank; i < vertPerNode * myRank + vertPerNode; i++)
	{
		auto value = graph.calculateVertex(i);
		if (value > localmax)
			localmax = value;
	}


	unsigned global_max;
	MPI_Reduce(&localmax, &global_max, 1, MPI_UNSIGNED, MPI_MAX,0,MPI_COMM_WORLD);
	
	if (myRank == 0) {
		cout << endl << "global max is : " << global_max;
		std::cout << std::endl << "Total calculation time : " << clock() - summary_time;
		//std::cout << std::endl << "press something to exit" << std::endl;
 	}


	delete[] edgeTable;
	delete[] weightTable;
	MPI_Finalize();
}