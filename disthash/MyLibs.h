#pragma once
#include "mpi.h"
#include <pthread.h>	// Thread
#include <stdio.h>		// printf
#include <stddef.h>
#include <stdarg.h>
#include <getopt.h>		// Argumente auslesen
#include <unistd.h>		// Sleep
#include <iostream>		// Eingabe
#include <string>		// String C++
#include <cstring>		// String C
#include <fstream>		// Datei lesen
#include <iterator>		// Zeilen zählen
#include <algorithm>	// Zeilen zählen (count)
using std::string;
using std::strcpy;

// Größe der HashMap (pro Prozess).
#define HASHMAP_SIZE 4096

// Anzahl der Einträge, die zufällig Eingefügt/Gelöscht/Abgefragt werden sollen.
#define RANDOM_TESTS 258000
// Anteil der zufällig ausgeführten Aktionen.
#define RANDOM_GET_PERCENT 45
#define RANDOM_DEL_PERCENT 10
#define RANDOM_INS_PERCENT 45	// wird nicht genutzt, ergibt sich implizit
// Länge der zufällig erzeugten Einträge.
#define RANDOM_MIN_STRING_LENGTH 5
#define RANDOM_MAX_STRING_LENGTH 20

// Statistik-Index:
// Neue Einträge hinzufügen
// Vorhandene Einträge ersetzen
// Vorhandene Einträge lesen
// Nicht vorhandene Einträge lesen
// Vorhandene Einträge löschen
// Nicht vorhandene Einträge löschen
// Zufällige Aktionen auf (tendenziell) vorhandenen Einträgen ausführen
// Zufällige Aktionen auf (tendenziell) nicht vorhandenen Einträgen ausführen
#define STAT_INS_NEW 0
#define STAT_INS_REPLACE 1
#define STAT_GET_EXIST 2
#define STAT_GET_NONEXIST 3
#define STAT_GET_NONEXIST_EMPTY 8
#define STAT_DEL_EXIST 4
#define STAT_DEL_NONEXIST 5
#define STAT_DEL_NONEXIST_EMPTY 9
#define STAT_ACT_RANDOM_EXIST 6
#define STAT_ACT_RANDOM 7

// Inhalt von TAG_ACTION bestimmt weitere (erwartete) Handlung.
#define TAG_EXIT		0
#define TAG_ACTION		1
#define TAG_KEY			2
#define TAG_VALUE_SIZE	3	// Alternative zu value-size über send-recieve Austasch wäre probe
#define TAG_VALUE		4
#define TAG_FEEDBACK	5
// Mögliche Inhalte von TAG_ACTION:
#define ACTION_DEL		10
#define ACTION_INS		11
#define ACTION_GET		12

void PrintColored(const char * fmt, ...);