#include "TestMPIThreads.h"

TestMPIThreads::TestMPIThreads() {

	extern pthread_t thread;
	extern int thread_comm, rank;

	int data = 42;
	// let process 0 print value of data:
	try { 
		MPI_Send(&data, 1, MPI_INT, 0, 1, thread_comm);
		printf("Sending the truth from %d.\n", rank);
	} catch (...) {
		printf("Aborting, receiver dead.");
	}
	// Exit receive thread:
	//MPI_Send(MPI_BOTTOM, 0, MPI_INT, 0, 0, thread_comm);
}

TestMPIThreads::~TestMPIThreads() {
}