#pragma once
#include "mpi.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <locale>
#include <clocale>
#include <stddef.h>
#include <string>
#include <cstring>
using std::string;
using std::strcpy;

// Größe der HashMap (pro Prozess)
#define HASHMAP_SIZE 50

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
