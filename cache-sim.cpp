#include "cache-sim.h"
#define FORCE false
#define OUTPUT true
#define DEBUG true

using namespace std;

int main(int argc, char *argv[]){
	try{
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
	cout << "Reading from " << argv[1] << "..." << endl;
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
		DMC dmc = DMC(&reader, sizes[i]);
		cout << "Direct-Mapped Cache: " << sizes[i] << "kB" << endl;
		cout << dmc.run() << "% Accurate\t" << dmc.getHits() << "," << dmc.getTotal() << endl;
		if(OUTPUT)
			output << dmc.getHits() << "," << dmc.getTotal() << ";" << endl;
	}
	} catch (exception e){
		cout << "\nException caught" << endl;
		throw e;
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
	this->setSizesAndMaxes();
	unsigned long num_lines = this->numLines();
	CacheLine temp_lines[num_lines];
	this->lines = temp_lines;
	for(unsigned long i=0; i<=index_max; i++)
		this->lines[i]=CacheLine(i);
}

void DMC::setSizesAndMaxes(){
	this->tag_max=1;
	this->tag_size=0;
	unsigned int num_lines = this->numLines();
	while(this->tag_max<<num_lines){
		this->tag_size++;
		this->tag_max*=2;
	}
	this->index_size=128-this->tag_size;
	this->index_max=1;
	for(unsigned int i=1; i<this->index_size; i++)
		this->index_max = this->index_max*=2;
}

Double DMC::run(){
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
<<<<<<< HEAD
	unsigned long index = current.getAddress()/this->tag_max;
	unsigned long tag = current.getAddress()%(this->tag_max+1);
	if(this->lines[index].valid && this->lines[index].tag==tag){
		if(DEBUG)
			cout << "Line " << this->reader->getIndex() << " Hit!" << endl;
=======
	unsigned long index = current.getAddress() >> this->tag_size;
	unsigned long tag = current.getAddress()%(1 << this->tag_size);
	if(this->lines)[index].valid && this->lines[index].tag==tag){
>>>>>>> 5af4cc56557ea5c911b87bcc2e957d38208a85a8
		this->tracker.addHit();
		return true;
	}
	this->tracker.addMiss();
	this->lines[index].tag=tag;
	this->lines[index].valid=true;
	return false;
	
}
