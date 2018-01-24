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
void MPIHash::InsertEntry(int key, int value) {
	hashMap->Insert(key, value);
}

void MPIHash::InsertDistEntry(int key, int value) {
	int destination = GetDistHashLocation(key);
	int valueSize;
	//char* valueArray;
	int action = ACTION_INS;
	// Action senden.
	MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, MPI_COMM_WORLD);
	printf("Sending request to insert (%d, %d) into Process' %d HashMap for Process %d.\n", key, value, destination, rank);
	// Key senden.
	printf("\t SEND KEY from %d.\n", rank);
	MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, MPI_COMM_WORLD);
	printf("\t KEY SENT from %d.\n", rank);
	// Sende Value.
	printf("\t SEND VALUE from %d.\n", rank);
	MPI_Send(&value, 1, MPI_INT, destination, TAG_VALUE, MPI_COMM_WORLD);
	printf("\t VALUE SENT from %d.\n", rank);
}

int MPIHash::GetEntry(int key) {
	return hashMap->Get(key);
}

int MPIHash::GetDistEntry(int key) {
	return -1;
}

bool MPIHash::DeleteEntry(int key) {
	return hashMap->Delete(key);
}

bool MPIHash::DeleteDistEntry(int key) {
	return false;
}