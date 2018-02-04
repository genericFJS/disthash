#include "MPIHash.h"

extern int rank;
extern int numProcesses;
extern MPI_Comm thread_comm;
extern int mode;

MPIHash::MPIHash(int hashmapSize) {
	hashMap = new HashMap(hashmapSize);
}

MPIHash::~MPIHash() {
}

int MPIHash::GetDistHashLocation(int key) {
	if (mode < 2) {
		// REMOTE
		return 1;
	} else {
		// DISTRIBUTED
		return key % numProcesses;
	}
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
	int destination;
	int valueSize;
	char* valueArray;
	int action = ACTION_INS;
	if (mode == 2 || (mode == 1 && rank == 0)) {
		// DISTRIBUTED / REMOTE 0
		destination = GetDistHashLocation(key);
		// Action senden.
		MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
		PrintColored("Request to insert (%d, %s) into Process' %d HashMap for Process %d sent.\n", key, value.c_str(), destination, rank);
		// Key senden.
		MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
		// Value Größe ermitteln.
		valueSize = value.size() + 1; 
		// Value in char-Array zwischen speichen.
		valueArray = new char[valueSize];
		value.copy(valueArray, valueSize);
		valueArray[valueSize - 1] = '\0';
		// Sende Länge des Values.
		MPI_Ssend(&valueSize, 1, MPI_INT, destination, TAG_VALUE_SIZE, thread_comm);
		// Sende Value.
		MPI_Ssend(valueArray, valueSize, MPI_CHAR, destination, TAG_VALUE, thread_comm);
	} else if (mode == 0) {
		// LOCAL
		this->InsertEntry(key, value);
		PrintColored("Inserted entry (%d, %s).\n", key, value.c_str());
	}
}


string MPIHash::GetDistEntry(int key) {
	string value;
	int destination;
	int valueSize;
	char* valueArray;
	int action = ACTION_GET;
	if (mode == 2 || (mode == 1 && rank == 0)) {
		// DISTRIBUTED / REMOTE 0
		destination = GetDistHashLocation(key);
		// Action senden.
		MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
		PrintColored("Request to get entry (%d, ?) in Process' %d HashMap for Process %d sent.\n", key, destination, rank);
		// Key senden.
		MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
		// Länge des Values herausfinden.
		MPI_Recv(&valueSize, 1, MPI_INT, destination, TAG_VALUE_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// Bei leerem Eintrag nichts weiter empfangen, sonst schon.
		if (valueSize < 0) {
			value = "N/A";
		} else {
			// Puffer bereitstellen.
			valueArray = new char[valueSize];
			// Value empfangen.
			MPI_Recv(valueArray, valueSize, MPI_CHAR, destination, TAG_VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// Value als String speichern.
			value = valueArray;
		}
	} else if (mode == 0) {
		// LOCAL
		value = this->GetEntry(key);
		PrintColored("Got entry (%d, %s).\n", key, value.c_str());
	}
	return value;
}


bool MPIHash::DeleteDistEntry(int key) {
	bool deleted = false;
	int destination;
	int feedback;
	int action = ACTION_DEL;
	if (mode == 2 || (mode == 1 && rank == 0)) {
		// DISTRIBUTED / REMOTE 0
		destination = GetDistHashLocation(key);
		// Action senden.
		MPI_Ssend(&action, 1, MPI_INT, destination, TAG_ACTION, thread_comm);
		PrintColored("Request to delete entry (%d, ?) in Process' %d HashMap for Process %d sent.\n", key, destination, rank);
		// Key senden.
		MPI_Ssend(&key, 1, MPI_INT, destination, TAG_KEY, thread_comm);
		// Feedback bekommen.
		MPI_Recv(&feedback, 1, MPI_INT, destination, TAG_FEEDBACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (feedback == 1)
			deleted = true;
	} else if (mode == 0) {
		// LOCAL
		deleted = this->DeleteEntry(key);
		if (deleted)
			PrintColored("Deleted entry (%d, ?).\n", key);
		else
			PrintColored("Could not delete entry (%d, ?).\n", key);
	}
	return deleted;
}