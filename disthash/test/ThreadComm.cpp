#include "mpi.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <locale>
#include <clocale>
#include <stddef.h>
#include <string>
#include <cstring>
using std::string;
using std::strcpy;

#define TAG_EXIT 0
#define TAG_CONTENT 1

int providedThreads, numProcesses, rank;

void* testThread(void* ptr) {
	int buf;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	MPI_Status status;
	while (1) {
		//MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("\t\tWaiting for message on thread in %d.\n", rank);
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, thread_comm, &status);
		if (status.MPI_TAG == TAG_EXIT)
			break;
		printf("\t\t%d: Received %d from %d.\n", rank, buf, status.MPI_SOURCE);
	}
	printf("\t\tLeaving thread %d.\n", rank);
	//MPI_Barrier(thread_comm);
	pthread_exit((void *)NULL);
}

int main(int argc, char *argv[]) {
	int data = 42, recipient;
	pthread_t thread;
	MPI_Comm thread_comm, main_comm;
	MPI_Status status;

	// MPI mit Thread initialisieren.
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (providedThreads < MPI_THREAD_FUNNELED)
		MPI_Abort(MPI_COMM_WORLD, 0);

	// Communicator für Hauptanwendung
	MPI_Comm_dup(MPI_COMM_WORLD, &main_comm);
	// Für jeden Prozess Thread mit gesondertem Communicator erstellen.
	MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);
	pthread_create(&thread, NULL, testThread, &thread_comm);

	// ================================================================
	//                      Eigentlicher Prozess
	// ================================================================

	//if (rank == 0) {
		recipient = (rank + 1)% numProcesses;
		printf("Sending something from %d to %d.\n", rank, recipient);
		//MPI_Ssend(&data, 1, MPI_INT, recipient, TAG_CONTENT, MPI_COMM_WORLD);
		MPI_Ssend(&data, 1, MPI_INT, recipient, TAG_CONTENT, thread_comm);
	//}

	// ================================================================

	sleep(5);
	//usleep(500);
	printf("Process %d waiting to exit.\n", rank);
	//MPI_Barrier(main_comm);
	//printf("Process %d telling thread to leave.\n", rank);
	MPI_Ssend(MPI_BOTTOM, 0, MPI_INT, rank, TAG_EXIT, thread_comm);
	//printf("Process %d waiting to for all threads to be closed.\n", rank);
	//MPI_Barrier(main_comm);

	MPI_Finalize();
	return 0;
}