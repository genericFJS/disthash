#include "LinkedHashEntry.h"
#include "HashMap.h"
#include "MPIHash.h"
#include "MPIHashThread.h"
#include "TestHashMap.h"
#include "TestMPIComm.h"
#include "TestMPIThreads.h"
#include "MyLibs.h"

int providedThreads = 0, numProcesses = 1, rank = 0;
pthread_t thread;
MPI_Comm thread_comm;
MPI_Status status;
MPIHash* mpiHash;

void printOnce(string text) {
	if (rank == 0) {
		printf("%s\n", text.c_str());
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

	///* Communication
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printOnce("Test MPI Communication:");
	//printOnce("=======================================");
	TestMPIComm* testMPIComm = new TestMPIComm();
	//printOnce("---------------------------------------");
	//*/

	/* Threads
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
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
				for (int i = rank; i < numProcesses - 1; i++) {
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
	*/
}

int main(int argc, char *argv[]) {
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());

	//TestTests(argc, argv);
	//return 0;

	// Die (MPI-)HashMap erstellen.
	mpiHash = new MPIHash(HASHMAP_SIZE);

	// MPI mit Thread initialisieren.
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (providedThreads < MPI_THREAD_FUNNELED)
		MPI_Abort(MPI_COMM_WORLD, 0);

	// Für jeden Prozess Thread mit gesondertem Communicator erstellen.
	MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);
	pthread_create(&thread, NULL, MPIHashThread, &thread_comm);

	// ================================================================
	//                      Eigentlicher Prozess
	// ================================================================

	// MPI_Barrier, um sicherzustellen, dass alle Initialsierungen fertig sind?! Geht nicht.
	//sleep(1);
	//MPI_Barrier(MPI_COMM_WORLD);

	//if (rank == 0)
	//	mpiHash->InsertDistEntry(5, "Brunhilde");
	//mpiHash->GetDistEntry(5);
	//mpiHash->DeleteDistEntry(6);
	mpiHash->DeleteDistEntry(5);
	//mpiHash->GetDistEntry(5);

	//sleep(2);
	//printf("Hallo?\n");
	// ================================================================
	// Nur Prozess 0 hat Nutzereingabe für weiter Bedienung.
	if (rank == 0) {
		for (std::string line; std::getline(std::cin, line);) {
			if (line == "exit") {
				int msg = 0;
				for (int i = rank; i < numProcesses - 1; i++) {
					MPI_Send(MPI_BOTTOM, 0, MPI_INT, i + 1, TAG_EXIT, MPI_COMM_WORLD);
				}
				break;
			}
			printf("'%s' is no command.\n", line.c_str());
		}
	} else {
		int buf;
		MPI_Recv(&buf, 1, MPI_INT, 0, TAG_EXIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	// Beim beenden, Thread auch beenden.
	KillThread();

	MPI_Finalize();
	return 0;
}