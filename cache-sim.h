#ifndef MQ_CACHE_SIM_H
#define MQ_CACHE_SIM_H
#include <stdlib.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class Line{
	private:
		bool isStore;
		unsigned long address;
	public:
		Line(bool, address);
		Line(std::string);
		bool isStore(){return this->isStore;};
		bool isLoad(){return !this->isStore;};
		unsigned long getAddress(){return this->address;};
		
};

class FileReader{
	private:
		std::ifstream file;
		std::vector<Line> lines;
		bool read;
		bool readNext();
	public:
		FileReader(std::ifstream);
		bool readFile();
		Line operator[](std::size_type);
};

class DMC{
	private:
		
	public:
		
};

#endif
