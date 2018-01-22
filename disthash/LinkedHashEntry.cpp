#include "LinkedHashEntry.h"

LinkedHashEntry::LinkedHashEntry(int key, string value) {
	this->key = key;
	this->value = value;
	this->next = NULL;
}

int LinkedHashEntry::getKey() {
	return key;
}

string LinkedHashEntry::getValue() {
	return value;
}

void LinkedHashEntry::setValue(string value) {
	this->value = value;
}

LinkedHashEntry* LinkedHashEntry::getNext() {
	return next;
}

void LinkedHashEntry::setNext(LinkedHashEntry *next) {
	this->next = next;
}