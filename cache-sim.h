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
		Line(const Line & line){this->isStore=line.isStore(); this->address=line.getAddress();};
		bool isStore() const {return this->isStore;};
		bool isLoad() const {return !this->isStore;};
		unsigned long getAddress() const {return this->address;};
		
};

class FileReader{
	private:
		std::ifstream file;
		std::vector<Line> lines;
		bool read;
		std::size_type index;
	protected:
		std::ifstream getFile() const {return this->file;};
		std::vector<Line> getLines() const {return this->lines;};
	public:
		FileReader(std::ifstream);
		FileReader(const FileReader &);
		bool isRead() const {return this->read;};
		bool readFile();
		Line operator[](std::size_type index) const {return this->lines[index];};
		bool atStart() const {return this->index==0;};
		bool atEnd() const {return this->index==this->getSize-1;};
		bool next();
		Line current() const {return this->lines[this->index];};
		void start(){this->index=0;};
		void end(){this->index=this->getSize()-1;};
		std::size_type getindex() const {return this->index;};
		std::size_type getSize() const {return this->lines.size();};
};

class Tracker{
	private:
		unsigned long correct;
		unsigned long total;
	public:
		Tracker(){this->correct=this->total=0;};
		Tracker(const Tracker & tracker){this->correct=tracker.getCorrect(); this->total=tracker.getTotal();};
		double percent() const {return ((double) this->correct)/this->total * 100;};
		void addCorrect(){this->correct++; this->total++;};
		void addWrong(){this->total++;};
		unsigned long getCorrect() const {return this->correct;};
		unsigned long getTotal() const {return this->total;};
}

class DMC{
	private:
		FileReader reader;
		unsigned int cache_size;
		const unsigned int line_size = 32;
		Tracker tracker;
	public:
		DMC(FileReader & reader, unsigned int cache_size){this->reader=reader; this->cache_size=cache_size; this->tracker=Tracker();};
		unsigned int getCacheSize(){return this->cache_size;};
		Line operator[](std::size_type index){return this->reader[index];};
		unsigned int numLines(){return cache_size*1024/line_size;};
		unsigned int tag_size();
		
};

#endif
