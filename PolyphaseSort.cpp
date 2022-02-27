#include "PolyphaseSort.h"

PolyphaseSort::PolyphaseSort(Diskfile* datafile, std::string PathOfTape_no1, std::string PathOfTape_no2, std::string PathOfTape_no3, bool DEBUG_MODE)
{
	this->datafile = datafile;
	this->tape_no1 = new Diskfile(PathOfTape_no1);
	this->tape_no2 = new Diskfile(PathOfTape_no2);
	this->tape_no3 = new Diskfile(PathOfTape_no3);
	this->debug_mode = DEBUG_MODE;
	this->phases = 0;
	this->preoffsetTape_no1 = 0;
	this->preoffsetTape_no2 = 0;
	this->fiber = new Fibbonacci();
	this->dummyruns = 0;
	this->dummytape = Tape_no1;
	
}

PolyphaseSort::~PolyphaseSort()
{
	if (this->tape_no1 != nullptr)
	{
		this->tape_no1->removeFile();
		delete this->tape_no1;
		this->tape_no1 = nullptr;
	}
	if (this->tape_no2 != nullptr)
	{
		this->tape_no2->removeFile();
		delete this->tape_no2;
		this->tape_no2 = nullptr;
	}
	if (this->tape_no3 != nullptr)
	{
		this->tape_no3->removeFile();
		delete this->tape_no3;
		this->tape_no3 = nullptr;
	}
	if (this->fiber != nullptr)
	{
		delete this->fiber;
		this->fiber = nullptr;
	}
}


bool PolyphaseSort::distribute()
{
	//Variables:
	Record* newrecord = nullptr;
	Record* lastrecord = nullptr;
	Record* lastrecordA = nullptr;
	Record* lastrecordB = nullptr;
	Diskfile* savetape = this->tape_no1;
	unsigned int nextfib = this->fiber->nextFib();
	unsigned int runCount = 0;
	unsigned int runCountA = 0;
	unsigned int runCountB = 0;
	bool newRunStart = false;

	//Distribution:
	for (unsigned int i = 0; datafile->checkeof() == false; i++)
	{

		//get records
		try
		{
			if (newrecord == nullptr)
			{
				newrecord = new Record(this->datafile->getRecord(0,true,true));
			}
		}
		catch (int e) { throw e; }

		//check if end of run
		if (lastrecord != nullptr && newrecord != nullptr && Record::compare(*lastrecord, *newrecord) == false && newRunStart == false)
		{
			// l > n meaning => new run
			runCount++;
			newRunStart = true;
			if (runCount >= nextfib)
			{
				newRunStart = false;
				//time to change tape
				nextfib = this->fiber->nextFib();
				if (savetape == this->tape_no1)
				{
					savetape = this->tape_no2;
					runCountA = runCount;
					runCount = runCountB;
					//check if runs coalese:
					if (lastrecordB != nullptr && newrecord != nullptr && Record::compare(*lastrecordB, *newrecord) == true)
					{
						//they do
						if (lastrecordB != nullptr && lastrecordB != lastrecord) delete lastrecordB;
						lastrecordB = newrecord;
						runCount--;
					}
					if (lastrecordA != nullptr ) delete lastrecordA;
					lastrecordA = lastrecord;
				}
				else // tape Brutus
				{
					savetape = this->tape_no1;
					runCountB = runCount;
					runCount = runCountA;
					//check if runs coalese:
					if (lastrecordA != nullptr && newrecord != nullptr && Record::compare(*lastrecordA, *newrecord) == true)
					{
						//they do
						if (lastrecordA != nullptr && lastrecordA != lastrecord) delete lastrecordA;
						lastrecordA = newrecord;
						runCount--;
					}
					if (lastrecordB != nullptr) delete lastrecordB;
					lastrecordB = lastrecord;
				}
			}

		}

		//save
		try
		{
			if (newrecord != nullptr)
			{
				savetape->saveRecord(*newrecord, true);
				if (lastrecord != nullptr && lastrecord != lastrecordA && lastrecord != lastrecordB) delete lastrecord;
				lastrecord = newrecord;
				newrecord = nullptr;
				newRunStart = false;
			}
		}
		catch (int e) { throw e; };

	}

	//Clean Up:
	if (lastrecord != nullptr)
	{
		runCount++;
		if (savetape == this->tape_no1)
		{
			runCountA = runCount;
			this->dummytape = Tape_no1;
		}
		else
		{
			runCountB = runCount;
			this->dummytape = Tape_no2;
		}
		delete lastrecord;
		lastrecord = nullptr;
		this->dummyruns = nextfib - runCount;
	}
	if (lastrecordA != nullptr)
	{
		delete lastrecordA;
		lastrecordA = nullptr;
	}
	if (lastrecordB != nullptr)
	{
		delete lastrecordB;
		lastrecordB = nullptr;
	}

	//check whether file is sorted:
	bool sortend = (runCount == 1 && savetape == this->tape_no1);

	//Print:
	if (this->debug_mode)
	{
		system("cls");
		std::cout << "Tape No.1    Runs: ";
		if (this->dummytape == Tape_no1) std::cout << runCountA + dummyruns << "(" << dummyruns << ")" << std::endl;
		else std::cout << runCountA << std::endl;
		Showfile::showFile(this->tape_no1, true,0, true);
		if (sortend == false)
		{
			std::cout << "--------------------------------------------------" << std::endl;
			std::cout << "Tape No.2     Runs: ";
			if (this->dummytape == Tape_no2) std::cout << runCountB + dummyruns << "(" << runCountB << ")" << std::endl;
			else std::cout << runCountB << std::endl;
			Showfile::showFile(this->tape_no2, true,0,true);
		}
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
	}

	//Returns:
	if (sortend) return true; //all records on one tape on one run => file already sorted
	return false;
}

