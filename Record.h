#pragma once
#include <cstdint>
#include <random> 
#include <iostream>
#include "Exception.h"
#include "NumbersMap.h"
#define NUM_COUNT 5
typedef uint32_t recordhash;

class Time;


class Record
{
private:
	recordhash record; //hash representation of the record (matches to map with record values)
	

public:
	static std::unordered_map<uint32_t, std::array<float, 6>> nmap; //map to store record values
	static const unsigned int RECORD_BYTE_SIZE = sizeof(recordhash); //Size in Bytes of the hash record
	static const unsigned int RECORD_MAX_INT   = ~ ((recordhash) 0); //Max hash value possible (currently up to  4,294,967,295 records)
	static const unsigned int RECORD_MAX       = ~((recordhash)0); // Set maximum hash value to be used in map, TO DO 

	
	void printRecordString(); //prints record in format "1. num1, 2. num2, ... and g. mean"
	Record(float first, float second, float third, float fourth, float fifth); // creates record from user input
	Record(unsigned int record); // creates record from file
	Record(std::mt19937 * rng); // creates random record
	~Record();
	recordhash getRecord(); // returns (int) value of record (amount of seconds)
	static bool compare(Record r1, Record r2); // compares r1 with r2 if r1<=r2 then true, else false (<= in terms of g. mean)
	float find_mean(); //return geometric mean of specified record
	recordhash add_numbers(float first, float second, float third, float fourth, float fifth); //generate new record hash and add numbers to map
	float calc_mean(float arr[], int n); //calculate geometric mean
	static void clear_map(); //all the records in map are dropped, leaving it with size of 0
};

