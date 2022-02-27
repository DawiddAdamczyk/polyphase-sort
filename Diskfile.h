#pragma once
#pragma warning disable 4101 //don't show unreferenced local variable warning (variable : int exeption)
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>  
#include "Record.h"
#include "Exception.h"

/*	Diskfile class 
	ment to provide: file loading, record read/write, buffers
	memory needed for one hash of record :  32 bits (4 byte integer)
*/


enum state { read, write, initial };

class Diskfile
{
private:
	//file stuff:
	std::string filepath;  
	std::fstream file;
	unsigned int filereadoffset;
	unsigned int filereadtimes;
	unsigned int filewritetimes;
	//buffer stuff:
	static const int DISK_PAGE_SIZE = 4096;  // This is maximal size of an buffer in Bytes 
	static const int BUFFER_SIZE = DISK_PAGE_SIZE / Record::RECORD_BYTE_SIZE; // size of disk buffer
	recordhash* diskbuffer; //This is input/output buffer used whenever  read/write to a file is performed
	unsigned int bufoffset; // This is offset of the next will be record in the buffer (initial 0) (one record 1)
	unsigned int bufreadoffset; // This variable points on the next record to be read (initial 0)
	state bufferstate; //Determines if we are writing to buffer or reading from buffer 
	bool endoffileflag; // important to know if we reached eof
	//File <-> Buffer transfer
	void writeFile(bool count ); //writes from buffer to file
	void readFile(unsigned int preoffset , bool count ); //reads from file to buffer
public:
	bool checkeof(); // checks if eof flag was hit in the file
	unsigned int getDiskInOperations() { return this->filewritetimes; }; // Return how many times file has been written into
	unsigned int getDiskOutOperations() { return this->filereadtimes; }; // Return how many times file has been read
	//object management:
	Diskfile(std::string newfilepath);
	~Diskfile();
	
	// Buffer <-> Program transfer
	recordhash getRecord(unsigned int preoffset , bool count, bool seccount = false); // reads from buffer to file
	void saveRecord(Record &record, bool count ); // save record to the buffer
	void removeFile(); // Removes file from the disk
	void clearFile(bool resetCount); // Removes all data form the file
	void reset(); // Set all flags of the read file to the start of the file
	std::string getFilePath() {return this->filepath; };
	void replaceFile(Diskfile * newFile, bool debugmode);
	void saveBuf(); // Saves rest of the buffer into file;


	
};
