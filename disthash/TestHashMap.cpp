#include "TestHashMap.h"

TestHashMap::TestHashMap() {
	HashMap* hashMap = new HashMap(MPIHash::HASHMAP_SIZE);

	int key1 = 42;
	int key2 = MPIHash::HASHMAP_SIZE + 42;
	int key3 = 21;

	hashMap->Insert(key1, "Hans");
	hashMap->Insert(key2, "Brunhilde");
	hashMap->Insert(key3, "G�nter");

	std::cout << "Eintrag " << key1 << " enth�lt " << hashMap->Get(key1) << "." << std::endl;
	std::cout << "Eintrag " << key2 << " enth�lt " << hashMap->Get(key2) << "." << std::endl;
	std::cout << "Eintrag " << key3 << " enth�lt " << hashMap->Get(key3) << "." << std::endl;
}


TestHashMap::~TestHashMap() {
}