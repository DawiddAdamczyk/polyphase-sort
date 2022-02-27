#include "Diskfile.h"

Diskfile::Diskfile(std::string newfilepath)
{
	this->endoffileflag = false;
	this->bufferstate = initial;
	this->diskbuffer = new recordhash[this->BUFFER_SIZE]; // create buffer for the new Diskfile object
	this->filepath = newfilepath;	
}

Diskfile::~Diskfile()
{
	if(this->diskbuffer != nullptr) delete this->diskbuffer;
}

void Diskfile::writeFile(bool count)
{
	//opening file:
	this->file.open(this->filepath,  std::ios::app | std::ios::binary );
	if (!this->file.is_open())
	{
		std::cout << "error - while opening file in Diskfile.cpp at:" << __LINE__ << std::endl;
		this->file.close();
		throw OPEN_FILE_EXEPTION;
	}
	this->file.seekp(0, std::ios::end); // append p pointer to the end of the file

	//save whole buffer into file:
	this->file.write((char *)this->diskbuffer, this->bufoffset * Record::RECORD_BYTE_SIZE);
	this->bufoffset = 0;
	this->endoffileflag = false; //file cannot be empty (just added new records)
	if(count) this->filewritetimes++;
	this->file.close();
}

void Diskfile::readFile(unsigned int preoffset, bool count)
{
	//read file into buffer
	this->file.open(this->filepath, std::ios::in | std::ios::binary);
	if (!this->file.is_open())
	{
		std::cout << "error - while opening file in Diskfile.cpp at:" << __LINE__ << std::endl;
		this->file.close();
		throw OPEN_FILE_EXEPTION;
	}

	//check size of the file :
	this->file.seekg(0, std::ios::end);
	int size = (int) this->file.tellg();
	if (size == 0 || size - this->filereadoffset - preoffset * Record::RECORD_BYTE_SIZE <= 0)
	{
		std::cout << "error - cannot read, file is empty in Diskfile.cpp at:" << __LINE__ << std::endl;
		this->file.close();
		throw EMPTY_FILE_EXEPTION;
	}

	//mind the already read part && check if rest is smaller tha DISK_PAGE_SIZE
	unsigned int readsize = 0;
	if (size - this->filereadoffset - preoffset * Record::RECORD_BYTE_SIZE< this->DISK_PAGE_SIZE) 
		readsize = size - this->filereadoffset  - preoffset * Record::RECORD_BYTE_SIZE;
	else 
		readsize = this->DISK_PAGE_SIZE;

	//read:
	this->file.seekg(this->filereadoffset + preoffset * Record::RECORD_BYTE_SIZE, std::ios::beg); // move g pointer to the last read record 
	try
	{
		this->file.read((char *)this->diskbuffer, readsize ); //save to buffer
	}
	catch (int exeption)
	{
		throw exeption;
	}
	this->filereadoffset += readsize; // increase amount of already read records
	this->bufoffset = readsize / Record::RECORD_BYTE_SIZE; //TODO
	this->bufreadoffset = 0;

	//check if this is the end of the file
	if (size - this->filereadoffset - preoffset * Record::RECORD_BYTE_SIZE <= 0) //TODO
	{
		//expected at somepoint
		this->endoffileflag = true; 
	}

	//closure:
	if(count) this->filereadtimes++;
	try
	{
		this->file.close();
	}
	catch (int er)
	{
		throw er;
	}
}

recordhash Diskfile::getRecord(unsigned int preoffset, bool count, bool seccount)
{
	recordhash record;
	if (this->bufferstate == initial)
	{
		this->bufoffset = 0;
		this->bufreadoffset = 0;
		this->bufferstate = read;
	}
	//check if present buffer is useful:
	if (this->bufferstate == write) //TODO
	{
		//present buffer is not useful 
		//we need to save buffer and reload:
		//saving:
		try
		{
			this->writeFile(seccount);
		}
		catch (int exeption)
		{
			std::cout << "error - while writing to the file [" << exeption << "] in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw WRITING_FILE_EXEPTION;
		}
		//reloading:
		this->bufferstate = read;
		if (this->endoffileflag) // check if there is some of the file left
		{
			//Error no more file to read from
			std::cout << "error - cannot read, file is empty in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw END_OF_FILE_EXEPTION;
		}
		//reading from the file:
		try { this->readFile(preoffset, count); }
		catch (int exeption)
		{
			std::cout << "error - while reading the file in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw READING_FILE_EXEPTION;
		}
	}

	//checking if there is need to read from the file:
	if (this->bufoffset  == 0 || this->bufoffset - this->bufreadoffset <= 0 ) //bufer is empty
	{
		if (this->endoffileflag) // check if there is some of the file left
		{
			//Error no more file to read from
			std::cout << "error - cannot read, file is empty in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw END_OF_FILE_EXEPTION;
		}
		try { this->readFile(preoffset, count); } // read buffer from file 
		catch(int exeption)
		{
			std::cout << "error - while reading the file in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw READING_FILE_EXEPTION;
		}
		if (this->bufoffset == 0) //nothing has been read - file must be empty   
		{
			std::cout << "error - reading file is empty in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw EMPTY_FILE_EXEPTION;
		}
	}

	//buffor is not empty so we can read from it:
	record = this->diskbuffer[this->bufreadoffset];
	this->bufreadoffset++;

	//checking if we now read all records in the buffer:
	if (this->bufoffset - this->bufreadoffset <= 0) //bufer is empty
	{
		//reset buffer:
		this->bufferstate = initial;
		this->bufreadoffset = 0;
		this->bufoffset = 0;
	}

	return record;

}

