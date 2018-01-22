#pragma once
#include <stddef.h>
#include <string>
using std::string;

class LinkedHashEntry {
private:
	int key;
	string value;
	LinkedHashEntry *next;
public:
	LinkedHashEntry(int key, string value);
	int getKey();
	string getValue();
	void setValue(string value);
	LinkedHashEntry *getNext();
	void setNext(LinkedHashEntry *next);
};