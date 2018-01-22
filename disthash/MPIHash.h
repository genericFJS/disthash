#pragma once
#include <string>
using std::string;

class MPIHash {
private:
	HashMap hashMap;
public:
	//MPIHash();
	MPIHash(int hashmapSize);
	~MPIHash();
	int GetDistHashLocation(int key);
	int GetDistHashKey(int key);
	void InsertDistEntry(int key, string value);
	string GetDistEntry(int key);
	bool DeleteDistEntry(int key);
};