bool PolyphaseSort::phaseSort(Diskfile* sourceTape_no1, Diskfile* sourceTape_no2, Diskfile* destTape_no3)
{
	enum sortPhaseType {Dummy, Merge, LeftTape_no1, LeftTape_no2};
	sortPhaseType sortPhase = (this->phases == 1) ? Dummy : Merge;
	sortPhaseType lastsortPhase = sortPhase;
	tape savetape = Tape_no1;
	Record* newrecordTape_no1 = nullptr;
	Record* newrecordTape_no2 = nullptr;
	Record* lastrecordTape_no1 = nullptr;
	Record* lastrecordTape_no2 = nullptr;
	Record* saverecord = nullptr;
	bool endTapeTape_no1 = false;
	bool endTapeTape_no2 = false;
	bool eofTape_no1 = false;
	bool eofTape_no2 = false;
	bool end = false;
	unsigned int recordsTape_no1 = 0;
	unsigned int recordsTape_no2 = 0;
	unsigned int runsCount = 0;
	bool doSave = true;
	while (end == false)
	{	
		//check if there are any records left:
		doSave = true;
		if(!endTapeTape_no1)endTapeTape_no1 = sourceTape_no1->checkeof();
		if(!endTapeTape_no2)endTapeTape_no2 = sourceTape_no2->checkeof();

		//get the records:
		try
		{
			if (newrecordTape_no1 == nullptr && endTapeTape_no1 == false)
			{
				newrecordTape_no1 = new Record(sourceTape_no1->getRecord(this->preoffsetTape_no1,true, true));
			}
			if (newrecordTape_no2 == nullptr && endTapeTape_no2 == false)
			{
				
				newrecordTape_no2 = new Record(sourceTape_no2->getRecord(this->preoffsetTape_no2,true, true));
			}

		}
		catch (int e) { throw e; };
		

		switch (sortPhase)
		{
			case Dummy:
			{
				/*
					One of the tapes has additional "dummy" runs (only first phase), they are on the start of the tape (always tape No. 1),
					when I combine dummy run with a normal run form the other tape it means I copy the normal run
					and decrease amount of dummy runs.
					When the other tape is empty, then I simply delete all dummy runs
				*/
				if (this->dummyruns > 0)
				{
				
					if (lastrecordTape_no2 != nullptr && newrecordTape_no2 != nullptr && Record::compare(*lastrecordTape_no2, *newrecordTape_no2) == false )
					{// end of the run
						this->dummyruns--;
						runsCount++;
						doSave = false;
					
					}
					//save record from No. 2 tape:						
					savetape = Tape_no2;
					break;
				}
				else
				{ //Dummy runs ended
					lastsortPhase = sortPhase;
					sortPhase = Merge;
					continue;
				}
				break;
			}
			case Merge:
			{
				/*
					Merge sort:
					I am combining 2 runs from 2 tapes into one run on one tape,
					as long as there are record (tron that runs) on both tapes,
					I will use "Merge" state to decide which record from those two should be saved first
					when run will end on the one of the tapes (or will reach eof) then I change state to "Name"Left
				*/
				if (endTapeTape_no1 && newrecordTape_no1 == nullptr)
				{
					sortPhase = LeftTape_no2;
					continue;
				}
				if (endTapeTape_no2 && newrecordTape_no2 == nullptr)
				{
					sortPhase = LeftTape_no1;
					continue;
				}
				if (lastrecordTape_no1 != nullptr && newrecordTape_no1 != nullptr && Record::compare(*lastrecordTape_no1, *newrecordTape_no1) == false)
				{ // records from Tape No. 1 are from another run
					lastsortPhase = sortPhase;
					sortPhase = LeftTape_no2;
					savetape = Tape_no1; //remove last record from Tape no. 1
					doSave = false;
					break;
				}
				if (lastrecordTape_no2 != nullptr && newrecordTape_no2 != nullptr && Record::compare(*lastrecordTape_no2, *newrecordTape_no2) == false)
				{// records from Tape no. 2 are from another run
					lastsortPhase = sortPhase;
					sortPhase = LeftTape_no1;
					savetape = Tape_no2;//remove last Tape no. 2 record
					doSave = false;
					break;
				}
				if (newrecordTape_no1 != nullptr && newrecordTape_no2 != nullptr && Record::compare(*newrecordTape_no1, *newrecordTape_no2))
				{ // rA <= rB
					savetape = Tape_no1;
				}
				else
				{ // rb <= rA
					savetape = Tape_no2;
				}
				break;
			}
			case LeftTape_no1:
			{
				/*
					Fist of two states that indicates that only Tape no. 1 has records left in this run ( or at all)
					Firstly I must check if tape no. 1 has any records left (in this run/at all)
					and if tape no. 2 is empty too, then I should start the new run returning to the merge state or end (if both eofs) 
					sorting whatsoever
				*/
				if (endTapeTape_no1 && newrecordTape_no1 == nullptr)
				{
					end = true;
					runsCount++;
					continue;
				}
				if (lastrecordTape_no1 != nullptr && newrecordTape_no1 != nullptr && Record::compare(*lastrecordTape_no1, *newrecordTape_no1) == false)
				{
					//Tape no. 1 run has ended
					if (!endTapeTape_no2 || newrecordTape_no2 != nullptr)
					{ // Tape no. 2 isn't empty 
						if (!endTapeTape_no1)
						{ // and Tape no. 1 too so I start a new run
							lastsortPhase = sortPhase;
							sortPhase = Merge;
							doSave = false;
							runsCount++;
						}
						else
						{// Tape no. 1 is empty
							end = true;
							runsCount++;
							continue;
						}
						
					}
					else // Tape no. 2 is empty and Tape no. 1 ended run or is empty
					{
						end = true;
						runsCount++;
						continue;
					}
				}
				if (newrecordTape_no1 != nullptr)
				{ // next record from Tape no. 1 is saved
					savetape = Tape_no1;
					break;
				}
			}
			case LeftTape_no2:
			{
				/*
					Second of two states that indicates that only Tape no. 2 has records left in this run ( or at all)
					Firstly I must check if tape no. 2 has any records left (in this run/at all)
					and if tape no. 1 is empty too, then I should start the new run returning to the merge state or end (if both eofs)
					sorting whatsoever
				*/
				if (endTapeTape_no2 && newrecordTape_no2 == nullptr)
				{
					end = true;
					runsCount++;
					continue;
				}
				if (lastrecordTape_no2 != nullptr && newrecordTape_no2 != nullptr && Record::compare(*lastrecordTape_no2, *newrecordTape_no2) == false)
				{ // end ot the Brutus run
					if (!endTapeTape_no1 || newrecordTape_no1 != nullptr)
					{
						if (!endTapeTape_no2  )
						{
							lastsortPhase = sortPhase;
							sortPhase = Merge;
							doSave = false; //start new run
							runsCount++;
						}
						else
						{// Tape no. 2 is empty
							end = true;
							runsCount++;
							continue;
						}
					}
					else
					{
						end = true;
						runsCount++;
						continue;
					}
				}
				// if LeftTape_no2 was called correctly ( there is no Tape no. 1 record in this run left)
				// record from tape no. 2 should be saved
				if (newrecordTape_no2 != nullptr)
				{
					savetape = Tape_no2;
					break;
				}
			}
		}


		//choose tape to save:
		switch (savetape)
		{
			case Tape_no1:
			{
				if (lastrecordTape_no1 != nullptr)
				{
					delete lastrecordTape_no1;
				}
				if (doSave)
				{
					recordsTape_no1++;
					lastrecordTape_no1 = newrecordTape_no1;
					saverecord = newrecordTape_no1;
					newrecordTape_no1 = nullptr;
				}
				else
				{
					lastrecordTape_no1 = nullptr;
				}

				break;
			}
			case Tape_no2:
			{
				if (lastrecordTape_no2 != nullptr)
				{
					delete lastrecordTape_no2;
				}
				if (doSave)
				{
					recordsTape_no2++;
					lastrecordTape_no2 = newrecordTape_no2;
					saverecord = newrecordTape_no2;
					newrecordTape_no2 = nullptr;
				}
				else
				{
					lastrecordTape_no2 = nullptr;
				}

				break;
			}
		}

		//save:
		try
		{
			if (saverecord != nullptr)
			{
				destTape_no3->saveRecord(*saverecord, true);
				saverecord = nullptr;
			}
		}
		catch (int e) { throw e; };

	}

	//clean up:
	if (lastrecordTape_no2 != nullptr)
	{
		delete lastrecordTape_no2;
		lastrecordTape_no2 = nullptr;
	}
	if (lastrecordTape_no1 != nullptr)
	{
		delete lastrecordTape_no1;
		lastrecordTape_no1 = nullptr;
	}

	//calculate preoffset:
	bool sortend = endTapeTape_no1 && endTapeTape_no2;
	this->preoffsetTape_no1 = 0;
	this->preoffsetTape_no2 = 0;
	if (sortend == false && endTapeTape_no2)
	{
		this->preoffsetTape_no1 += recordsTape_no1;
	}
	else if (sortend == false && endTapeTape_no1)
	{
		this->preoffsetTape_no2 += recordsTape_no2;
	}
	sourceTape_no1->reset();
	sourceTape_no2->reset();

	//Print:
	if (this->debug_mode)
	{
		system("cls");
		std::cout << "Sorting Phase: " << this->phases << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		if (!endTapeTape_no1)
		{
			std::cout << "First Source tape: " << std::endl;
			Showfile::showFile(sourceTape_no1, true, this->preoffsetTape_no1,true);
			std::cout << "--------------------------------------------------" << std::endl;
		}
		if (!endTapeTape_no2)
		{
			std::cout << "Second Source tape" << std::endl;
			Showfile::showFile(sourceTape_no2, true, this->preoffsetTape_no2,true);
			std::cout << "--------------------------------------------------" << std::endl;
		}
		std::cout << "Destination Tape, Runs: " << runsCount <<  std::endl;
		Showfile::showFile(destTape_no3, true,0,true);
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
	}

	//closure:
	if (sortend) return true;
	return false;
}

