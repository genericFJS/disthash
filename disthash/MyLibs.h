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
#define RANDOM_GET_PERCENT 45
#define RANDOM_DEL_PERCENT 10
#define RANDOM_INS_PERCENT 45	// wird nicht genutzt, ergibt sich eigentlich implizit
#define RANDOM_MIN_STRING_LENGTH 5
#define RANDOM_MAX_STRING_LENGTH 20

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