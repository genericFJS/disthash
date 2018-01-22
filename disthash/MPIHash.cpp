#include "MPIHash.h"
#include <iostream>
#include "HashMap.h"

static const int HASHMAP_SIZE = 50;
static int DISTHASH_SIZE = 4;
static int VERBOSE = 1;

MPIHash::MPIHash(int hashmapSize) {
	hashMap = new HashMap(HASHMAP_SIZE);
}

MPIHash::~MPIHash() {
}

int MPIHash::GetDistHashLocation(int key) {
	return key % DISTHASH_SIZE;
}


int MPIHash::GetDistHashKey(int key) {
	return (key / DISTHASH_SIZE) % HASHMAP_SIZE;
}


void MPIHash::InsertDistEntry(int key, string value) {
	
}

string MPIHash::GetDistEntry(int key) {

}

bool MPIHash::DeleteDistEntry(int key) {

}

int main() {
	MPIHash* mpiHash = new MPIHash(HASHMAP_SIZE);
	//std::cout << "Hello World!" << std::endl;

	mpiHash->InsertDistEntry(5, "Alex");
	mpiHash->InsertDistEntry(55, "Thomas");
	mpiHash->InsertDistEntry(56, "Brunhilde");

	return 0;
}
