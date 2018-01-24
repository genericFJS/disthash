#pragma once
#include "LinkedHashEntry.h"
#include "MyLibs.h"

class HashMap {
private:
	LinkedHashEntry **table;
public:
	HashMap(int tableSize);
	~HashMap();
	int GetHashKey(int key);
	int Get(int key);
	void Insert(int key, int value);
	bool Delete(int key);
};