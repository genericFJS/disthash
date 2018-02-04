#include "LinkedHashEntry.h"
#include "HashMap.h"
#include "MPIHash.h"
#include "MPIHashThread.h"
#include "MyLibs.h"

int providedThreads = 0, numProcesses = 1, rank = 0;
pthread_t thread;
MPI_Comm thread_comm;
MPI_Status status;
MPIHash* mpiHash;
int hashMapSize = HASHMAP_SIZE;
int randomTests = RANDOM_TESTS;

bool verbose = false;
// 0: local HashMap, 1: remote HashMap, 2: distributed HashMap 
int mode = 2;
bool skipTests = false;
HashMap* hashMap = new HashMap(hashMapSize);
double startTime, endTime, totalTime;
// Ergebnisse:
// 0: Neue Eintr�ge hinzuf�gen
// 1: Vorhandene Eintr�ge ersetzen
// 2: Vorhandene Eintr�ge lesen
// 3: Nicht vorhandene Eintr�ge lesen
// 4: Vorhandene Eintr�ge l�schen
// 5: Nicht vorhandene Eintr�ge l�schen
// 6: Zuf�llige Aktionen auf (tendenziell) vorhandenen Eintr�gen ausf�hren
// 7: Zuf�llige Aktionen auf (tendenziell) nicht vorhandenen Eintr�gen ausf�hren
double testResults[8];

void PrintOnce(const char * text, ...) {
	if (rank == 0) {
		va_list args;
		va_start(args, text);
		vprintf(text, args);
		va_end(args);
	}
}

void MeasurementStart() {
	startTime = MPI_Wtime();
}

void MeasurementEnd() {
	endTime = MPI_Wtime();
	totalTime = endTime - startTime;
}

void MeasurementEnd(int i) {
	MeasurementEnd();
	testResults[i] = totalTime;
}

/// <summary>
/// Erzeugt eine zuf�llige Zahl zwischen zwei Werten. Quelle: https://stackoverflow.com/a/11766794/8805428
/// </summary>
/// <param name="min">Kleinste m�gliche Zahl</param>
/// <param name="max">Gr��te m�gliche Zahl</param>
/// <returns>Zuf�llige Zahl zwischen min und max.</returns>
int RandBetween(int min, int max) {
	int n = max - min + 1;
	int remainder = RAND_MAX % n;
	int x;
	do {
		x = rand();
	} while (x >= RAND_MAX - remainder);
	return min + x % n;
}

