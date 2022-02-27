#include <iostream>
#include "Exception.h"
#include "Diskfile.h"
#include "Services.h"
#include "PolyphaseSort.h"

std::string DATA_FILE_PATH = "Tapes/data.txt";
const std::string TAPE_NO_1_FILE_PATH = "Tapes/tapeNo1.txt";
const std::string TAPE_NO_2_FILE_PATH = "Tapes/tapeNo2.txt";
const std::string TAPE_NO_3_FILE_PATH = "Tapes/tapeNo3.txt";


int menu(bool DEBUG_MODE)
{
	int key;
	system("cls");
	std::cin.seekg(0, std::ios::end);
	std::cin.clear();
	std::cout << "     Main Menu		" << std::endl;
	std::cout << "1.      Sort			" << std::endl;
	std::cout << "2.    Show File    	" << std::endl;
	std::cout << "3.   Add Records		" << std::endl;
	if (DEBUG_MODE)
	{
		std::cout << "4. Leave debug mode" << std::endl;
	}
	else
	{
		std::cout << "4. Enter debug mode" << std::endl;
	}
	std::cout << "5.    Load File    	" << std::endl;
	std::cout << "6.   Clear File	    " << std::endl;
	std::cout << "7.      Exit	    	" << std::endl;
	std::cout << ">> ";
	std::cin >> key;
	return key;
	
}


int main()
{
	//std::fstream file;
	Diskfile * datafile = new Diskfile(DATA_FILE_PATH); // file which we want to sort
	datafile->clearFile(true);
	bool DEBUG_MODE = false; //for testing (shows sorting phase by phase)
	bool keep = true;
	bool first_run = true;

	do
	{
		int key = menu(DEBUG_MODE);
		switch (key)
		{
		case 1:
		{
			PolyphaseSort * sort = new PolyphaseSort(datafile, TAPE_NO_1_FILE_PATH, TAPE_NO_2_FILE_PATH, TAPE_NO_3_FILE_PATH, DEBUG_MODE);
			sort->sort();
			keep = true;
			delete sort;
			int x = 0;
			break;
		}
		case 2:
			//Show File
			if (first_run) {
				
				first_run = false;
			}
			Showfile::showFile(datafile,false);
			keep = true;
			break;
		case 3:
		{
			//Add Record
			RecordAdd::start(datafile);
			keep = true;
			break;
		}
		case 4:
			//Change DEBUG state
			DEBUG_MODE = !DEBUG_MODE;
			keep = true;
			break;
		case 5:
			// load file:
			system("cls");
			
			LoadFile::loadFile(datafile);
			keep = true;
			break;
		case 6:
		{
			datafile->clearFile(true);
			Record r(10);
			r.clear_map();
			break;
		}
		case 7:
		{
			keep = false;
			break;
		}
		default:
			keep = true;
			break;
		}
	}while (keep);


	delete datafile;
	return 0;
}

