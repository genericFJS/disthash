#include "MPIHashThread.h"

extern int rank;
extern MPIHash* mpiHash;

void* MPIHashThread(void* ptr) {
	int actionCode;
	MPI_Comm thread_comm = *(MPI_Comm*)ptr;
	MPI_Status status;
	int len;
	// char name[MPI_MAX_PROCESSOR_NAME];
	// MPI_Get_processor_name(name, &len);
	// PrintColored("\tStarting thread for Process %d on %s.\n", rank, name);

	// Schleife zum Abarbeiten von HashMap-Aktivt�ten
	while (true) {
		// PrintColored("\tWaiting for something to happen in Process %d.\n", rank);
		// Warte, bis eine Aktion gesendet wird.
		MPI_Recv(&actionCode, 500, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, thread_comm, &status);
		int action = status.MPI_TAG;
		int source = status.MPI_SOURCE;
		// PrintColored("\tGot action in Process %d by %d.\n", rank, source);
		// Action-Tag gibt an, was geschehen soll.
		//========================================
		if (action == TAG_ACTION) {
			switch (actionCode) {
			case ACTION_DEL:
			{ // Element l�schen.
				int key;
				bool deleted;
				int feedback;
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, thread_comm, MPI_STATUS_IGNORE);
				// Eintrag (versuchen zu) l�schen.
				deleted = mpiHash->DeleteEntry(key);
				// Feedback formulieren (1: Erfolg, 0: Misserfolg).
				deleted ? feedback = 1 : feedback = 0;
				// Feedback senden.
				MPI_Ssend(&feedback, 1, MPI_INT, source, TAG_FEEDBACK, MPI_COMM_WORLD);
				if (deleted)
					PrintColored("\tDeleted (%d, -) for Process %d from HashMap by Process %d.\n", key, source, rank);
				else
					PrintColored("\tCould not delete (%d, -) for Process %d from HashMap by Process %d.\n", key, source, rank);
				break;
			}
			case ACTION_INS:
			{ // Element einf�gen.
				int key;
				int valueSize;
				char* valueArray;
				string value;
				//printf("Checking key for Process %d.\n", source);
				// Key herausfinden.
				MPI_Recv(&key, 1, MPI_INT, source, TAG_KEY, thread_comm, MPI_STATUS_IGNORE);
				//printf("Received key %d from Process %d.\n", key, source);
				// L�nge des Values herausfinden.
				MPI_Recv(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, thread_comm, MPI_STATUS_IGNORE);
				//printf("Received length %d.\n", valueSize);
				// Puffer bereitstellen.
				valueArray = new char[valueSize];
				// Value empfangen.
				//printf("Created Buffer. Getting Value.\n");
				MPI_Recv(valueArray, valueSize, MPI_CHAR, source, TAG_VALUE, thread_comm, MPI_STATUS_IGNORE);
				//printf("Got Value.\n");
				// Value als String speichern.
				value = valueArray;
				// Eintrag einf�gen.
				//printf("Trying to insert.\n");
				mpiHash->InsertEntry(key, value);
				PrintColored("\tInserted (%d, %s) from Process %d into HashMap by Process %d.\n", key, valueArray, source, rank);
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
				value = mpiHash->GetEntry(key);
				if (value.empty()) {
					// Falls Eintrag nicht vorhanden, sende negative Value-L�nge.
					valueSize = -1;
					MPI_Ssend(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, MPI_COMM_WORLD);
					PrintColored("\tEntry (%d, ?) for Process %d from HashMap by Process %d is empty.\n", key, source, rank);
				} else {
					// Falls Eintrag vorhanden, sende diesen.
					valueSize = value.size() + 1;
					char* valueArray = new char[valueSize];
					value.copy(valueArray, valueSize);
					valueArray[valueSize - 1] = '\0';
					// Sende L�nge des Values.
					MPI_Ssend(&valueSize, 1, MPI_INT, source, TAG_VALUE_SIZE, MPI_COMM_WORLD);
					// Sende Value.
					MPI_Ssend(valueArray, valueSize, MPI_CHAR, source, TAG_VALUE, MPI_COMM_WORLD);
					PrintColored("\tGot (%d, %s) for Process %d from HashMap by Process %d.\n", key, valueArray, source, rank);
				}
				break;
			}
			default:
				// Bei nicht validen Tags zum entsprechenden Label springen.
				goto WrongTag;
				break;
			}

		} else if (action == TAG_EXIT) {
			// Aus Loop ausbrechen und Thread beenden (sollte nur zum Beenden ausgef�hrt werden).
			//PrintColored("\tBye from Process %d.\n", rank);
			break;
		} else {
		WrongTag:
			PrintColored("\tWarning: Tag %d from Process %d not applicable in this context.\n", action, source);
		}
	}
	// PrintColored("\tBye from thread %d.\n", rank);
	pthread_exit((void *)NULL);
}

/// <summary>
/// Sendet MPI-Nachricht, um eigenen Thread zu stoppen.
/// </summary>
bool KillThread() {
	extern MPI_Comm thread_comm;
	MPI_Ssend(MPI_BOTTOM, 0, MPI_INT, rank, TAG_EXIT, thread_comm);
	return true;
}