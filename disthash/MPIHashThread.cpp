#include "MPIHashThread.h"

extern MPI_Comm thread_comm;
extern int rank;

void* MPIHashThread(void* ptr) {
	int buf;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	extern MPI_Status status;

	while (1) {
		MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, thread_comm, &status);
		if (status.MPI_TAG == 0)
			break;
		printf("Received %d from %d\n", buf, status.MPI_SOURCE);
	}
	pthread_exit((void *)NULL);
}

void KillThread() {
	MPI_Send(MPI_BOTTOM, 0, MPI_INT, rank, 0, thread_comm);
}