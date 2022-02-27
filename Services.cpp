#include "Services.h"
#include <random>
#include <iomanip>
#include <tuple>
#include <sstream>
#include <string>

Diskfile * RecordAdd::savefile = nullptr;

void RecordAdd::start(Diskfile * savefile)
{
	//This function organises adding record:
	RecordAdd::savefile = savefile;
	bool keep = true;
	do
	{
		switch (addRecordMenu())
		{
		case 1:
			//manally add record
			userAddRecord();
			keep = true;
			break;
		case 2:
			//add random records
			generateRecords();
			keep = true;
			break;
		case 3:
			//go back
			try { savefile->saveBuf(); } // To save records to file (needed if user doesn't want to sort or show file)
			catch (int e) { std::cout << "Error occured during saving records to file" << std::endl; };
			keep = false;
			break;
		default:
			keep = true;
			break;
		}
	} while (keep);
}

int RecordAdd::addRecordMenu()
{
	//This is menu for the adding records class
	using namespace std;
	int key;
	system("cls");
	cin.seekg(0, ios::end);
	cin.clear();
	cout << "     New Record Menu" << endl;
	cout << "1.   Manual" << endl;
	cout << "2.   Random" << endl;
	cout << "3.   Return" << endl;
	cout << ">> ";
	cin >> key;
	return key;
}

void RecordAdd::userAddRecord()
{
	//User can add one record 
	//Entering five floats
	float numbers[5];
	using namespace std;
	system("cls");
	cout << "Creating new record in format 5 Unsigned Numbers " << endl;
	for (int i = 0; i < 5; i++) {
		cout << "Insert " << (i + 1) << " number	: ";
		cin >> numbers[i];
		cout << std::endl;
	}

	RecordAdd::savefile->saveRecord(Record(numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]), false);
	
	cout << "Record Saved Successfully!" << endl;
	cout << "Press enter to continue" << endl;
	std::cin;
}

void RecordAdd::generateRecords()
{
	int N;
	using namespace std;
	//creates generator:
	mt19937 * rng = new mt19937((unsigned int)chrono::steady_clock::now().time_since_epoch().count());
	system("cls");
	cout << "Number of Records to generate:" << endl;
	cout << ">> N = ";
	cin >> N;
	cout << endl;

	for (int n = 0; n < N; n++)
	{
		//here I: save record(generate one record()) 
		RecordAdd::savefile->saveRecord(Record(rng),false);
	}

	cout << N << " Records Saved Successfully!" << endl;
	cout << "Press enter to continue" << endl;
	std::cin;
}

void Showfile::showFile(Diskfile * showfilearg, bool sequence, unsigned int preoffset, bool seccount)
{
	Diskfile * showfile = showfilearg;
	bool eof = false;
	Record *showrecord = nullptr;
	Record *lastrecord = nullptr;
	unsigned int runs = 0;
	if (!sequence)
	{
			system("cls");
	}
	std::cout << "File \"" << showfile->getFilePath() <<"\":" << std::endl;
	for (int i = 0; showfile->checkeof() == false; i++)
	{
		
		try
		{
			showrecord = new Record(showfile->getRecord(preoffset,false, seccount));
		}
		catch (int exeption)
		{
			system("cls"); // IF you want errors to be shown comment this line
			std::cout << "File is empty or does not exist" << std::endl;
			std::cout << "To exit press any letter and Enter" << std::endl;
			char c;
			std::cin >> c;
			system("cls");
			return;
		}
		if (i != 0 && Record::compare(*lastrecord, *showrecord) == false)
		{
			//new run:
			runs++;
			std::cout << std::endl;
		}
		showrecord->printRecordString();
		if(lastrecord != nullptr)
			delete lastrecord;
		lastrecord = showrecord;
		showrecord = nullptr; //
		
	}
	runs++;
	if(lastrecord != nullptr) delete lastrecord;
	std::cout << std::endl << "End of file reached, Runs: "<< runs << std::endl;
	if (!sequence)
	{
		std::cin.seekg(0, std::ios::end);
		std::cin.clear(); //clear cin's input buffer
		std::cout << "To continue press any letter and Enter" << std::endl << std::endl;
		char c;
		std::cin >> c;
		system("cls");
	}
	else
	{
		std::cout << std::endl;
	}
}

void LoadFile::loadFile(Diskfile * datafile)
{
	std::string loadpath;
	system("cls");
	std::cout << "You can load your own data (presented in \AA,a BB,b CC,c DD,d EE,e\" format)" << std::endl;
	std::cout << "Please enter filepath of your data (relative to this project)" << std::endl;
	std::cin >> loadpath;
	datafile->clearFile(true);
	Record r(10);
	r.clear_map();
	std::fstream loadfile;
	loadfile.open(loadpath, std::ios::in);
	if (!loadfile.is_open())
	{
		loadfile.close();
		std::cout << "No such file as " << loadpath << " found - try again" << std::endl;
		std::cout << "To continue press any letter and Enter" << std::endl << std::endl;
		std::cin.seekg(0, std::ios::end);
		std::cin.clear(); //clear cin's input buffer
		char c;
		std::cin >> c;
		system("cls");
		return;
	}
	std::string loadrecord;
	for (std::string line; std::getline(loadfile, line); )   //read stream line by line
	{
		std::istringstream in(line);      //make a stream for the line itself

		//std::string type;
		//in >> type;                  //and read the first whitespace-separated token


		float numbers[NUM_COUNT];
		for (int i = 0; i < NUM_COUNT; i++)
			in >> numbers[i];       //now read the whitespace-separated floats
		datafile->saveRecord(Record(numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]), false);
	}
	
	std::cout << "Loaded file successfully" << std::endl;
	std::cout << "To continue press any letter and Enter" << std::endl << std::endl;
	char c;
	std::cin >> c;
	system("cls");
	loadfile.close();

}