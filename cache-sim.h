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
		bool is_store;
		unsigned long address;
	public:
		Line(bool, unsigned long);
		Line(std::string);
		Line(const Line & line){this->is_store=line.isStore(); this->address=line.getAddress();};
		bool isStore() const {return this->is_store;};
		bool isLoad() const {return !this->is_store;};
		unsigned long getAddress() const {return this->address;};
		
};

class FileReader{
	private:
		std::ifstream * file;
		std::vector<Line> lines;
		bool read;
		std::size_t index;
	public:
		FileReader(std::ifstream *);
		~FileReader(){file=NULL;};
		bool isRead() const {return this->read;};
		bool readFile();
		Line operator[](std::size_t index) const {return this->lines[index];};
		bool atStart() const {return this->index==0;};
		bool atEnd() const {return this->index==this->getSize()-1;};
		bool next();
		Line current() const {return this->lines[this->index];};
		void start(){this->index=0;};
		void end(){this->index=this->getSize()-1;};
		std::size_t getIndex() const {return this->index;};
		std::size_t getSize() const {return this->lines.size();};
};

class Tracker{
	private:
		unsigned long hits;
		unsigned long total;
	public:
		Tracker(){this->hits=this->total=0;};
		Tracker(const Tracker & tracker){this->hits=tracker.getHits(); this->total=tracker.getTotal();};
		double percent() const {return ((double) this->hits)/this->total * 100;};
		void addHit(){this->hits++; this->total++;};
		void addMiss(){this->total++;};
		unsigned long getHits() const {return this->hits;};
		unsigned long getMisses() const {return this->total-this->hits;};
		unsigned long getTotal() const {return this->total;};
};

class CacheLine{
	private:
		unsigned long index;
	public:
		CacheLine(){this->index=0; this->valid=false; this->tag=0;};
		CacheLine(unsigned long index){this->index=index; this->valid=false; this->tag=0;};
		CacheLine(const CacheLine & o){this->index=o.getIndex(); this->valid=o.valid; this->tag=o.tag;};
		bool valid;
		unsigned long tag;
		unsigned long getIndex() const {return this->index;};
		void printLine();
};

class DMC{
	private:
		FileReader * reader;
		unsigned int cache_size;
		const unsigned int line_size = 32;
		unsigned int tag_size;
		unsigned int index_size;
		unsigned int index_max;
		unsigned int tag_max;
		CacheLine * lines;
		Tracker tracker;
		void setSizesAndMaxes();
		void printCache();
		void printVars();
	public:
		DMC(FileReader *, unsigned int);
		unsigned int getCacheSize() const {return this->cache_size;};
		Line operator[](std::size_t index) const {return (*this->reader)[index];};
		unsigned long numLines() const {return (cache_size*1024)/line_size;};
		unsigned int getTagSize() const {return this->tag_size;};
		unsigned long getHits() const {return this->tracker.getHits();};
		unsigned long getMisses() const {return this->tracker.getMisses();};
		unsigned long getTotal() const {return this->tracker.getTotal();};
		double percent() const {return this->tracker.percent();};
		double run();
		bool step();
};

#endif
