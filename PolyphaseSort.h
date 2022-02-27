#pragma once
#include "Diskfile.h"
#include "Record.h"
#include "Services.h"
#include <iostream>
#include <limits>
class Fibbonacci;
enum tape;

class PolyphaseSort
{
	//Does whole process of sorting datafile
private:
	//Tapes for sorting:
	Diskfile * tape_no1; 
	Diskfile * tape_no2; 
	Diskfile * tape_no3; 
	//File with inital data:
	Diskfile * datafile;
	//Used to get the next number of fibbonaci cycle, needed to properly distribute runs 
	Fibbonacci * fiber;
	//Debug Mode adds overview after every step / phase of sorting 
	bool debug_mode;
	//Current number of pst sorting phases  
	unsigned int phases;
	//Number of "dummy" runs added after distribution and used in the frst phase of sorting 
	unsigned int dummyruns;
	//After each phase one of source tapes isn't empty for the another phase
	//preoffset enables us to read in the next phase those unused runs
	unsigned int preoffsetTape_no1;
	unsigned int preoffsetTape_no2;
	//Enum - where dummy runs are stored
	tape dummytape;
	//Methodes:
	bool distribute(); //divides records from datafile to 2 tapes A and B acordingly to the Fibonnaci cycle
	bool phaseSort(Diskfile * sourceTape_no1, Diskfile * sourceTape_no2, Diskfile * destTape_no3); //Combines records from 2 source tapes into one dest tape, sorting them
	void saveSortedFile(Diskfile * datafile, Diskfile * destTape_no3);//save dest tape with sorted records into datafile,
	void printFinalStats();// Prints all statistics
public:
	PolyphaseSort(Diskfile * datafile, std::string PathOfTape_no1, std::string PathOfTape_no2, std::string PathOfTape_no3, bool DEBUG_MODE);
	~PolyphaseSort();
	void sort(); // sorting menager
};

class Fibbonacci
{
private:
	unsigned int prev;
	unsigned int that;
	unsigned int next;
public:
	Fibbonacci() : prev{ 0 }, that{ 0 }, next{ 1 } {};
	~Fibbonacci() {};
	unsigned int nextFib(); //gets next number from fibbonaci cycle

	void reset()
	{
		prev = 0;
		that = 0;
		next = 1;
	}
};

enum tape {Tape_no1, Tape_no2, Tape_no3};
