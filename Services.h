#pragma once
#include <iostream>
#include "Record.h"
#include <chrono> //for the generator 
#include "Diskfile.h"
class RecordAdd
{
	//This class is for adding new records
private:
	static int addRecordMenu(); //show additional menu
	static void userAddRecord(); //manual add 
	static void generateRecords(); //random add
	static Diskfile * savefile; //target file
public:
	static void start(Diskfile * savefile); // this is entry point to the adding records class
};

class Showfile
{
public:
	//Prints showfile to the console
	static void showFile(Diskfile * showfile, bool sequence, unsigned int preoffset = 0,bool seccount = false);
};


class LoadFile
{
	//Loads user chosen file as a datafile
public: 
	static void loadFile(Diskfile * datafile);
};