void Diskfile::saveRecord(Record &record, bool count)
{
	/* 
		Summary:
		This function takes a record from program 
	    and saves it in the buffer, if buffer is full
		then buffer is saved in the file specified in this->filepath
		Arguments: 
			int record - THE record we are trying to save
		Returns : false if there was any error; true otherwise
	*/
	if (this->bufferstate == initial || this->bufferstate == read )
	{
		//data in buffer is useless
		this->bufoffset = 0;
		this->bufreadoffset = 0;
		this->bufferstate = write;
	}


	//check if buffer is full prior to saving record (security)
	if (this->bufoffset >= this->BUFFER_SIZE )
	{
		//need to save full buffer to file
		try 
		{ 
			this->writeFile(count);
		}
		catch (int exeption)
		{
			std::cout << "error - while writing to the file [" << exeption << "] in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw WRITING_FILE_EXEPTION;
		}
	}

	//record is saved into the buffer
	this->diskbuffer[this->bufoffset] = record.getRecord();
	this->bufoffset++;

	//check if buffer is full after saving record 
	if (this->bufoffset >= this->BUFFER_SIZE)
	{
		//when buffer is full it is being saved to the file
		try 
		{ 
			this->writeFile(count);
			this->bufferstate = initial;
		}
		catch (int exeption)
		{
			std::cout << "error - while writing to the file [" << exeption << "] in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw WRITING_FILE_EXEPTION;
		}
	}
}

bool Diskfile::checkeof()
{
	bool answer = false;
	if (this->endoffileflag && this->bufoffset - this->bufreadoffset <= 0)
	{
		answer = this->endoffileflag;
		// reset
		this->endoffileflag = false; 
		this->filereadoffset = 0;
	}
	return answer;
}

void Diskfile::reset()
{
	if (this->bufferstate == read || this->bufferstate == initial)
	{
		this->bufferstate = initial;
		this->endoffileflag = false;
		this->filereadoffset = 0;

	}
	else
	{
		std::cout << "Wrong buffer state to reset - no action taken" << std::endl;
	}
}


void Diskfile::removeFile()
{
	
	if (remove(this->filepath.c_str()) != 0)
	{
		throw ERROR_DELETING_FILE;
		std::cout<<"Error deleting "<<this->filepath<<"file"<<std::endl;
	}
}

void Diskfile::replaceFile(Diskfile * newFile, bool debugmode)
{

	if (debugmode == false, newFile->bufferstate == write)
	{
		//saving final records from buffer:
		try
		{
			newFile->writeFile(true);
			newFile->bufferstate = initial;
		}
		catch (int exeption)
		{
			std::cout << "error - while writing to the file [" << exeption << "] in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw WRITING_FILE_EXEPTION;
		}
	}
	
	// remove file
	if (remove(this->filepath.c_str()) != 0)
	{
		std::cout << "Error while replacing file - cannot remove sourcefile" << std::endl;
		throw ERROR_DELETING_FILE;
	}
	// rename file
	if (rename(newFile->filepath.c_str(), this->filepath.c_str()) != 0)
	{
		std::cout << "Error while replacing file - cannot rename sourcefile" << std::endl;
		throw ERROR_RENAMING_FILE;
	}

	//copy variables:
	this->bufferstate = newFile->bufferstate;
	this->endoffileflag = newFile->endoffileflag;
	this->bufoffset = newFile->bufoffset;
	this->bufreadoffset = newFile->bufreadoffset;
	this->filereadoffset = newFile->filereadoffset;
	this->filereadtimes = 0;
	this->filewritetimes = 0;

	//copy buffer
	if (this->diskbuffer != nullptr) delete this->diskbuffer;
	this->diskbuffer = newFile->diskbuffer;
	newFile->diskbuffer = nullptr;
}

void Diskfile::clearFile(bool resetCount)
{
	this->file.open(this->filepath, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!this->file.is_open())
	{
		std::cout << "error - while opening file in Diskfile.cpp at:" << __LINE__ << std::endl;
		this->file.close();
		throw OPEN_FILE_EXEPTION;
	}
	this->file.close();
	if (resetCount)
	{
		this->filereadtimes = 0;
		this->filewritetimes = 0;
	}
}

void Diskfile::saveBuf()
{
	if (this->bufferstate == write)
	{
		//saving final records from buffer:
		try
		{
			this->writeFile(false);
			this->bufferstate = initial;
		}
		catch (int exeption)
		{
			std::cout << "error - while writing to the file [" << exeption << "] in Diskfile.cpp at:" << __LINE__ << std::endl;
			throw WRITING_FILE_EXEPTION;
		}
	}
}
