#include "LinkedHashEntry.h"
#include "HashMap.h"
#include "MPIHash.h"
#include "MPIHashThread.h"
#include "TestHashMap.h"
#include "TestMPIComm.h"
#include "TestMPIThreads.h"
#include "MyLibs.h"

int providedThreads, numTasks, rank;
pthread_t thread;
MPI_Status status;
MPI_Comm thread_comm;
// Inhalt des Action-Tags bestimmt weitere (erwartete) Handlung.
#define TAG_EXIT		0
#define TAG_ACTION		1
#define TAG_KEY			2
#define TAG_VALUE_SIZE	3	// Alternative zu value-size über send-recieve Austasch wäre probe
#define TAG_VALUE		4
enum Actions { delEntry, insEntry, getEntry };

void printOnce(string text) {
	if (rank == 0) {
		std::cout << text << std::endl;
	}
}

void TestTests(int argc, char *argv[]) {
	/* HashMap
	printOnce("Test HashMap:");
	printOnce("=======================================");
	if (rank == 0) {
		TestHashMap* testHashMap = new TestHashMap();
	}
	printOnce("---------------------------------------");
	*/

	/* Communication
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printOnce("Test MPI Communication:");
	printOnce("=======================================");
	TestMPIComm* testMPIComm = new TestMPIComm();
	printOnce("---------------------------------------");
	*/

	///* Threads
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printOnce("Test MPI Threads:");
	printOnce("=======================================");
	printf("Number of threads: %d (rank %d)\n", providedThreads, rank);
	if (providedThreads < MPI_THREAD_FUNNELED)
		MPI_Abort(MPI_COMM_WORLD, 0);

	// Create a communicator just for communication with the thread:
	MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);
	if (rank == 0) {
		pthread_create(&thread, NULL, MPIHashThread, &thread_comm);
	}
	TestMPIThreads* testMPIComm = new TestMPIThreads();
	printOnce("---------------------------------------");

	if (rank == 0) {
		for (std::string line; std::getline(std::cin, line);) {
			if (line == "exit") {
				int msg = 0;
				for (int i = rank; i < numTasks - 1; i++) {
					MPI_Send(MPI_BOTTOM, 0, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
				}
				//MPI_Bcast(&msg, 1, MPI_INT, 0, MPI_COMM_WORLD);
				break;
			}
			std::cout << line << std::endl;
		}
	} else {
		int buf;
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
	if (rank == 0) {
		KillThread();
	}
	//*/
}

int main(int argc, char *argv[]) {
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());

	bool test = true;

	if (test) {
		TestTests(argc, argv);
	} else {
		MPIHash* mpiHash = new MPIHash(MPIHash::HASHMAP_SIZE);
		//std::cout << "Hello World!" << std::endl;

		mpiHash->InsertDistEntry(5, "Alex");
		mpiHash->InsertDistEntry(55, "Thomas");
		mpiHash->InsertDistEntry(56, "Brunhilde");
	}
	MPI_Finalize();
}