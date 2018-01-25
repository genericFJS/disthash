#include "MPIHash.h"

extern int rank;
extern int numProcesses;
extern MPI_Comm thread_comm;

MPIHash::MPIHash(int hashmapSize) {
	hashMap = new HashMap(HASHMAP_SIZE);
}

MPIHash::~MPIHash() {
}

int MPIHash::GetDistHashLocation(int key) {
	return key % numProcesses;
}

/// <summary>
/// Fügt Eintrag in die lokale HashMap ein. Nur vom zuständigen Thread auszuführen!
/// </summary>
/// <param name="key">Key</param>
/// <param name="value">Value</param>
void MPIHash::InsertEntry(int key, string value) {
	hashMap->Insert(key, value);
}

string MPIHash::GetEntry(int key) {
	return hashMap->Get(key);
}

bool MPIHash::DeleteEntry(int key) {
	return hashMap->Delete(key);
}

void MPIHash::InsertDistEntry(int key, string value) {
	int destination = GetDistHashLocation(key);
	int valueSize;
	int action = ACTION_INS;
	// Action senden.
	//printf("\x1B[%dmTrying to send request to insert (%d, %s) into Process' %d HashMap for Process %d.\x1B[0m\n", 31 + rank, key, value.c_str(), destination, rank);
	MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
	printf("\x1B[%dmRequest to insert (%d, %s) into Process' %d HashMap for Process %d sent.\x1B[0m\n", 31+rank, key, value.c_str(), destination, rank);
	// Key senden.
	//printf("\t SEND KEY from %d.\n", rank);
	MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
	//printf("\t KEY SENT from %d.\n", rank);
	// Value senden.
	valueSize = value.size();
	// Sende Länge des Values.
	//printf("\t SEND VALUE SIZE %d from %d.\n", valueSize, rank);
	MPI_Ssend(&valueSize, 1, MPI_INT, destination, TAG_VALUE_SIZE, thread_comm);
	//printf("\t VALUE SIZE SENT from %d.\n", rank);
	// Sende Value.
	//printf("\t SEND VALUE from %d.\n", rank);
	MPI_Ssend(value.c_str(), valueSize, MPI_CHAR, destination, TAG_VALUE, thread_comm);
	//printf("\t VALUE SENT from %d.\n", rank);
}


string MPIHash::GetDistEntry(int key) {
	int destination = GetDistHashLocation(key);
	int valueSize;
	string value;
	int action = ACTION_GET;
	// Action senden.
	MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
	printf("\x1B[%dmRequest to get entry (%d, ?) in Process' %d HashMap for Process %d sent.\x1B[0m\n", 31 + rank, key, destination, rank);
	// Key senden.
	MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
	// Länge des Values herausfinden.
	MPI_Recv(&valueSize, 1, MPI_INT, destination, TAG_VALUE_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	// Bei leerem Eintrag nichts weiter empfangen, sonst schon.
	if (valueSize < 0) {
		value = "N/A";
	} else {
		// Puffer bereitstellen.
		char valueArray[valueSize];
		// Value empfangen.
		MPI_Recv(valueArray, valueSize, MPI_CHAR, destination, TAG_VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// Value als String speichern.
		value = valueArray;
	}
	return value;
}


bool MPIHash::DeleteDistEntry(int key) {
	int destination = GetDistHashLocation(key);
	int feedback;
	bool deleted = false;
	int action = ACTION_DEL;
	// Action senden.
	MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
	printf("\x1B[%dmRequest to delete entry (%d, ?) in Process' %d HashMap for Process %d sent.\x1B[0m\n", 31 + rank, key, destination, rank);
	// Key senden.
	MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
	// Feedback bekommen.
	MPI_Recv(&feedback, 1, MPI_INT, destination, TAG_FEEDBACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (feedback == 1) 
		deleted = true;
	//printf("\x1B[%dmDelete entry (%d, ?) in Process' %d HashMap for Process %d complete.\x1B[0m\n", 31 + rank, key, destination, rank);
	return deleted;
}