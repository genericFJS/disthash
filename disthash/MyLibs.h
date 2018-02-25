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
#include <iterator>		// Zeilen z�hlen
#include <algorithm>	// Zeilen z�hlen (count)
#include <numeric>		// Zuf�llige Reihenfolge
#include <vector>		
#include <random>		// Zufall
#include <limits>		// Grenze von Zahlen (f�r Bereichsgrenzen)
using std::string;
using std::strcpy;

// Gr��e der HashMap (pro Prozess).
#define HASHMAP_SIZE 4096

// Anzahl der Eintr�ge, die zuf�llig Eingef�gt/Gel�scht/Abgefragt werden sollen.
#define RANDOM_TESTS 258000
// Anteil der zuf�llig ausgef�hrten Aktionen.
#define RANDOM_GET_PERCENT 45
#define RANDOM_DEL_PERCENT 10
#define RANDOM_INS_PERCENT 45	// wird nicht genutzt, ergibt sich implizit
// L�nge der zuf�llig erzeugten Eintr�ge.
#define RANDOM_MIN_STRING_LENGTH 5
#define RANDOM_MAX_STRING_LENGTH 20

// Modi
#define MODE_LOCAL 0
#define MODE_REMOTE 1
#define MODE_DISTRIBUTED 2

// Statistik-Index:
// Neue Eintr�ge hinzuf�gen
// Vorhandene Eintr�ge ersetzen
// Vorhandene Eintr�ge lesen
// Nicht vorhandene Eintr�ge lesen
// Vorhandene Eintr�ge l�schen
// Nicht vorhandene Eintr�ge l�schen
// Zuf�llige Aktionen auf (tendenziell) vorhandenen Eintr�gen ausf�hren
// Zuf�llige Aktionen auf (tendenziell) nicht vorhandenen Eintr�gen ausf�hren
// Vorhandene Eintr�ge zuf�llig ersetzen
// (Tendenziell) Nicht vorhandene Eintr�ge zuf�llig einf�gen
#define STAT_INS_EMPTY 0
#define STAT_INS_MOSTLYNONEXIST_RANDOM 1
#define STAT_INS_EXIST 2
#define STAT_INS_EXIST_RANDOM 3
#define STAT_GET_EXIST_RANDOM 4
#define STAT_GET_NONEXIST_RANDOM 5
#define STAT_GET_EMPTY_NONEXIST 6
#define STAT_DEL_EXIST 7
#define STAT_DEL_NONEXIST 8
#define STAT_DEL_EMPTY_NONEXIST 9
#define STAT_ACT_MOSTLYEXIST_RANDOM 10
#define STAT_ACT_MOSTLYNONEXIST_RANDOM 11

// Inhalt von TAG_ACTION bestimmt weitere (erwartete) Handlung.
#define TAG_EXIT		0
#define TAG_ACTION		1
#define TAG_KEY			2
#define TAG_VALUE_SIZE	3	// Alternative zu value-size �ber send-recieve Austasch w�re probe
#define TAG_VALUE		4
#define TAG_FEEDBACK	5
// M�gliche Inhalte von TAG_ACTION:
#define ACTION_DEL		10
#define ACTION_INS		11
#define ACTION_GET		12

void PrintColored(const char * fmt, ...);