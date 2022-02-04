/**
 * @file abstractor.cpp
 * @author Atilla Türkmen
 *
 * @brief Finds similarity between input and files in seperate threads
 *
 * All file names are stored as a queue globally.
 * Required number of threads are created.
 * Each thread takes a letter with global threadNo integer.
 * Each thread takes a file name from queue until it is empty.
 * Threads prints their unique letter and file name to output file.
 * After taking the file name, threads generate similarity point and summary.
 * Then push it to global results vector.
 * All threads join to main thread after all files are processed.
 * Main thread writes best results to output file.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

// Result of processed abstracts
struct result{
	string file;
	double score;
	string summary;
};
// Results are compared with scores, used for finding best results
bool compareResults(result r1, result r2) {
	return (r1.score > r2.score);
}

unordered_set<string> query; 					   // words that are searched in abstracts
queue<string> abstracts;	 					   // file names of abstracts
vector<result> results;		 					   // results of processed abstracts
int threadNo = 0;			 					   // used for naming thread with letters
FILE* out;										   // threads print logs to this file
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex lock between threads to prevent data races

void fillQuery(ifstream &in);	  // Populates query set by reading from given inputstream
void fillAbstracts(ifstream &in); // Reads file names and puts them in abstracts queue
void* runner(void* arg);		  // Runs text processing algorithm, used in threads

int main(int argc, char* argv[]) {
	// halt if input file and output file is not given
	if (argc < 3) {
		cout << "usage: ./abstractor.out input_file_name.txt output_file_name.txt" << endl;
		return -1;
	}
	// read name of input and output files
	string infile_name = argv[1];
    string outfile_name = argv[2];

    // open input stream
    ifstream in;
    in.open(infile_name);

    // read thread count, abstract count and number of abstracts that will be returned and summarized
    int t, a, n;
    in >> t >> a >> n;

    // fill query and abstracts shared datas
    fillQuery(in);
    fillAbstracts(in);

    // close input stream
    in.close();
    // open output stream
    out = fopen(outfile_name.c_str(), "w");

    // store thread ID's
    vector<pthread_t> tid(t);

    // create threads that process abstracts
    for (int i = 0; i < t; i++) {
		if (pthread_create(&tid[i], NULL, &runner, NULL) != 0) {
			printf("An error occured while creating the thread\n");
		}
    }

    // wait for threads to finish
    for (int i = 0; i < t; i++) {
		if (pthread_join(tid[i], NULL) != 0) {
			printf("An error occured while joining the threads\n");
		}
    }

    // sort results to find maximum scores
    sort(results.begin(), results.end(), compareResults);

    // print best n results
    fprintf(out, "###\n");
    for (int i = 0; i < n; i++) {
    	fprintf(out, "Result %d:\n", i + 1);
    	fprintf(out, "File: %s\n", results[i].file.c_str());
    	fprintf(out, "Score: %.4f\n", results[i].score);
    	fprintf(out, "Summary: %s\n", results[i].summary.c_str());
    	fprintf(out, "###\n");
    }
    fclose(out);
	return 0;
}

// Runs text processing algorithm, used in threads
void* runner(void* arg) {
	// START OF CRITICAL ZONE
	pthread_mutex_lock(&mutex); // lock mutex before critical zone
	char name = 'A' + threadNo; // initialize thread character
	threadNo++;					// increment threadNo for thread after this
	while (!abstracts.empty()) {
		// pop abstract files if all of them is not processed
		string fileName = abstracts.front();
		abstracts.pop();
		// print thread id and abstract that is processed
		fprintf(out, "Thread %c is calculating %s\n", name, &fileName[0]);
		pthread_mutex_unlock(&mutex); // unlock mutex to let other threads run
		// END OF CRITICAL ZONE
		// heavy text processing
		// open input stream
	    ifstream in;
	    in.open("../abstracts/" + fileName);
	    // local variables for text processing
	    unordered_set<string> words;  // unique words in this abstract
	    long long intersect = 0;	  // number of words both in query and abstract
	    long long uni = query.size(); // size of union between query and abstract
	    string sentence = "";		  // current sentence while iterating, appended to summary if includes a word from query
	    string summary = "";		  // sentences that have words from query
	    bool inSummary = false;		  // true if current sentence includes a word from query
	    string word;				  // current word while iterating abstract
	    // iterate over abstract word by word
	    while (in >> word) {
	    	// add word to current sentence
	    	if (sentence != "")
	    		sentence += " ";
	    	sentence += word;
	    	// sentence is finished
	    	if (word == ".") {
	    		if (inSummary)
	    			summary += sentence + " ";
	    		inSummary = false;
	    		sentence = "";
	    		// continue;
	    		// example output in the project description takes '.' as a valid word
	    		// uncomment continue above to ignore '.'
	    	}
	    	// this sentence will be on summary if it includes a word from query
	    	if (query.find(word) != query.end())
	    		inSummary = true;
	    	// skip duplicate words when counting
	    	if (words.find(word) != words.end()) continue;
	    	words.insert(word);
	    	// count intersect and union size
	    	if (query.find(word) != query.end()) {
		    	intersect++;
		    } else {
		   		uni++;
		   	}
	    }
	    // calculate result and put it in global data
	    double score = (double)intersect / uni;
		result r = {
			.file = fileName,
			.score = score,
			.summary = summary
		};
		// START OF CRITICAL ZONE
		pthread_mutex_lock(&mutex);
		results.push_back(r);
	}
	pthread_mutex_unlock(&mutex);
	// END OF CRITICAL ZONE
	return NULL;
}

// Populates query set by reading from given inputstream
void fillQuery(ifstream &in) {
	string line;
    getline(in, line); // skip last empty line after three integers
    getline(in, line);
    stringstream s(line);
    string word;
    while (s >> word)
    	query.insert(word);
}

// Reads file names and puts them in abstracts queue
void fillAbstracts(ifstream &in) {
	string abstract;
	while (in >> abstract)
		abstracts.push(abstract);
}
