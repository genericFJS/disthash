#pragma once
#include "MyLibs.h"

class LinkedHashEntry {
private:
	int key;
	int value;
	LinkedHashEntry *next;
public:
	LinkedHashEntry(int key, int value);
	int getKey();
	int getValue();
	void setValue(int value);
	LinkedHashEntry *getNext();
	void setNext(LinkedHashEntry *next);
};