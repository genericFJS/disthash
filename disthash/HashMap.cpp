#include "HashMap.h"

extern int numProcesses;

/// <summary>
/// Initialisiert die HashMap: Ein Tabelle von LinkedHashEntrys der entsprechenden Gr��e wird angelegt und vorerst mit NULL gef�llt.
/// </summary>
/// <param name="tableSize">Gr��e der HashMap (Anzahl der Eintr�ge)</param>
HashMap::HashMap(int tableSize) {
	table = new LinkedHashEntry*[HASHMAP_SIZE];
	for (int i = 0; i < tableSize; i++)
		table[i] = NULL;
}

/// <summary>
/// Alle Elemente von vorhandenen LinkedHashEntrys werden vor dem L�schen der Tabelle gel�scht.
/// </summary>
HashMap::~HashMap() {
	for (int i = 0; i < HASHMAP_SIZE; i++) {
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


int HashMap::GetHashKey(int key) {
	return (key / numProcesses) % HASHMAP_SIZE;
}

/// <summary>
/// Gibt den Eintrag (Value) des entsprechenden Keys. Dazu wird gegebenenfalls der LinkedHashEntry durchsucht.
/// </summary>
/// <param name="key">Key des Eintrags.</param>
/// <returns>Leer, wenn Key nicht vorhanden. Value, sonst.</returns>
int HashMap::Get(int key) {
	int hash = GetHashKey(key);
	if (table[hash] == NULL) {
		return -1;
	} else {
		LinkedHashEntry *entry = table[hash];
		while (entry != NULL && entry->getKey() != key)
			entry = entry->getNext();
		if (entry == NULL)
			return -1;
		else
			return entry->getValue();
	}
}

/// <summary>
/// F�gt einen Eintrag (Key mit entsprechendem Value) in die HashMap ein. Falls noch kein Eintrag an der HashMap-Stelle existiert, wird ein neuer erzeugt.
/// Ansonsten wird in der LinkkedHashEntry nach dem Key gesucht und ggf. ein neuer Eintrag angeh�ngt.
/// </summary>
/// <param name="key">Der Key des zu erstellenden Eintrags.</param>
/// <param name="value">Der Value des zu erstellenden Eintrags.</param>
void HashMap::Insert(int key, int value) {
	int hash = GetHashKey(key);
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
/// L�scht ein Element aus der HashMap anhand des Keys.
/// </summary>
/// <param name="key">Der Key des zu l�schenden Eintrags.</param>
/// <returns>true, wenn Eintrag gel�scht wurde. false, wenn Eintrag nicht gel�scht wurde (weil er nicht existiert).</returns>
bool HashMap::Delete(int key) {
	int hash = GetHashKey(key);
	// �berpr�fe, ob Element existiert. Sonst kann nichts gel�scht werden.
	if (table[hash] != NULL) {
		LinkedHashEntry *prevEntry = NULL;
		LinkedHashEntry *entry = table[hash];
		// Durchsuche LinkedHashEntry nach Element.
		while (entry->getNext() != NULL && entry->getKey() != key) {
			prevEntry = entry;
			entry = entry->getNext();
		}
		// Wenn Element gefunden (oder letztes Element), l�sche es.
		if (entry->getKey() == key) {
			// Wenn Element am Anfang steht, l�sche es und setze das n�chste Element als Startelement des LinkedHashEntry. Ansonsten nehme Element aus dem LinkedHashEntry.
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