#include "HashMap.h"

/// <summary>
/// Initialisiert die HashMap: Ein Tabelle von LinkedHashEntrys der entsprechenden Größe wird angelegt und vorerst mit NULL gefüllt.
/// </summary>
/// <param name="tableSize">Größe der HashMap (Anzahl der Einträge)</param>
HashMap::HashMap(int tableSize) {
	this->tableSize = tableSize;
	table = new LinkedHashEntry*[tableSize];
	for (int i = 0; i < tableSize; i++)
		table[i] = NULL;
}

/// <summary>
/// Alle Elemente von vorhandenen LinkedHashEntrys werden vor dem Löschen der Tabelle gelöscht.
/// </summary>
HashMap::~HashMap() {
	for (int i = 0; i < tableSize; i++) {
		if (table[i] != NULL) {
			LinkedHashEntry *prevEntry = NULL;
			LinkedHashEntry *entry = table[i];
			while (entry != NULL) {
				prevEntry = entry;
				entry = entry->getNext();
				delete prevEntry;
			}
		}
	}
	delete[] table;
}

/// <summary>
/// Gibt den Eintrag (Value) des entsprechenden Keys. Dazu wird gegebenenfalls der LinkedHashEntry durchsucht.
/// </summary>
/// <param name="key">Key des Eintrags.</param>
/// <returns>NULL, wenn Key nicht vorhanden. Value, sonst.</returns>
string HashMap::Get(int key) {
	int hash = (key % tableSize);
	if (table[hash] == NULL) {
		return NULL;
	} else {
		LinkedHashEntry *entry = table[hash];
		while (entry != NULL && entry->getKey() != key)
			entry = entry->getNext();
		if (entry == NULL)
			return NULL;
		else
			return entry->getValue();
	}
}

/// <summary>
/// Fügt einen Eintrag (Key mit entsprechendem Value) in die HashMap ein. Falls noch kein Eintrag an der HashMap-Stelle existiert, wird ein neuer erzeugt.
/// Ansonsten wird in der LinkkedHashEntry nach dem Key gesucht und ggf. ein neuer Eintrag angehängt.
/// </summary>
/// <param name="key">Der Key des zu erstellenden Eintrags.</param>
/// <param name="value">Der Value des zu erstellenden Eintrags.</param>
void HashMap::Insert(int key, string value) {
	int hash = (key % tableSize);
	if (table[hash] == NULL) {
		table[hash] = new LinkedHashEntry(key, value);
	} else {
		LinkedHashEntry *entry = table[hash];
		while (entry->getNext() != NULL && entry->getKey() != key)
			entry = entry->getNext();
		if (entry->getKey() == key)
			entry->setValue(value);
		else
			entry->setNext(new LinkedHashEntry(key, value));
	}
}

/// <summary>
/// Löscht ein Element aus der HashMap anhand des Keys.
/// </summary>
/// <param name="key">Der Key des zu löschenden Eintrags.</param>
/// <returns>true, wenn Eintrag gelöscht wurde. false, wenn Eintrag nicht gelöscht wurde (weil er nicht existiert).</returns>
bool HashMap::Delete(int key) {
	int hash = (key % tableSize);
	// Überprüfe, ob Element existiert. Sonst kann nichts gelöscht werden.
	if (table[hash] != NULL) {
		LinkedHashEntry *prevEntry = NULL;
		LinkedHashEntry *entry = table[hash];
		// Durchsuche LinkedHashEntry nach Element.
		while (entry->getNext() != NULL && entry->getKey() != key) {
			prevEntry = entry;
			entry = entry->getNext();
		}
		// Wenn Element gefunden (oder letztes Element), lösche es.
		if (entry->getKey() == key) {
			// Wenn Element am Anfang steht, lösche es und setze das nächste Element als Startelement des LinkedHashEntry. Ansonsten nehme Element aus dem LinkedHashEntry.
			if (prevEntry == NULL) {
				LinkedHashEntry *nextEntry = entry->getNext();
				delete entry;
				table[hash] = nextEntry;
			} else {
				LinkedHashEntry *next = entry->getNext();
				delete entry;
				prevEntry->setNext(next);
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}