#include "Record.h"


std::unordered_map<uint32_t, std::array<float, 6>> Record::nmap;

Record::Record(float first, float second, float third, float fourth, float fifth)
{
	/*
	Constructor meant to create Record form user input
	Argument: specified numbers to create geometric mean from
		
	*/

	recordhash record;
	std::random_device rd;
	constexpr recordhash REC_MIN = 0;
	constexpr recordhash REC_MAX = ~((recordhash)0); ;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<recordhash> distr(REC_MIN, REC_MAX);
	//generate record hash and check if it does not exist in map
	do {
		record = distr(eng);
	} while (!(this->nmap.find(record) == this->nmap.end()));
	float num[5] = { first, second, third, fourth, fifth };
	float mean = calc_mean(num, 5);
	std::array<float, 6> numbers = { first, second, third, fourth, fifth, mean };
	// add record, numbers pair to map
	this->nmap.insert(make_pair(record, numbers));

	//std::unordered_map<recordint, std::array<float, 6>>::iterator itr;


	 this->record=record;
}



Record::Record(unsigned int record) 
{
	/*
	Constructor meant to create Record objects read from tape file
	Argument: record - hash of the given record
	*/

	this->record = record;
	
}

Record::Record(std::mt19937 * rng) 
{
	/*
	Constructor meant to create random Record objects
	Argument:std::mt19937: pointer to the generator (not used currently)
	*/
	
	//replaced rng with better generator from uniform real distribution
	std::random_device rd;
	//specifying range of generation
	constexpr int FLOAT_MIN = 10;
	constexpr int FLOAT_MAX = 1000;
	float num[NUM_COUNT];
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

	for (int n = 0; n < 5; ++n) {
		num[n] = distr(eng);
		if (num[n] < 0) n--;
	}

	constexpr recordhash REC_MIN = 0;
	constexpr recordhash REC_MAX = ~((recordhash)0); ;

	std::uniform_int_distribution<recordhash> dist(REC_MIN, REC_MAX);
	//generate record hash and check if it does not exist in map
	do {
		record = dist(eng);
	} while (!(this->nmap.find(record) == this->nmap.end()));
	
	float mean = calc_mean(num, 5);
	std::array<float, 6> numbers = { num[0], num[1], num[2], num[3], num[4], mean };
	// add record, numbers pair to map
	this->nmap.insert(make_pair(record, numbers));

	//std::unordered_map<recordint, std::array<float, 6>>::iterator itr;


	this->record = record;
	

}

Record::~Record() {};


void Record::printRecordString() 
{
	//prints record to the console
	std::unordered_map<recordhash, std::array<float, 6>>::const_iterator got = this->nmap.find(record);
	if (got == this->nmap.end()) {
		std::cout << "Failed to find record. ";
		std::cout << std::endl;
	}
	else {
		
		for (int i = 0; i < NUM_COUNT; i++) {
			std::cout << i + 1 << ". " <<got->second[i]  << " ,";
		}
		std::cout << "g_mean: " << got->second[NUM_COUNT];
		std::cout << std::endl;
	}
}



bool Record::compare(Record r1, Record r2)
{
	return (r1.find_mean() <= r2.find_mean()) ? true : false;
}

recordhash Record::getRecord()
{
	return this->record;
}

float Record::find_mean()
{

	std::unordered_map<recordhash, std::array<float, 6>>::const_iterator got = this->nmap.find(uint32_t(this->record));
	if (got == this->nmap.end())
		return -1.0f;
	else
		return got->second[5];

}



float Record::calc_mean(float arr[], int n)
{
	// declare sum variable and
// initialize it to 0.
	float sum = 0;

	// Compute the sum of all the
	// elements in the array.
	for (int i = 0; i < n; i++)
		sum = sum + log(arr[i]);

	// compute geometric mean through formula
	// antilog(((log(1) + log(2) + . . . + log(n))/n)
	// and return the value to main function.
	sum = sum / n;

	return exp(sum);
}

void Record::clear_map()
{
	nmap.clear();
}