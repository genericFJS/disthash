#include "MPIHash.h"
#include "TestHashMap.h"
#include "TestMPIComm.h"
#include "mpi.h"
#include <locale>
#include <clocale>
#include <iostream>

int numTasks, rank;
// Content of actionTag dictates next packages
int actionTag = 0;
int keyTag = 1, valueSizeTag = 2, valueTag = 3;
enum Actions { delEntry, insEntry, getEntry };
MPI_Status statMPI;

void printOnce(string text) {
	if (rank == 0) {
		std::cout << text << std::endl;
	}
}

void TestTests() {

	//printOnce("Test HashMap:")
	//printOnce("=======================================")
	//if (rank == 0) {
	//	TestHashMap* testHashMap = new TestHashMap();
	//}
	//printOnce("---------------------------------------")

	printOnce("Test MPI Communication:");
	printOnce("=======================================");
	TestMPIComm* testMPIComm = new TestMPIComm();
	printOnce("---------------------------------------");
}

int main(int argc, char *argv[]) {
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	bool test = true;

	if (test) {
		TestTests();
	} else {
		MPIHash* mpiHash = new MPIHash(MPIHash::HASHMAP_SIZE);
		//std::cout << "Hello World!" << std::endl;

		mpiHash->InsertDistEntry(5, "Alex");
		mpiHash->InsertDistEntry(55, "Thomas");
		mpiHash->InsertDistEntry(56, "Brunhilde");
	}
	return 0;
}