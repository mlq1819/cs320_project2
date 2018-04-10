#include "cache-sim.h"
#define FORCE false
#define OUTPUT true

using namespace std;

int main(int argc, char *argv[]){
	
	if(!FORCE){
		if((OUTPUT && argc<3) || argc<2){
			cout << "Not enough arguments" << endl;
			return 0;
		}
	}
	ifstream file;
	if(FORCE)
		file.open("trace1.txt");
	else
		file.open(argv[1]);
	if(!file || !file.good()){
		cout << "Bad input file" << endl;
		if(file.is_open())
			file.close();
		return 1;
	}
	
	ofstream output;
	if(OUTPUT){
		if(FORCE)
			output.open("output.txt");
		else
			output.open(argv[2]);
		if(!output || !output.good()){
			cout << "Bad output file" << endl;
			if(output.is_open())
				output.close();
			return 1;
		}
	}
	cout << "Creating Reader..." << endl;
	FileReader reader = FileReader(&file);
	cout << "Reading from" << argv[1] << endl;
	if(!reader.readFile()){
		cout << "Failed to read file" << endl;
		file.close();
		return false;
	}
	file.close();
	if(OUTPUT)
		cout << "Outputting to " << argv[2] << endl;
	
	unsigned int sizes[] = {1, 4, 16, 32};
	for(int i=0; i<4; i++){
		DMC dmc = DMC(&reader, i);
		cout << "Direct-Mapped Cache: " << i << "kB" << endl;
		cout << dmc.run() << "% Accurate\t" << dmc.getHits() << "," << dmc.getTotal() << endl;
		if(OUTPUT)
			output << dmc.getHits() << "," << dmc.getTotal() << ";" << endl;
	}
	
	
	return 0;
}

Line::Line(bool isStore, unsigned long address){
	this->is_store=isStore;
	this->address=address;
}

Line::Line(string str){
	this->is_store=(str[0]=='S');
	this->address=stol(str.substr(4,12), 0, 16);
}

FileReader::FileReader(ifstream * file){
	this->file=file;
	this->read=false;
	this->lines=vector<Line>();
}

bool FileReader::readFile(){
	if(!this->file->is_open())
		return false;
	this->file->clear();
	this->file->seekg(0, ios_base::beg);
	string str;
	cout << "Reading file..." << endl;
	while(getline(*file, str))
		this->lines.push_back(Line(str));
	cout << "File read" << endl;
	this->lines.shrink_to_fit();
	this->read=true;
	return true;
}

bool FileReader::next(){
	if(this->index<this->getSize()-1){
		this->index++;
		return true;
	}
	return false;
}

DMC::DMC(FileReader * reader, unsigned int cache_size){
	this->reader=reader; 
	this->cache_size=cache_size; 
	this->tracker=Tracker();
	this->setTagSize();
	unsigned long num_lines = this->numLines();
	CacheLine lines[num_lines];
	this->lines = lines;
	for(unsigned long i=0; i<num_lines; i++)
		this->lines[i]=CacheLine(i);
}

void DMC::setTagSize(){
	unsigned int num=1;
	this->tag_size=0;
	unsigned int num_lines = this->numLines();
	while(num<num_lines){
		this->tag_size++;
		num = num << 1;
	}
}

double DMC::run(){
	if(!this->reader->isRead())
		this->reader->readFile();
	else
		this->reader->start();
	do{
		this->step();
	} while (this->reader->next());
	return this->tracker.percent();
}

bool DMC::step(){
	Line current = this->reader->current();
	unsigned long index = current.getAddress() >> this->tag_size;
	unsigned long tag = current.getAddress()%(1 << this->tag_size);
	if(this->lines[index].valid && this->lines[index].tag==tag){
		this->tracker.addHit();
		return true;
	}
	this->tracker.addMiss();
	this->lines[index].tag=tag;
	this->lines[index].valid=true;
	return false;
	
}