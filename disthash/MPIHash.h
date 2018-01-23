#pragma once
#include "HashMap.h"
#include "MyLibs.h"

class MPIHash {
private:
	HashMap* hashMap;
public:
	static const int HASHMAP_SIZE;
	static int DISTHASH_SIZE;
	//MPIHash();
	MPIHash(int hashmapSize);
	~MPIHash();
	int GetDistHashLocation(int key);
	int GetDistHashKey(int key);
	void InsertDistEntry(int key, string value);
	string GetDistEntry(int key);
	bool DeleteDistEntry(int key);
};

