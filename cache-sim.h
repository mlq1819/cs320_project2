#ifndef MQ_CACHE_SIM_H
#define MQ_CACHE_SIM_H
#include <stdlib.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <exception>

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
		std::size_type index;
	public:
		FileReader(std::ifstream);
		bool isRead(){return this->read;};
		bool readFile();
		Line operator[](std::size_type index){return this->lines[index];};
		bool atStart(){return this->index==0;};
		bool atEnd(){return this->index==this->getSize-1;};
		bool next();
		Line current(){return this->lines[this->index];};
		void start(){this->index=0;};
		void end(){this->index=this->getSize()-1;};
		std::size_type getindex(){return this->index;};
		std::size_type getSize(){return this->lines.size();};
};

class DMC{
	private:
		FileReader reader;
		unsigned int cache_size;
	public:
		DMC(FileReader, unsigned int);
		
};

#endif
