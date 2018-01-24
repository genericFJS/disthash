#include "MPIHashThread.h"

extern int rank;
extern MPI_Comm thread_comm;
extern MPI_Status status;
extern MPIHash* mpiHash;

void* MPIHashThread(void* ptr) {
	int actionCode;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	int len;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(name, &len);
	printf("\x1B[%dmStarting thread for Process %d on %s.\x1B[0m\n", 31 + rank, rank, name);

	// Schleife zum Abarbeiten von HashMap-Aktivtäten
	while (true) {
		printf("\x1B[%dmWaiting for something to happen in Process %d.\x1B[0m\n", 31 + rank, rank);
		// Warte, bis eine Aktion gesendet wird.
		MPI_Recv(&actionCode, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, thread_comm, &status);
		printf("\x1B[%dmGot action in Process %d.\x1B[0m\n", 31 + rank, rank);
		int action = status.MPI_TAG;
		int source = status.MPI_SOURCE;
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
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, thread_comm, MPI_STATUS_IGNORE);
				// Eintrag (versuchen zu) löschen.
				deleted = mpiHash->DeleteEntry(key);
				// Feedback formulieren (1: Erfolg, 0: Misserfolg).
				deleted ? feedback = 1 : feedback = 0;
				// Feedback senden.
				MPI_Ssend(&feedback, 1, MPI_INT, source, TAG_FEEDBACK, MPI_COMM_WORLD);
				if (deleted)
					printf("\x1B[%dmDeleted (%d, -) for Process %d from HashMap by Process %d.\x1B[0m\n", 31+rank, key, source, rank);
				else
					printf("\x1B[%dmCould not delete (%d, -) for Process %d from HashMap by Process %d.\x1B[0m\n", 31+rank, key, source, rank);
				break;
			}
			case ACTION_INS:
			{ // Element einfügen.
				int key;
				int valueSize;
				string value;
				printf("Checking key for Process %d.\n", source);
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, thread_comm, MPI_STATUS_IGNORE);
				printf("Received key %d from Process %d.\n", key, source);
				// Länge des Values herausfinden.
				MPI_Recv(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, thread_comm, MPI_STATUS_IGNORE);
				printf("Received length %d.\n", valueSize);
				// Puffer bereitstellen.
				char valueArray[valueSize];
				// Value empfangen.
				printf("Created Buffer. Getting Value.\n");
				MPI_Recv(valueArray, valueSize, MPI_CHAR, source, TAG_VALUE, thread_comm, MPI_STATUS_IGNORE);
				printf("Got Value.\n");
				// Value als String speichern.
				value = valueArray;
				// Eintrag einfügen.
				printf("Trying to insert.\n");
				mpiHash->InsertEntry(key, value);
				printf("\x1B[%dmInserted (%d, %s) from Process %d into HashMap by Process %d.\x1B[0m\n", 31+rank, key, valueArray, source, rank);
				break;
			}
			case ACTION_GET:
			{ // Element holen.
				int key;
				int valueSize;
				char* valueArray;
				string value = "";
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, thread_comm, MPI_STATUS_IGNORE);
				// Value holen.
				printf("Get value.\n");
				value = mpiHash->GetEntry(key);
				if (value.empty()) {
					// Falls Eintrag nicht vorhanden, sende negative Value-Länge.
					valueSize = -1;
					MPI_Ssend(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, MPI_COMM_WORLD);
				} else {
					// Falls Eintrag vorhanden, sende diesen.
					valueSize = value.length();
					valueArray = new char[valueSize];
					strcpy(valueArray, value.c_str());
					// Sende Länge des Values.
					MPI_Ssend(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, MPI_COMM_WORLD);
					// Sende Value.
					MPI_Ssend(valueArray, valueSize, MPI_CHAR, source, TAG_VALUE, MPI_COMM_WORLD);
					printf("\x1B[%dmGot (%d, %s) for Process %d from HashMap by Process %d.\x1B[0m\n", 31+rank, key, valueArray, source, rank);
				}
				break;
			}
			default:
				// Bei nicht validen Tags zum entsprechenden Label springen.
				goto WrongTag;
				break;
			}

		} else if (action == TAG_EXIT) {
			// Aus Loop ausbrechen und Thread beenden (sollte nur zum Beenden ausgeführt werden).
			printf("Terminating thread.\n");
			break;
		} else {
		WrongTag:
			printf("Warning: Tag %d from Process %d not applicable in this context.\n", action, source);
		}
	}
	pthread_exit((void *)NULL);
}

/// <summary>
/// Sendet MPI-Nachricht, um eigenen Thread zu stoppen.
/// </summary>
void KillThread() {
	MPI_Ssend(MPI_BOTTOM, 0, MPI_INT, rank, TAG_EXIT, thread_comm);
}