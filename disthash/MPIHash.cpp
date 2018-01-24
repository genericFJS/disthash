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

void MPIHash::InsertDistEntry(int key, string value) {
	int destination = GetDistHashLocation(key);
	int valueSize;
	char* valueArray;
	int action = ACTION_INS;
	printf("Sending request to insert (%d, %s) into Process' %d HashMap for Process %d.\n", key, value.c_str(), destination, rank);
	// Action senden.
	MPI_Send(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
	// Key senden.
	printf("\t SEND KEY from %d.\n", rank);
	MPI_Send(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
	printf("\t KEY SENT from %d.\n", rank);
	// Value senden.
	valueSize = value.length();
	valueArray = new char[valueSize];
	strcpy(valueArray, value.c_str());
	// Sende Länge des Values.
	printf("\t SEND VALUE SIZE from %d.\n", rank);
	MPI_Send(&valueSize, 1, MPI_INT, destination, TAG_VALUE_SIZE, thread_comm);
	printf("\t VALUE SIZE SENT from %d.\n", rank);
	// Sende Value.
	printf("\t SEND VALUE.\n");
	MPI_Send(valueArray, valueSize, MPI_CHAR, destination, TAG_VALUE, MPI_COMM_WORLD);
	printf("\t VALUE SENT.\n");
}

string MPIHash::GetEntry(int key) {
	return hashMap->Get(key);
}

string MPIHash::GetDistEntry(int key) {
	return string();
}

bool MPIHash::DeleteEntry(int key) {
	return hashMap->Delete(key);
}

bool MPIHash::DeleteDistEntry(int key) {
	return false;
}