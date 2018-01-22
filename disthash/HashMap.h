#pragma once
#include "LinkedHashEntry.h"
#include <stddef.h>

class HashMap {
private:
	int tableSize;
	LinkedHashEntry **table;
public:
	HashMap(int tableSize);
	~HashMap();
	string Get(int key);
	void Insert(int key, string value);
	bool Delete(int key);
};