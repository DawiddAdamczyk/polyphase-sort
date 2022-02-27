#include "NumbersMap.h"


NumbersMap::NumbersMap()
{
	std::unordered_map<recordhash, std::array<float, 6>> map;
	this->nmap = map;
}

NumbersMap::~NumbersMap()
{
}

float NumbersMap::find_mean(uint32_t record)
{

	//float rec = 0;
	std::unordered_map<recordhash, std::array<float, 6>>::const_iterator got = this->nmap.find(record);
	if (got == this->nmap.end())
		return -1.0f;
	else
		return got->second[5];

}

recordhash NumbersMap::add_numbers(float first, float second, float third, float fourth, float fifth)
{
	

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
	float mean = calc_mean(num,5);
	std::array<float, 6> numbers = { first, second, third, fourth, fifth, mean };
	// add record, numbers pair to map
	this->nmap.insert(make_pair(record, numbers));

	//std::unordered_map<recordint, std::array<float, 6>>::iterator itr;


	return record;
}

float NumbersMap::calc_mean(float arr[], int n )
{
	// declare sum variable and
// initialize it to 1.
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

void NumbersMap::clear_map()
{
	this->nmap.clear();
}