void PolyphaseSort::sort()
{
	//Prepare:
	system("cls");
	try
	{
		this->tape_no1->clearFile(true);
		this->tape_no2->clearFile(true);
		this->tape_no3->clearFile(true);
	}
	catch (int e)
	{
		if (this->debug_mode == false) system("cls");
		std::cout << "Sorting error" << std::endl;
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
		return;
	}

	//DISTRIBUTE
	this->phases = 0;
	bool distrReturn = false;
	try
	{
		distrReturn = distribute();
	}
	catch (int e)
	{
		if (this->debug_mode == false) system("cls");
		std::cout << "Sorting error - is file empty?" << std::endl;
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
		return;
	}
	if (distrReturn)
	{

		//File is sorted
		this->saveSortedFile(this->datafile, this->tape_no1);
		this->tape_no1 = nullptr;
		//cloasure:
		return;
	}


	//PHASE SORT
	Diskfile* sourceTape_no1 = this->tape_no1; //source with more runs
	Diskfile* sourceTape_no2 = this->tape_no2; // source with less runs
	Diskfile* destTape_no3 = this->tape_no3; //desitnation
	if (dummytape == Tape_no2)
	{ // if tape no. 2 has more runs, change A with B to keep sourceTape_no1 with more runs
		sourceTape_no1 = this->tape_no2;
		sourceTape_no2 = this->tape_no1;
	}
	this->phases = 1;
	try
	{ //Sorting:
		while (phaseSort(sourceTape_no1, sourceTape_no2, destTape_no3) == false)
		{
			//next phase of sorting
			this->phases++;
			Diskfile* buffTape = nullptr;
			if (this->preoffsetTape_no1 != 0)
			{// source tape no. 2 is destination tape of next phase
				buffTape = sourceTape_no2;
				sourceTape_no2 = sourceTape_no1;
				sourceTape_no1 = destTape_no3;
				destTape_no3 = buffTape;
				this->preoffsetTape_no2 = this->preoffsetTape_no1;
				this->preoffsetTape_no1 = 0;
			}
			else if (this->preoffsetTape_no2 != 0)
			{// source tape no. 1 is destination tape of next phase
				buffTape = sourceTape_no1;
				sourceTape_no1 = destTape_no3;
				destTape_no3 = sourceTape_no1;
			}
			destTape_no3->clearFile(false);


		}

	}
	catch (int e)
	{
		if (this->debug_mode == false) system("cls");
		std::cout << "Sorting error" << std::endl;
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
		return;
	}

	//SAVE
	this->saveSortedFile(this->datafile, destTape_no3);
}

