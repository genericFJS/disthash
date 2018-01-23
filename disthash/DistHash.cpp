#include "MPIHash.h"
#include "TestHashMap.h"
#include "TestMPIComm.h"
#include "TestMPIThreads.h"
#include "mpi.h"
#include <locale>
#include <clocale>
#include <iostream>
#include <pthread.h>

int providedThreads, numTasks, rank;
pthread_t thread;
// Content of actionTag dictates next packages
int actionTag = 0;
int keyTag = 1, valueSizeTag = 2, valueTag = 3;
enum Actions { delEntry, insEntry, getEntry };
MPI_Status status;
MPI_Comm thread_comm;

void printOnce(string text) {
	if (rank == 0) {
		std::cout << text << std::endl;
	}
}

// thread:
void* receive_thread(void* ptr) {
	//printf("Hello from a thread from %d.\n", rank);
	int buf;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	extern MPI_Status status;

	while (1) {
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, thread_comm, &status);
		if (status.MPI_TAG == 0)
			break;
		//printf("Received %d from %d\n", buf, status.MPI_SOURCE);
	}
	printf("Good bye from a thread from %d.\n", rank);
	pthread_exit((void *)NULL);
}

void TestTests(int argc, char *argv[]) {

	//printOnce("Test HashMap:")
	//printOnce("=======================================")
	//if (rank == 0) {
	//	TestHashMap* testHashMap = new TestHashMap();
	//}
	//printOnce("---------------------------------------")

	//printOnce("Test MPI Communication:");
	//printOnce("=======================================");
	//MPI_Init(&argc, &argv);
	//MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//TestMPIComm* testMPIComm = new TestMPIComm();
	//printOnce("---------------------------------------");

	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printOnce("Test MPI Threads:");
	//printOnce("=======================================");
	//printf("Number of threads: %d (rank %d)\n", providedThreads, rank);
	if (providedThreads < MPI_THREAD_FUNNELED)
		MPI_Abort(MPI_COMM_WORLD, 0);

	// Create a communicator just for communication with the thread:
	MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);
	if (rank == 0) {
		pthread_create(&thread, NULL, receive_thread, &thread_comm);
	}
	TestMPIThreads* testMPIComm = new TestMPIThreads();
	//printOnce("---------------------------------------");
}

void KillThread() {
	printf("Abort thread in %d.\n", rank);
	MPI_Send(MPI_BOTTOM, 0, MPI_INT, rank, 0, thread_comm);
	printf("Thread in %d aborted.\n", rank);
}

int main(int argc, char *argv[]) {
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());

	bool test = true;

	if (test) {
		TestTests(argc, argv);
		printf("\tIt's time for %d.\n", rank);
	} else {
		MPIHash* mpiHash = new MPIHash(MPIHash::HASHMAP_SIZE);
		//std::cout << "Hello World!" << std::endl;

		mpiHash->InsertDistEntry(5, "Alex");
		mpiHash->InsertDistEntry(55, "Thomas");
		mpiHash->InsertDistEntry(56, "Brunhilde");
	}
	if (rank == 0) {
		for (std::string line; std::getline(std::cin, line);) {
			if (line == "exit") {
				int msg = 0;
				for (int i = rank; i < numTasks-1; i++) {
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
		printf("Hallo");
	}
	if (rank == 0) {
		KillThread();
	}
	printf("Shutting down %d.\n", rank);
	MPI_Finalize();
	printf("That's it with %d.\n", rank);
	return 0;
}