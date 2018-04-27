#include "cache-sim.h"
#define FORCE false
#define OUTPUT true
#define DEBUG true
#define FINEDEB false
#include <vector>

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
	int max = 4;
	if(FINEDEB)
		max=1;
	for(int i=0; i<max; i++){
		DMC dmc = DMC(&reader, sizes[i]);
		cout << "\nDirect-Mapped Cache: " << sizes[i] << "kB" << endl;
		dmc.run();
		cout << dmc.percent() << "% Accurate: " << dmc.getHits() << ", " << dmc.getTotal() << "\n" << endl;
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

void CacheLine::printLine(){
	if(this->valid)
		cout << "1:";
	else
		cout << "0:";
	cout << this->tag << ":" << this->index;
}

DMC::DMC(FileReader * reader, unsigned int cache_size){
	this->reader=reader; 
	this->cache_size=cache_size; 
	this->tracker=Tracker();
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<CacheLine>();
	for(unsigned int i=0; i<this->index_max; i++)
		this->lines.push_back(CacheLine(i));
	this->lines.shrink_to_fit();
}

void DMC::setSizesAndMaxes(){
	this->index_max=1;
	this->index_size=0;
	unsigned int num_lines = this->numLines();
	while(this->index_max<num_lines){
		this->index_size++;
		this->index_max=this->index_max*2;
	}
	this->offset_size=(4-this->index_size%4)%4;
	this->offset_max=1;
	for(unsigned int i=1; i<=this->offset_size; i++)
		this->offset_max*=2;
	this->tag_size=32-this->index_size-this->offset_size;
	this->tag_max=1;
	for(unsigned int i=1; i<=this->tag_size; i++)
		this->tag_max=this->tag_max*2;
}

unsigned long DMC::maxAddress() const {
	unsigned long toReturn=1;
	for(unsigned int i=1; i<=this->line_size; i++)
		toReturn*=2;
	return toReturn;
}

double DMC::run(){
	if(!this->reader->isRead())
		this->reader->readFile();
	else
		this->reader->start();
	do{
		this->step();
	} while (this->reader->next());
	if(DEBUG){
		this->printCache();
		this->printVars();
	}
	return this->tracker.percent();
}

bool DMC::step(){
	Line current = this->reader->current();
	unsigned long index = (current.getAddress()%this->index_max);
	if(DEBUG && index>=this->index_max)
		cout << "WARNING: index out of bounds at " << current.getAddress() << ":" << index << ">=" << this->index_max << endl;
	unsigned long tag = (current.getAddress()/this->index_max)/this->offset_max;
	if(DEBUG && index>=this->index_max)
		cout << "WARNING: tag out of bounds at " << current.getAddress() << ":" << tag << ">=" << this->tag_max << endl;
	/*if(current.isStore()){
		this->lines[index].tag=tag;
		this->lines[index].valid=true;
		this->tracker.addMiss();
		if(FINEDEB){
			cout << "Store:\t" << current.getAddress() << "->";
			this->lines[index].printLine();
			cout << "\t";
			this->fdb_looper++;
			if(this->fdb_looper%5==0){
				if(this->fdb_looper==25){
					this->fdb_looper=0;
					cout << endl;
				} else
					cout << "\n";
			}
		}
	} else {*/
		if(this->lines[index].valid && this->lines[index].tag==tag){
			if(FINEDEB){
				cout << "Hit:  \t" << current.getAddress() << "->";
				this->lines[index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%5==0){
					if(this->fdb_looper==25){
						this->fdb_looper=0;
						cout << endl;
					} else
						cout << "\n";
				}
			}
			this->tracker.addHit();
			return true;
		}
		if(FINEDEB){
			cout << "Miss: \t" << current.getAddress() << "->";
			this->lines[index].printLine();
			cout << "\t";
			this->fdb_looper++;
			if(this->fdb_looper%5==0){
				if(this->fdb_looper==25){
					this->fdb_looper=0;
					cout << endl;
				} else
					cout << "\n";
			}
		}
		this->tracker.addMiss();
		this->lines[index].tag=tag;
		this->lines[index].valid=true;
	//}
	return false;
}

void DMC::printCache(){
	unsigned long num_lines=this->numLines();
	for(unsigned long i=0; i<num_lines; i++){
		this->lines[i].printLine();
		cout << "; \t";
		if(i%24==23)
			cout << endl;
	}
	cout << endl;
}

void DMC::printVars(){
	cout << "cache_size: \t" << this->cache_size << "\tkB\t| numLines():\t" << this->numLines() << "\tlines\t" << endl;
	cout << "--------------------------------+---------------------------" << endl;
	cout << "line_size:  \t" << this->line_size << "\tbits\t| maxAddress:\t" << this->maxAddress() << endl;
	cout << "index_size: \t" << this->index_size << "\tbits\t| index_max: \t" << this->index_max << endl;
	cout << "tag_size:   \t" << this->tag_size << "\tbits\t| tag_max:   \t" << this->tag_max << endl;
	cout << "offset_size:\t" << this->offset_size << "\tbits\t| offset_max:\t" << this->offset_max << endl;
}