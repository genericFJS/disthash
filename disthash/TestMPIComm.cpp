#include "TestMPIComm.h"

TestMPIComm::TestMPIComm() {
	string testString = "Brunhilde";

	extern int numTasks, rank;
	extern MPI_Status statMPI;
	int dest, source, rc, count, tag = 1;
	char inmsg, outmsg = 'x';
	
	if (rank == 0) {
		dest = 1;
		source = 1;
		//MPI_Send(&result, result.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		//MPI_Send(result.c_str(), result.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		rc = MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &statMPI);
	} else if (rank == 1) {
		dest = 0;
		source = 0;
		std::cout << "Hallo" << std::endl;
		usleep(1000000);
		rc = MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &statMPI);
		rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}

	rc = MPI_Get_count(&statMPI, MPI_CHAR, &count);
	printf("Task %d: Received %d char(s) from task %d with tag %d: %c/%c\n",
		rank, count, statMPI.MPI_SOURCE, statMPI.MPI_TAG, inmsg, outmsg);

	const int lentag = 0;
	const int datatag = 1;
	if (rank == 0) {
		string send = "Brunhilde";

		// Send length, then data
		int len = send.length();
		MPI_Send(&len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD);

		std::cout << "Rank " << rank << " sends: " << send << std::endl;
		MPI_Send(send.c_str(), len, MPI_CHAR, 1, datatag, MPI_COMM_WORLD);

	} else if (rank == 1) {

		int len;
		MPI_Recv(&len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		char recv_data[len];
		MPI_Recv(recv_data, len, MPI_CHAR, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		string recv = recv_data;

		std::cout << "Rank " << rank << " got sent: " << recv << std::endl;
	}

	MPI_Finalize();
}


TestMPIComm::~TestMPIComm() {
}