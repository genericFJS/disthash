#include "MPIHashThread.h"

extern int rank;
extern MPI_Comm thread_comm;
extern MPI_Status status;
extern MPIHash* mpiHash;

void* MPIHashThread(void* ptr) {
	int actionCode;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	printf("Starting thread for Process %d.\n", rank);

	// Schleife zum Abarbeiten von HashMap-Aktivtäten
	while (true) {
		// Warte, bis eine Aktion gesendet wird.
		MPI_Recv(&actionCode, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		int action = status.MPI_TAG;
		int source = status.MPI_SOURCE;
		printf("Got action %d from Process %d.\n", actionCode, source);
		// Action-Tag gibt an, was geschehen soll.
		//========================================
		if (action == TAG_ACTION) {
			switch (actionCode) {
			case ACTION_DEL:
			{ // Element löschen.
				int key;
				bool deleted;
				int feedback;
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// Eintrag (versuchen zu) löschen.
				deleted = mpiHash->DeleteEntry(key);
				// Feedback formulieren (1: Erfolg, 0: Misserfolg).
				deleted ? feedback = 1 : feedback = 0;
				// Feedback senden.
				MPI_Send(&feedback, 1, MPI_INT, source, TAG_FEEDBACK, MPI_COMM_WORLD);
				if (deleted)
					printf("Deleted (%d, -) for Process %d from HashMap by Process %d.\n", key, source, rank);
				else
					printf("Could not delete (%d, -) for Process %d from HashMap by Process %d.\n", key, source, rank);
				break;
			}
			case ACTION_INS:
			{ // Element einfügen.
				int key;
				int value;
				printf("Checking key for Process %d.\n", source);
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("Received key %d from Process %d.\n", key, source);
				// Value empfangen.
				printf("Getting Value.\n");
				MPI_Recv(&value, 1, MPI_INT, source, TAG_VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("Got Value.\n");
				// Eintrag einfügen.
				printf("Trying to insert.\n");
				mpiHash->InsertEntry(key, value);
				printf("Inserted (%d, %d) from Process %d into HashMap by Process %d.\n", key, value, source, rank);
				break;
			}
			case ACTION_GET:
			{ // Element holen.
				int key;
				int value;
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// Value holen.
				value = mpiHash->GetEntry(key);
				// Sende Value.
				MPI_Send(&value, 1, MPI_INT, source, TAG_VALUE, MPI_COMM_WORLD);
				printf("Got (%d, %d) for Process %d from HashMap by Process %d.\n", key, value, source, rank);
				break;
			}
			default:
				// Bei nicht validen Tags zum entsprechenden Label springen.
				goto WrongTag;
				break;
			}

		} else if (action == TAG_EXIT) {
			// Aus Loop ausbrechen und Thread beenden (sollte nur zum Beenden ausgeführt werden).
			break;
		} else {
		WrongTag:
			printf("Warning: Tag not applicable in this context (%d).\n", action);
		}
	}
	pthread_exit((void *)NULL);
}

/// <summary>
/// Sendet MPI-Nachricht, um eigenen Thread zu stoppen.
/// </summary>
void KillThread() {
	MPI_Send(MPI_BOTTOM, 0, MPI_INT, rank, TAG_EXIT, thread_comm);
}