void PolyphaseSort::saveSortedFile(Diskfile* datafile, Diskfile* destTape_no3)
{
	system("cls");
	unsigned int writecounter, readcounter;
	writecounter = this->datafile->getDiskInOperations();
	readcounter = this->datafile->getDiskOutOperations();
	try
	{
		datafile->replaceFile(destTape_no3,this->debug_mode);
	}
	catch (int e)
	{
		std::cout << "Error occured while saving sorted file" << std::endl;
		std::cout << "To continue press any letter and Enter" << std::endl;
		char c;
		std::cin >> c;
		system("cls");
	}
	writecounter += this->tape_no1->getDiskInOperations()
		+ this->tape_no2->getDiskInOperations() + this->tape_no3->getDiskInOperations();
	readcounter += this->tape_no1->getDiskOutOperations()
		+ this->tape_no2->getDiskOutOperations() + this->tape_no3->getDiskOutOperations();
	
	if (destTape_no3 == this->tape_no1) this->tape_no1 = nullptr;
	else if (destTape_no3 == this->tape_no2) this->tape_no2 = nullptr;
	else if (destTape_no3 == this->tape_no3) this->tape_no3 = nullptr;

	std::cout << "File Sorted Successfully" << std::endl;
	std::cout << "Phases: " << this->phases << std::endl;
	std::cout << "Read operations: " << readcounter<< std::endl;
	std::cout << "Write operations: " << writecounter << std::endl;
	std::cout << "To continue press any letter and Enter" << std::endl;
	char c;
	std::cin >> c;
	system("cls");
}

unsigned int Fibbonacci::nextFib()
{
	if (!prev && !that)
	{
		that++; // turns to 1
		return next; //for 0 and 0 we have 1 
	}
	//normally:
	next = prev + that;
	prev = that;
	that = next;
	return next;
}