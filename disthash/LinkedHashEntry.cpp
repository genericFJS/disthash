/// <summary>
/// LinkedHashEntry beschreibt einen Eintrag in der HashMap, der zeitgleich auch eine (einfach verkettete) �berlaufliste darstellt.
/// </summary>
#include "LinkedHashEntry.h"

/// <summary>
/// Erstelle neue verkettete Liste, die mit dem gegebenen Schl�ssel und Wert als Eintrag beginnt.
/// </summary>
/// <param name="key">Schl�ssel des neuen Eintrags</param>
/// <param name="value">Wert des neuen Eintrags</param>
LinkedHashEntry::LinkedHashEntry(int key, string value) {
	this->key = key;
	this->value = value;
	this->next = NULL;
}

/// <summary>
/// Gibt den Schl�ssel des Eintrags zur�ck.
/// </summary>
/// <returns>Schl�ssel des Eintrags</returns>
int LinkedHashEntry::getKey() {
	return key;
}

/// <summary>
/// Gibt den Wert des Eintrags zur�ck.
/// </summary>
/// <returns>Wert (Zeichenkette) des Eintrags</returns>
string LinkedHashEntry::getValue() {
	return value;
}

/// <summary>
/// Setzt den Wert des Eintrags auf den �bergebenen.
/// </summary>
/// <param name="value">Neuer Wert des Eintrags</param>
void LinkedHashEntry::setValue(string value) {
	this->value = value;
}

/// <summary>
/// Gibt den n�chsten Eintrag zur�ck.
/// </summary>
/// <returns>N�chster Eintrag</returns>
LinkedHashEntry* LinkedHashEntry::getNext() {
	return next;
}

/// <summary>
/// Setzt den n�chsten Eintrag.
/// </summary>
/// <param name="next">Neuer n�chster Eintrag</param>
void LinkedHashEntry::setNext(LinkedHashEntry *next) {
	this->next = next;
}