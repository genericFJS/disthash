#include "MPIHash.h"

const int MPIHash::HASHMAP_SIZE = 50;
int MPIHash::DISTHASH_SIZE = 4;

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