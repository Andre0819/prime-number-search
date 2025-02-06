// AQUINO, Karl Andre F. STDISCM S12

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <ranges>

using namespace std;
using namespace chrono;

mutex printMutex;  
vector<pair<int, thread::id>> primeNumbers; 

// Config parameters
string printVariation, taskDivision;
int numThreads, searchLimit;

bool is_digits(string& str)
{
  for (char ch : str) {
    int v = ch; // ASCII Val converted
    if (!(ch >= 48 && ch <= 57)) {
      return false;
    }
  }
  return true;
}

string getCurrentTimestamp() {
  auto now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
  time_t tt = system_clock::to_time_t(now);
  tm localTime;
  localtime_s(&localTime, &tt);

  stringstream ss;
  ss << put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "." << setfill('0') << setw(3) << ms.count();
  return ss.str();
}

// Read the config file and parse values
void readConfigFile() {
  ifstream file("config.txt");
  string line;

  if (!file) {
    cerr << "Error: Could not open config.txt" << endl;
    exit(1);
  }

  while (getline(file, line)) {
    size_t pos = line.find('=');
    if (pos != string::npos) {
      string key = line.substr(0, pos);
      string value = line.substr(pos + 1);

      try {
        if (key == "PRINT_VARIATION") {
          if (value != "I" && value != "II") {
            throw invalid_argument("Invalid PRINT_VARIATION. Must be 'I' or 'II'.");
          }
          printVariation = value;
        }
        else if (key == "TASK_DIVISION") {
          if (value != "I" && value != "II") {
            throw invalid_argument("Invalid TASK_DIVISION. Must be 'I' or 'II'.");
          }
          taskDivision = value;
        }
        else if (key == "NUM_THREADS") {
          if (!is_digits(value)) { throw invalid_argument("NUM_THREADS must be a positive number."); }

          numThreads = stoi(value);
          if (numThreads <= 0) {
            throw out_of_range("NUM_THREADS must be greater than 0.");
          }
        }
        else if (key == "SEARCH_LIMIT") {
          if (!is_digits(value)) { throw invalid_argument("NUM_THREADS must be a positive number."); }

          searchLimit = stoi(value);
          if (searchLimit <= 0) {
            throw out_of_range("SEARCH_LIMIT must be greater than 0.");
          }
					else if (searchLimit > 2147483647) {
						throw out_of_range("SEARCH_LIMIT must be less than or equal to 2147483647.");
					}
				}
				else {
					throw invalid_argument("Invalid key in config file.");
				}
      }
      catch (const exception& e) {
        cerr << "Error in config file (" << key << "): " << e.what() << endl;
        exit(1);
      }
    }
  }
  file.close();

  if (numThreads > searchLimit) {
    cerr << "Error: NUM_THREADS must be less than or equal to SEARCH_LIMIT." << endl;
    exit(1);
  }

}

bool isPrimeNumber(int num) {
  if (num <= 1) return false;
  if (num == 2 || num == 3) return true;
  if (num % 2 == 0 || num % 3 == 0) return false;

  for (int i = 5; i * i <= num; i += 6)
    if (num % i == 0 || num % (i + 2) == 0)
      return false;

  return true;
}

void printPrime(int num) {
  thread::id tid = this_thread::get_id();
  
  if (printVariation == "I") {
    lock_guard<mutex> lock(printMutex);
    string timestamp = getCurrentTimestamp();
    cout << "Thread [" << tid << "]: Found prime " << num << " at " << timestamp << endl;
  }
  else {
    lock_guard<mutex> lock(printMutex);
    primeNumbers.emplace_back(num, tid);
  }
}


// Task division I: Each thread gets a range
void findPrimesRange(int start, int end) {
  for (int i = start; i <= end; i++) {
    if (isPrimeNumber(i)) {
      printPrime(i);
    }
  }
}

// Task division II: Each thread checks divisibility for a single number
void findPrimesDivisibility(int num) {
  if (isPrimeNumber(num)) {
    printPrime(num);
  }
}

void startPrimeSearch() {
  vector<thread> threads;

  auto startTime = high_resolution_clock::now();
  string startTimestamp = getCurrentTimestamp();
  cout << "Search started at: " << startTimestamp << endl;
  cout << endl;

  if (taskDivision == "I") {
    int chunkSize = searchLimit / numThreads;
    for (int i = 0; i < numThreads; i++) {
      int start = i * chunkSize + 1;
      int end = (i == numThreads - 1) ? searchLimit : (start + chunkSize - 1);
      threads.emplace_back(findPrimesRange, start, end);
    }
  }
  else if (taskDivision == "II") {
    for (int i = 1; i <= searchLimit; i++) {
      threads.emplace_back(findPrimesDivisibility, i);
      if (threads.size() >= numThreads) {
        for (auto& t : threads) t.join();
        threads.clear();
      }
    }
  }

  for (auto& t : threads) {
    t.join();
  }

  if (printVariation == "II") {
    lock_guard<mutex> lock(printMutex);
    cout << "All primes found: " << endl;
    for (const auto& pair : primeNumbers) {
      cout << "Thread [" << hash<thread::id>{}(pair.second) << "]: " << pair.first << endl;
    }
  }
  cout << endl;
  auto endTime = high_resolution_clock::now();
  string endTimestamp = getCurrentTimestamp();
  cout << "Search ended at: " << endTimestamp << endl;

  auto duration = duration_cast<milliseconds>(endTime - startTime);
  cout << "Total execution time: " << duration.count() << " ms" << endl;
}

int main() {
  readConfigFile();
  startPrimeSearch();
  return 0;
}
