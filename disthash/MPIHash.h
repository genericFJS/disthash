#pragma once
#include "HashMap.h"
#include "MyLibs.h"

class MPIHash {
private:
	HashMap* hashMap;
public:
	//MPIHash();
	MPIHash(int hashmapSize);
	~MPIHash();
	int GetDistHashLocation(int key);
	void InsertEntry(int key, int value);
	void InsertDistEntry(int key, int value);
	int GetEntry(int key);
	int GetDistEntry(int key);
	bool DeleteEntry(int key);
	bool DeleteDistEntry(int key);
};