/// <summary>
/// Erzeugt eine zuf�llige Zeichenkette der gegebenen L�nge. Quelle: https://stackoverflow.com/a/12468109/8805428
/// </summary>
/// <param name="length">L�nge des zuf�lligen Strings</param>
/// <returns>Zuf�lligen String</returns>
std::string RandString(size_t length) {
	auto randchar = []() -> char {
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

int ExitApp() {
	// Warte, bis alle Prozesse bereit zum Beenden sind.
	MPI_Barrier(MPI_COMM_WORLD);
	// Beim beenden, Thread auch beenden.
	KillThread();
	usleep(1000);
	MPI_Finalize();
	PrintOnce("==================================\nThank you for using DistHash by FJS.\n");
	return 0;
}

int main(int argc, char *argv[]) {
	// MPI mit Thread initialisieren.
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &providedThreads);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (providedThreads < MPI_THREAD_MULTIPLE)
		MPI_Abort(MPI_COMM_WORLD, 0);

	// F�r jeden Prozess Thread mit gesondertem Communicator erstellen.
	MPI_Comm_dup(MPI_COMM_WORLD, &thread_comm);
	pthread_create(&thread, NULL, MPIHashThread, &thread_comm);

	// ================================================================
	//                Vorbereitung/Optionen einlesen
	// ================================================================

	int opt;
	// Fehler selber abfangen (return '?'): 
	opterr = 0;

	// Optionen Verarbeiten:
	while ((opt = getopt(argc, argv, "vlrdsh:n:")) != -1) {
		switch (opt) {
		case 'v':
			// verbose: Zeige Vorg�nge an
			verbose = true;
			break;
		case 'l':
			// (l)ocal mode: HashMap wird lokal erstellt und verwaltet.
			mode = 0;
			if (numProcesses > 1) {
				PrintOnce("LOCAL tests require n=1 process (has %d).\n", numProcesses);
				return ExitApp();
			}
			break;
		case 'r':
			// (r)emote mode: HashMap wird von entferntem Prozess 1 verwaltet, Prozess 0 macht nur Anfragen. 
			mode = 1;
			if (numProcesses > 2) {
				PrintOnce("REMOTE tests require n=2 processes (has %d).\n", numProcesses);
				return ExitApp();
			}
			break;
		case 'd':
			// (d)istributed mode: HashMap wird von allen Prozessen verwaltet, jeder Prozess kann Anfragen ausf�hren.
			mode = 2;
			break;
		case 's':
			// (s)kip tests: Tests werden nicht ausgef�hrt.
			skipTests = true;
			break;
		case 'h':
			// (h)ashMap size: Gr��e der HashMap (pro Prozess)
			if (optarg) {
				hashMapSize = atoi(optarg);
			}
			break;
		case 'n':
			// (n)umber of tests: Anzahl der durchzuf�hrenden zuf�lligen Tests.
			if (optarg) {
				randomTests = atoi(optarg);
			}
			break;
		case '?':
			PrintOnce("Unknown option: '%c'!\n", char(optopt));
			break;
		}
	}

	string modeString = "";
	switch (mode) {
	case 0:
		modeString = "LOCAL";
		break;
	case 1:
		modeString = "REMOTE";
		break;
	case 2:
		modeString = "DISTRIBUTED (default)";
		break;
	}

	PrintOnce("==================================\nWelcome to DistHash by FJS.\n");
	PrintOnce("Mode: %s\t Processes: %d  HashMap size: %d\n==================================\n", modeString.c_str(), numProcesses, hashMapSize);

	// Die (MPI-)HashMap erstellen.
	mpiHash = new MPIHash(hashMapSize);

	// ================================================================
	//                   Datei zwischenspeichern
	// ================================================================

	// Einlesen der Test-Datei in Zwischenpseicher
	std::ifstream inputFile("baby-names.csv");

	// Z�hle die Anzahl der Zeilen
	inputFile.unsetf(std::ios_base::skipws);
	unsigned dataEntrys = std::count(std::istream_iterator<char>(inputFile), std::istream_iterator<char>(), '\n');

	// Setze filepointer zur�ck
	inputFile.clear();
	inputFile.seekg(0, std::ios::beg);

	// Erstelle Zwischenspeicher
	int processEntrys = (dataEntrys - 1) / numProcesses;
	//string inputData[processEntrys];
	string* inputData = new string[processEntrys];

	string entry;
	int line = -1;
	while (inputFile.good()) {
		line++;
		// Lese n�chste Zeile
		std::getline(inputFile, entry, '\n');
		// �berspringe erste Zeile (Spalten�berschriften)
		if (line <= 0)
			continue;
		// �berspringe nicht relevante Zeilen
		if (line < processEntrys*(rank)+1)
			continue;
		// Stoppe, wenn alle relevanten Zeilen bearbeitet
		if (line > processEntrys*(rank + 1))
			break;
		// Speicher Zeile in Array
		string entryValues[4];
		size_t pos = 0;
		int index = 0;
		std::string token;
		while ((pos = entry.find(",")) != std::string::npos) {
			entryValues[index] = entry.substr(0, pos);
			entry.erase(0, pos + 1);
			index++;
		}
		// Speicher Eintrag in Zwischenspeicher
		string data = entryValues[1].substr(1, (entryValues[1]).size() - 2) + " [" + entryValues[0] + ": ~" + std::to_string(std::stod(entryValues[2]) * 100) + "%]";

		inputData[(line - 1) % processEntrys] = data;
	}

	// Warte, bis alle Prozesse vorbereitet sind und ihren Teil in den Zwischenspeicher geladen haben.
	MPI_Barrier(MPI_COMM_WORLD);

	// ================================================================
	//                            Tests
	// ================================================================
	srand(time(NULL));
	int id;
	string value;

	// Tests ggf. �berspringen.
	if (skipTests) {
		PrintOnce("Skipping tests.\n");
		goto userInput;
	}
	// ============= Gecachte Werte Einf�gen ================
	PrintOnce("Inserting cached data (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	// Trage alle Eintr�ge aus Zwischenspeicher ein:
	for (int i = 0; i < processEntrys; i++) {
		mpiHash->InsertDistEntry(rank*processEntrys + i, inputData[i]);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(0);
	PrintOnce("Cached data inserted in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	MPI_Barrier(MPI_COMM_WORLD);
	// ============= Gecachte Werte erneut Einf�gen ================
	PrintOnce("Rewriting cached data (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	// Trage alle Eintr�ge aus Zwischenspeicher ein:
	for (int i = 0; i < processEntrys; i++) {
		mpiHash->InsertDistEntry(rank*processEntrys + i, inputData[i]);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(1);
	PrintOnce("Cached data rewritten in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	MPI_Barrier(MPI_COMM_WORLD);
	// ============= Zuf�llige vorhandene Eintr�ge abfragen ================
	PrintOnce("Get random existing entrys (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < processEntrys; i++) {
		id = RandBetween(0, (processEntrys*numProcesses) - 1);
		mpiHash->GetDistEntry(id);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(2);
	PrintOnce("Random existing entrys got in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	// ============= Zuf�llige nicht vorhandene Eintr�ge abfragen ================
	PrintOnce("Get random not existing entrys (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < processEntrys; i++) {
		id = RandBetween((processEntrys*numProcesses), (processEntrys*numProcesses)+numProcesses);
		mpiHash->GetDistEntry(id);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(3);
	PrintOnce("Random not existing entrys got in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	// ============= Zuf�llige vorhandene Eintr�ge l�schen ================
	PrintOnce("Delete all existing entrys (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < processEntrys; i++) {
		mpiHash->DeleteDistEntry(rank*processEntrys + i);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(4);
	PrintOnce("All existing entrys deleted in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	// ============= Zuf�llige nicht vorhandene Eintr�ge l�schen ================
	PrintOnce("\"Delete\" not existing entrys (%d actions).\n", processEntrys*numProcesses);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < processEntrys; i++) {
		mpiHash->DeleteDistEntry(rank*processEntrys + i);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(5);
	PrintOnce("Not existing entrys \"deleted\" in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	// ============= Gecachte Werte f�r sp�tere Tests erneut einf�gen ================
	PrintOnce("Reinserting cached data...\n");
	for (int i = 0; i < processEntrys; i++) {
		mpiHash->InsertDistEntry(rank*processEntrys + i, inputData[i]);
	}
	// ============= Zuf�llige Aktionen auf gecachten Werte ausf�hren  ================
	PrintOnce("Process random actions on inserted data (%d actions -- Insert: %d%%   Get: %d%%   Delete: %d%%).\n", randomTests, RANDOM_INS_PERCENT, RANDOM_GET_PERCENT, RANDOM_DEL_PERCENT);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < (randomTests / numProcesses); i++) {
		int action = RandBetween(1, 100);
		id = RandBetween(0, (processEntrys*numProcesses) - 1);
		if (action <= RANDOM_GET_PERCENT) {
			// Frage zuf�lligen Eintrag ab.
			//printf("Getting Entry %d for Process %d.\n", id, rank);
			mpiHash->GetDistEntry(id);
		} else if (action <= RANDOM_DEL_PERCENT + RANDOM_GET_PERCENT) {
			// L�sche zuf�lligen Eintrag.
			//printf("Deleting Entry %d for Process %d.\n", id, rank);
			mpiHash->DeleteDistEntry(id);
		} else {
			// F�ge zuf�lligen Eintrag ein.
			value = RandString(RandBetween(RANDOM_MIN_STRING_LENGTH, RANDOM_MAX_STRING_LENGTH));
			//printf("Inserting Entry %d,%s for Process %d.\n", id, value.c_str(), rank);
			mpiHash->InsertDistEntry(id, value);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(6);
	PrintOnce("Random actions on inserted data processed in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	MPI_Barrier(MPI_COMM_WORLD);
	// ============= Zuf�llige Eintr�ge ================
	PrintOnce("Process random data (%d actions -- Insert: %d%%   Get: %d%%   Delete: %d%%).\n", randomTests, RANDOM_INS_PERCENT, RANDOM_GET_PERCENT, RANDOM_DEL_PERCENT);
	MeasurementStart();
	MPI_Barrier(MPI_COMM_WORLD);

	for (int i = 0; i < (randomTests / numProcesses); i++) {
		int action = RandBetween(1, 100);
		id = RandBetween(0, RAND_MAX / 2);
		if (action <= RANDOM_GET_PERCENT) {
			// Frage zuf�lligen Eintrag ab.
			//printf("Getting Entry %d for Process %d.\n", id, rank);
			mpiHash->GetDistEntry(id);
		} else if (action <= RANDOM_DEL_PERCENT + RANDOM_GET_PERCENT) {
			// L�sche zuf�lligen Eintrag.
			//printf("Deleting Entry %d for Process %d.\n", id, rank);
			mpiHash->DeleteDistEntry(id);
		} else {
			// F�ge zuf�lligen Eintrag ein.
			value = RandString(RandBetween(RANDOM_MIN_STRING_LENGTH, RANDOM_MAX_STRING_LENGTH));
			//printf("Inserting Entry %d,%s for Process %d.\n", id, value.c_str(), rank);
			mpiHash->InsertDistEntry(id, value);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MeasurementEnd(7);
	PrintOnce("Random data processed in   \x1b[97m%f\x1b[0m   seconds.\n", totalTime);
	MPI_Barrier(MPI_COMM_WORLD);
	PrintOnce(
		"----------------------------------------\n"
		"Insert new:                              %fs\n"
		"Reinsert:                                %fs\n"
		"Get existing:                            %fs\n"
		"Get not existing:                        %fs\n"
		"Delete existing:                         %fs\n"
		"Delete not existing:                     %fs\n"
		"Random actions on mostly existing:       %fs\n"
		"Random actions on mostly not existing:   %fs\n"
		"----------------------------------------\n",
		testResults[0],
		testResults[1],
		testResults[2],
		testResults[3],
		testResults[4],
		testResults[5],
		testResults[6],
		testResults[7]
	);
	// ================================================================
	//                      Benutzereingabe
	// ================================================================
userInput:
	// Ab jetzt (auf jeden Fall) detaillierte R�ckmeldung.
	verbose = true;
	// Nur Prozess 0 hat Nutzereingabe f�r weiter Bedienung.
	if (rank == 0) {
		usleep(200);
		printf("==================================\n");
		string helpText = "Commands: 'get ID', 'ins ID string', 'del ID', 'h(elp), 'q(uit)''\n";
		printf(helpText.c_str());
		for (std::string line; printf("   Command: "), std::getline(std::cin, line); usleep(200)) {
			if (line == "quit" || line == "q") {
				// Sende Beenden-Signal an alle Prozesse.
				for (int i = rank; i < numProcesses - 1; i++) {
					MPI_Ssend(MPI_BOTTOM, 0, MPI_INT, i + 1, TAG_EXIT, MPI_COMM_WORLD);
				}
				break;
			} else if (line == "help" || line == "h") {
				// Gebe Hilfstext (erneut) aus.
				printf(helpText.c_str());
			} else {
				// Pr�fe auf Aktion.
				int action;
				int id;
				size_t pos = 0;
				if ((pos = line.find(" ")) != std::string::npos) {
					string actionString = line.substr(0, pos);
					// Aktion herausfinden.
					if (actionString == "ins") {
						action = ACTION_INS;
					} else if (actionString == "get") {
						action = ACTION_GET;
					} else if (actionString == "del") {
						action = ACTION_DEL;
					} else {
						printf("Unkown command '%s'.\n", actionString.c_str());
						continue;
					}
					line.erase(0, pos + 1);
					// ID herausfinden.
					if ((pos = line.find(" ")) != std::string::npos) {
						id = std::stoi(line.substr(0, pos));
						if (action == ACTION_INS) {
							// Insert mit ID und Rest der Eingabe ausf�hren.
							line.erase(0, pos + 1);
							mpiHash->InsertDistEntry(id, line);
						} else {
							printf("Command '%s' only has one argument.\n", actionString.c_str());
						}
					} else {
						id = std::stoi(line);
						if (action == ACTION_GET) {
							// Get mit ID ausf�hren.
							mpiHash->GetDistEntry(id);
						} else if (action == ACTION_DEL) {
							// Delete mit ID ausf�hren.
							mpiHash->DeleteDistEntry(id);
						} else {
							printf("Too few arguments for command '%s'.\n", actionString.c_str());
						}
					}
				} else {
					printf("'%s' is no command.\n", line.c_str());
				}
			}
		}
	} else {
		// Alle anderen Prozesse warten auf das Beenden.
		int buf;
		MPI_Recv(&buf, 1, MPI_INT, 0, TAG_EXIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	return ExitApp();
}