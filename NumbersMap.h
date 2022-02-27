#pragma once
#include <unordered_map>
#include "Record.h"
#include <array>

class NumbersMap 
{
private:
	std::unordered_map<uint32_t, std::array<float, 6>> nmap;
public:
	float find_mean(uint32_t record); //return geometric mean
	uint32_t add_numbers(float first, float second, float third, float fourth, float fifth); //generate new record hash and add numbers to map
	float calc_mean(float arr[], int n ); //calculate geometric mean
	void clear_map(); //all the records in map are dropped, leaving it with size of 0
	NumbersMap();
	~NumbersMap();
	friend class Record;
};
