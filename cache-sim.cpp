#include "cache-sim.h"
#define FORCE false
#define OUTPUT true
#define DEBUG true
#define FINEDEB false
#define PART 0
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
	int max;
	
	if(PART==0 || PART==1){
		unsigned int sizes[] = {1, 4, 16, 32};
		max = 4;
		if(FINEDEB)
			max=1;
		for(int i=0; i<max; i++){
			DMC dmc = DMC(&reader, sizes[i]);
			cout << "\nDirect-Mapped Cache: " << sizes[i] << "kB" << endl;
			dmc.run();
			cout << dmc.percent() << "% Accurate: " << dmc.getHits() << ", " << dmc.getTotal() << "\n" << endl;
			if(OUTPUT)
				output << dmc.getHits() << "," << dmc.getTotal() << "; ";
		}
		if(OUTPUT)
				output << endl;
	}
	
	if(PART==0 || PART==2){
		unsigned int associativities[] = {2, 4, 8, 16};
		max =4;
		if(FINEDEB)
			max=1;
		for(int i=0; i<max; i++){
			SAC sac = SAC(&reader, associativities[i]);
			cout << "\n" << associativities[i] << "-Way Set-Associative Cache: " << endl;
			sac.run();
			cout << sac.percent() << "% Accurate: " << sac.getHits() << ", " << sac.getTotal() << "\n" << endl;
			if(OUTPUT)
				output << sac.getHits() << "," << sac.getTotal() << "; ";
		}
	}
	
	if(PART==0 || PART==3){
		FACL facl = FACL(&reader);
		cout << "\nFully-Associative Cache: LRU" << endl;
		facl.run();
		cout << facl.percent() << "% Accurate: " << facl.getHits() << ", " << facl.getTotal() << "\n" << endl;
		if(OUTPUT)
			output << facl.getHits() << "," << facl.getTotal() << "; ";
		if(OUTPUT)
				output << endl;
	}
	
	if(PART==0 || PART==4){
		FACH fach = FACH(&reader);
		cout << "\nFully-Associative Cache: Hot/Cold" << endl;
		fach.run();
		cout << fach.percent() << "% Accurate: " << fach.getHits() << ", " << fach.getTotal() << "\n" << endl;
		if(OUTPUT)
			output << fach.getHits() << "," << fach.getTotal() << "; ";
		if(OUTPUT)
				output << endl;
	}
	
	if(PART==0 || PART==5){
		unsigned int associativities[] = {2, 4, 8, 16};
		max=4;
		if(FINEDEB)
			max=1;
		for(int i=0; i<max; i++){
			SAC sac = SAC(&reader, associativities[i], false);
			cout << "\n" << associativities[i] << "-Way Set-Associative Cache: No allocation on write miss" << endl;
			sac.run();
			cout << sac.percent() << "% Accurate: " << sac.getHits() << ", " << sac.getTotal() << "\n" << endl;
			if(OUTPUT)
				output << sac.getHits() << "," << sac.getTotal() << "; ";
		}
		if(OUTPUT)
			output << endl;
	}
	
	if(PART==0 || PART==6){
		unsigned int associativities[] = {2, 4, 8, 16};
		max=4;
		if(FINEDEB)
			max=1;
		for(int i=0; i<max; i++){
			SAC sac = SAC(&reader, associativities[i], true, true);
			cout << "\n" << associativities[i] << "-Way Set-Associative Cache: Prefetching" << endl;
			sac.run();
			cout << sac.percent() << "% Accurate: " << sac.getHits() << ", " << sac.getTotal() << "\n" << endl;
			if(OUTPUT)
				output << sac.getHits() << "," << sac.getTotal() << "; ";
		}
		if(OUTPUT)
			output << endl;
	}
	
	if(PART==0 || PART==7){
		unsigned int associativities[] = {2, 4, 8, 16};
		max=4;
		if(FINEDEB)
			max=1;
		for(int i=0; i<max; i++){
			SAC sac = SAC(&reader, associativities[i], true, false, true);
			cout << "\n" << associativities[i] << "-Way Set-Associative Cache: Prefetch on Miss" << endl;
			sac.run();
			cout << sac.percent() << "% Accurate: " << sac.getHits() << ", " << sac.getTotal() << "\n" << endl;
			if(OUTPUT)
				output << sac.getHits() << "," << sac.getTotal() << "; ";
		}
		if(OUTPUT)
			output << endl;
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
	this->address=stoul(str.substr(4,12), 0, 16);
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

Line FileReader::peak() const {
	if(this->index<this->getSize()-1){
		return this->lines[this->index+1];
	}
	return Line(false, 0);
}

void CacheLine::printLine(){
	cout << "0x" << hex << this->address << ":" << dec;
	if(this->valid)
		cout << "1-";
	else
		cout << "0-";
	cout << "0x" << hex << this->tag << "-0x" << hex << this->index << dec;
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
	this->offset_size=0;
	for(unsigned int i=1; i<this->line_size; i*=2)
		this->offset_size++;
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
		if(FINEDEB)
			cout << endl;
		this->printCache();
		this->printVars();
	}
	return this->tracker.percent();
}

bool DMC::step(){
	Line current = this->reader->current();
	unsigned long index = (current.getAddress()>>this->offset_size)%this->index_max;
	if(DEBUG && index>=this->index_max)
		cout << "WARNING: index out of bounds at 0x" << hex << current.getAddress() << ": 0x" <<  hex << index << ">=0x" << hex << this->index_max << dec << endl;
	unsigned long tag = ((current.getAddress()>>this->offset_size)>>this->index_size);
	/*if(DEBUG && index>=this->index_max)
		cout << "WARNING: tag out of bounds at 0x" << hex << current.getAddress() << ": 0x" << hex << tag << ">=0x" << hex << this->tag_max  << dec<< endl;
	*/
	/*if(DEBUG){
		//unsigned long calculated_address=(tag*(this->maxAddress()/this->tag_max)) + index;
		unsigned long calculated_address=index;
		if(current.getAddress()!=calculated_address){
			if(this->fdb_looper%4!=0){
				this->fdb_looper=0;
				cout << endl;
			}
			cout << "WARNING: calculated address (0x" << hex << calculated_address << ") not equal to actual (0x" << hex << current.getAddress() << ")" << dec << endl;
		}
	}*/
	if(this->lines[index].valid && this->lines[index].tag==tag){
			if(FINEDEB){
				cout << "Hit:  \t0x" << hex << current.getAddress() << "->" << dec;
				this->lines[index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%4==0){
					if(this->fdb_looper==20){
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
			cout << "Miss: \t0x" << hex << current.getAddress() << "->" << dec;
			this->lines[index].printLine();
			cout << "\t";
			this->fdb_looper++;
			if(this->fdb_looper%4==0){
				if(this->fdb_looper==20){
					this->fdb_looper=0;
					cout << endl;
				} else
					cout << "\n";
			}
		}
		this->tracker.addMiss();
		this->lines[index].tag=tag;
		this->lines[index].address=current.getAddress(); //used for debugging
		this->lines[index].valid=true;
	/*if(this->lines[index].valid && this->lines[index].tag==tag){
			if(FINEDEB){
				cout << "Hit:  \t0x" << hex << current.getAddress() << "->" << dec;
				this->lines[index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%4==0){
					if(this->fdb_looper==20){
						this->fdb_looper=0;
						cout << endl;
					} else
						cout << "\n";
				}
			}
			if(DEBUG && this->lines[index].address!=current.getAddress()){
				if(this->fdb_looper%4!=0){
					this->fdb_looper=0;
					cout << endl;
				}
				cout << "BAD HIT: \t0x" << hex << current.getAddress() << "!=0x" << hex << this->lines[index].address << endl;
			}
			this->tracker.addHit();
			return true;
		}
		if(FINEDEB){
			cout << "Miss: \t0x" << hex << current.getAddress() << "->" << dec;
			this->lines[index].printLine();
			cout << "\t";
			this->fdb_looper++;
			if(this->fdb_looper%4==0){
				if(this->fdb_looper==20){
					this->fdb_looper=0;
					cout << endl;
				} else
					cout << "\n";
			}
		}
		if(DEBUG && this->lines[index].valid && this->lines[index].address==current.getAddress()){
				if(this->fdb_looper%4!=0){
					this->fdb_looper=0;
					cout << endl;
				}
				cout << "BAD MISS: \t0x" << hex << current.getAddress() << "==0x" << hex << this->lines[index].address << dec << endl;
			}
		this->tracker.addMiss();
		this->lines[index].tag=tag;
		this->lines[index].address=current.getAddress(); //used for debugging
		this->lines[index].valid=true;
	}*/
	return false;
}

void DMC::printCache(){
	unsigned long num_lines=this->numLines();
	for(unsigned long i=0; i<num_lines; i++){
		this->lines[i].printLine();
		cout << ";   \t";
		if(i%16==15)
			cout << endl;
	}
	cout << endl;
}

void DMC::printVars(){
	cout << "cache_size: \t" << dec << this->cache_size << "\tkB\t| numLines():\t" << dec << this->numLines() << "\tlines\t" << dec << endl;
	cout << "--------------------------------+---------------------------" << endl;
	cout << "line_size:  \t" << dec << this->line_size << "\tbits\t| maxAddress:\t0x" << hex << this->maxAddress() << dec << endl;
	cout << "index_size: \t" << dec << this->index_size << "\tbits\t| index_max: \t0x" << hex << this->index_max << dec << endl;
	cout << "tag_size:   \t" << dec << this->tag_size << "\tbits\t| tag_max:   \t0x" << hex << this->tag_max << dec << endl;
	cout << "offset_size:\t" << dec << this->offset_size << "\tbits\t| offset_max: \t0x" << hex << this->offset_max << dec << endl;
}

SAC::SAC(FileReader * reader, unsigned int associativity){
	this->allocate_on_write_miss=true;
	this->prefetching=false;
	this->prefetch_on_miss=false;
	this->reader=reader; 
	this->tracker=Tracker();
	this->set_associativity=associativity;
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<vector<CacheLine>>();
	this->lru=vector<vector<int>>();
	for(unsigned int i=0; i<this->index_max; i++){
		this->lines.push_back(vector<CacheLine>());
		this->lru.push_back(vector<int>());
		for(unsigned int j=0; j<this->set_associativity; j++){
			this->lines[i].push_back(CacheLine(i));
			this->lru[i].push_back(this->set_associativity-j-1);
		}
		this->lines[i].shrink_to_fit();
		this->lru[i].shrink_to_fit();
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

SAC::SAC(FileReader * reader, unsigned int associativity, bool allocate_on_write_miss){
	this->allocate_on_write_miss=allocate_on_write_miss;
	this->prefetching=false;
	this->prefetch_on_miss=false;
	this->reader=reader; 
	this->tracker=Tracker();
	this->set_associativity=associativity;
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<vector<CacheLine>>();
	this->lru=vector<vector<int>>();
	for(unsigned int i=0; i<this->index_max; i++){
		this->lines.push_back(vector<CacheLine>());
		this->lru.push_back(vector<int>());
		for(unsigned int j=0; j<this->set_associativity; j++){
			this->lines[i].push_back(CacheLine(i));
			this->lru[i].push_back(this->set_associativity-j-1);
		}
		this->lines[i].shrink_to_fit();
		this->lru[i].shrink_to_fit();
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

SAC::SAC(FileReader * reader, unsigned int associativity, bool allocate_on_write_miss, bool prefetching){
	this->allocate_on_write_miss=allocate_on_write_miss;
	this->prefetching=prefetching;
	this->prefetch_on_miss=false;
	this->reader=reader; 
	this->tracker=Tracker();
	this->set_associativity=associativity;
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<vector<CacheLine>>();
	this->lru=vector<vector<int>>();
	for(unsigned int i=0; i<this->index_max; i++){
		this->lines.push_back(vector<CacheLine>());
		this->lru.push_back(vector<int>());
		for(unsigned int j=0; j<this->set_associativity; j++){
			this->lines[i].push_back(CacheLine(i));
			this->lru[i].push_back(this->set_associativity-j-1);
		}
		this->lines[i].shrink_to_fit();
		this->lru[i].shrink_to_fit();
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

SAC::SAC(FileReader * reader, unsigned int associativity, bool allocate_on_write_miss, bool prefetching, bool prefetching_on_miss){
	this->allocate_on_write_miss=allocate_on_write_miss;
	this->prefetching=!prefetching_on_miss&&prefetching;
	this->prefetch_on_miss=prefetching_on_miss;
	this->reader=reader; 
	this->tracker=Tracker();
	this->set_associativity=associativity;
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<vector<CacheLine>>();
	this->lru=vector<vector<int>>();
	for(unsigned int i=0; i<this->index_max; i++){
		this->lines.push_back(vector<CacheLine>());
		this->lru.push_back(vector<int>());
		for(unsigned int j=0; j<this->set_associativity; j++){
			this->lines[i].push_back(CacheLine(i));
			this->lru[i].push_back(this->set_associativity-j-1);
		}
		this->lines[i].shrink_to_fit();
		this->lru[i].shrink_to_fit();
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

void SAC::setSizesAndMaxes(){
	this->index_max=1;
	this->index_size=0;
	unsigned int num_lines = this->numLines();
	while(this->index_max<num_lines){
		this->index_size++;
		this->index_max=this->index_max*2;
	}
	this->offset_size=0;
	for(unsigned int i=1; i<this->line_size; i*=2)
		this->offset_size++;
	this->offset_max=1;
	for(unsigned int i=1; i<=this->offset_size; i++)
		this->offset_max*=2;
	this->tag_size=32-this->index_size-this->offset_size;
	this->tag_max=1;
	for(unsigned int i=1; i<=this->tag_size; i++)
		this->tag_max=this->tag_max*2;
}

unsigned long SAC::maxAddress() const {
	unsigned long toReturn=1;
	for(unsigned int i=1; i<=this->line_size; i++)
		toReturn*=2;
	return toReturn;
}

double SAC::run(){
	if(!this->reader->isRead())
		this->reader->readFile();
	else
		this->reader->start();
	do{
		this->step();
	} while (this->reader->next());
	if(DEBUG){
		if(FINEDEB)
			cout << endl;
		this->printCache();
		this->printVars();
	}
	return this->tracker.percent();
}

bool SAC::step(){
	Line current = this->reader->current();
	unsigned long next_index=0;
	unsigned long next_inner_index=0;
	unsigned long index = (current.getAddress()>>this->offset_size)%this->index_max;
	
	if(DEBUG && index>=this->index_max)
		cout << "WARNING: index out of bounds at 0x" << hex << current.getAddress() << ": 0x" <<  hex << index << ">=0x" << hex << this->index_max << dec << endl;
	unsigned long tag = ((current.getAddress()>>this->offset_size)>>this->index_size);
	bool hit=false;
	unsigned int inner_index;
	for(inner_index=0; inner_index<this->lines[index].size(); inner_index++){
		if(this->lines[index][inner_index].valid && this->lines[index][inner_index].tag==tag){
			hit=true;
			break;
		}
	}
	if(hit){
		if(FINEDEB){
				cout << "Hit:  \t0x" << hex << current.getAddress() << "->" << dec;
				this->lines[index][inner_index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%4==0){
					if(this->fdb_looper==20){
						this->fdb_looper=0;
						cout << endl;
					} else
						cout << "\n";
				}
			}
		if(this->lru[index][inner_index]!=0){
			for(unsigned int i=0; i<this->lru[index].size(); i++){
				this->lru[index][i]++;
			}
			this->lru[index][inner_index]=0;
		}
		this->tracker.addHit();
		if(this->prefetching){
			Line next = this->reader->peak();
			if(next.isStore() || next.getAddress()!=0){
				next_index=(next.getAddress()>>this->offset_size)%this->index_max;
				unsigned long tag = ((next.getAddress()>>this->offset_size)>>this->index_size);
				bool hit=false;
				for(next_inner_index=0; next_inner_index<this->lines[next_index].size(); next_inner_index++){
					if(this->lines[next_index][next_inner_index].valid && this->lines[next_index][next_inner_index].tag==tag){
						hit=true;
						break;
					}
				}
				if(!hit){
					for(unsigned int i=0; i<this->lru[next_index].size(); i++){
						if(this->lru[next_index][i]>this->lru[next_index][next_inner_index])
							next_inner_index=i;
					}
					if(this->lru[next_index][next_inner_index]!=0){
						for(unsigned int i=0; i<this->lru[next_index].size(); i++){
							this->lru[next_index][i]++;
						}
					this->lru[next_index][next_inner_index]=0;
					}
				}
			}
		}
	return true;
	}
	if(FINEDEB){
		cout << "Miss: \t0x" << hex << current.getAddress() << dec << "\t";
		this->fdb_looper++;
		if(this->fdb_looper%4==0){
			if(this->fdb_looper==20){
				this->fdb_looper=0;
				cout << endl;
			} else
				cout << "\n";
		}
	}
	inner_index=0;
	if(this->allocate_on_write_miss || !current.isStore()){
		for(unsigned int i=0; i<this->lru[index].size(); i++){
			if(this->lru[index][i]>this->lru[index][inner_index])
				inner_index=i;
		}
		if(this->lru[index][inner_index]!=0){
			for(unsigned int i=0; i<this->lru[index].size(); i++){
				this->lru[index][i]++;
			}
			this->lru[index][inner_index]=0;
		}
		this->lines[index][inner_index].tag=tag;
		this->lines[index][inner_index].address=current.getAddress(); //used for debugging
		this->lines[index][inner_index].valid=true;
	}
	this->tracker.addMiss();
	if(this->prefetching || this->prefetch_on_miss){
		Line next = this->reader->peak();
		if(next.isStore() || next.getAddress()!=0){
			next_index=(next.getAddress()>>this->offset_size)%this->index_max;
			unsigned long tag = ((next.getAddress()>>this->offset_size)>>this->index_size);
			bool hit=false;
			for(next_inner_index=0; next_inner_index<this->lines[next_index].size(); next_inner_index++){
				if(this->lines[next_index][next_inner_index].valid && this->lines[next_index][next_inner_index].tag==tag){
					hit=true;
					break;
				}
			}
			if(!hit){
				for(unsigned int i=0; i<this->lru[next_index].size(); i++){
					if(this->lru[next_index][i]>this->lru[next_index][next_inner_index])
						next_inner_index=i;
				}
				if(this->lru[next_index][next_inner_index]!=0){
					for(unsigned int i=0; i<this->lru[next_index].size(); i++){
						this->lru[next_index][i]++;
					}
				this->lru[next_index][next_inner_index]=0;
				}
			}
		}
	}
	return false;
}

void SAC::printCache(){
	unsigned long num_lines=this->numLines();
	for(unsigned long i=0; i<num_lines; i++){
		for(unsigned int j=0; j<this->lines[i].size(); j++){
			this->lines[i][j].printLine();
			cout << ";   \t";
		}
		cout << endl;
	}
	cout << endl;
}

void SAC::printVars(){
	cout << "associativity:\t" << dec << this->set_associativity << "-way\t\t| numLines():\t" << dec << this->numLines() << "\tlines\t" << dec << endl;
	cout << "--------------------------------+---------------------------" << endl;
	cout << "line_size:  \t" << dec << this->line_size << "\tbits\t| maxAddress:\t0x" << hex << this->maxAddress() << dec << endl;
	cout << "index_size: \t" << dec << this->index_size << "\tbits\t| index_max: \t0x" << hex << this->index_max << dec << endl;
	cout << "tag_size:   \t" << dec << this->tag_size << "\tbits\t| tag_max:   \t0x" << hex << this->tag_max << dec << endl;
	cout << "offset_size:\t" << dec << this->offset_size << "\tbits\t| offset_max: \t0x" << hex << this->offset_max << dec << endl;
}

FACL::FACL(FileReader * reader){
	this->reader=reader; 
	this->tracker=Tracker();
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<CacheLine>();
	this->lru=vector<int>();
	unsigned long num_lines = this->numLines();
	for(unsigned int i=0; i<num_lines; i++){
		this->lines.push_back(CacheLine(i));
		this->lru.push_back(num_lines-i-1);
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

void FACL::setSizesAndMaxes(){
	this->index_max=1;
	this->index_size=0;
	/*unsigned int num_lines = this->numLines();
	while(this->index_max<num_lines){
		this->index_size++;
		this->index_max=this->index_max*2;
	}*/
	this->offset_size=0;
	for(unsigned int i=1; i<this->line_size; i*=2)
		this->offset_size++;
	this->offset_max=1;
	for(unsigned int i=1; i<=this->offset_size; i++)
		this->offset_max*=2;
	this->tag_size=32-this->index_size-this->offset_size;
	this->tag_max=1;
	for(unsigned int i=1; i<=this->tag_size; i++)
		this->tag_max=this->tag_max*2;
}

unsigned long FACL::maxAddress() const {
	unsigned long toReturn=1;
	for(unsigned int i=1; i<=this->line_size; i++)
		toReturn*=2;
	return toReturn;
}

double FACL::run(){
	if(!this->reader->isRead())
		this->reader->readFile();
	else
		this->reader->start();
	do{
		this->step();
	} while (this->reader->next());
	if(DEBUG){
		if(FINEDEB)
			cout << endl;
		this->printCache();
		this->printVars();
	}
	return this->tracker.percent();
}

bool FACL::step(){
	Line current = this->reader->current();
	unsigned long index;
	unsigned long tag = current.getAddress()>>(this->offset_size);
	bool hit=false;
	for(index=0; index<this->lines.size(); index++){
		if(this->lines[index].valid && this->lines[index].tag==tag){
			hit=true;
			break;
		}
	}
	if(hit){
		if(FINEDEB){
				cout << "Hit:  \t0x" << hex << current.getAddress() << "->" << dec;
				this->lines[index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%4==0){
					if(this->fdb_looper==20){
						this->fdb_looper=0;
						cout << endl;
					} else
						cout << "\n";
				}
			}
			if(this->lru[index]!=0){
				for(unsigned int i=0; i<this->lru.size(); i++){
					this->lru[i]++;
				}
				this->lru[index]=0;
			}
			this->tracker.addHit();
			return true;
	}
	if(FINEDEB){
		cout << "Miss: \t0x" << hex << current.getAddress() << dec << "\t";
		this->fdb_looper++;
		if(this->fdb_looper%4==0){
			if(this->fdb_looper==20){
				this->fdb_looper=0;
				cout << endl;
			} else
				cout << "\n";
		}
	}
	for(unsigned int i=0; i<this->lru.size(); i++){
		if(this->lru[i]>this->lru[index])
			index=i;
	}
	if(this->lru[index]!=0){
		for(unsigned int i=0; i<this->lru.size(); i++){
			this->lru[i]++;
		}
		this->lru[index]=0;
	}
	this->lines[index].tag=tag;
	this->lines[index].address=current.getAddress(); //used for debugging
	this->lines[index].valid=true;
	this->tracker.addMiss();
	return false;
}

void FACL::printCache(){
	for(unsigned long i=0; i<this->lines.size(); i++){
		cout << "0x" << hex << this->lines[i].address << ";   \t" << dec;
		if(i%16==15)
			cout << endl;
	}
	cout << endl;
}

void FACL::printVars(){
	cout << "cache_size: \t" << dec << this->cache_size << "\tkB\t| numLines():\t" << dec << this->numLines() << "\tlines\t" << dec << endl;
	cout << "--------------------------------+---------------------------" << endl;
	cout << "line_size:  \t" << dec << this->line_size << "\tbits\t| maxAddress:\t0x" << hex << this->maxAddress() << dec << endl;
	cout << "index_size: \t" << dec << this->index_size << "\tbits\t| index_max: \t0x" << hex << this->index_max << dec << endl;
	cout << "tag_size:   \t" << dec << this->tag_size << "\tbits\t| tag_max:   \t0x" << hex << this->tag_max << dec << endl;
	cout << "offset_size:\t" << dec << this->offset_size << "\tbits\t| offset_max: \t0x" << hex << this->offset_max << dec << endl;
}

FACH::FACH(FileReader * reader){
	this->reader=reader; 
	this->tracker=Tracker();
	this->hot=0;
	this->tag_max=1;
	this->tag_size=1;
	this->index_max=1;
	this->index_size=1;
	this->fdb_looper=0;
	this->setSizesAndMaxes();
	this->lines=vector<vector<CacheLine>>();
	this->lru=vector<vector<int>>();
	unsigned long num_lines = this->numLines();
	for(unsigned int i=0; i<2; i++){
		this->lines.push_back(vector<CacheLine>());
		this->lru.push_back(vector<int>());
		for(unsigned int j=0; j<num_lines/2; j++){
			this->lines[i].push_back(CacheLine(j*(i+1)));
			this->lru[i].push_back((num_lines/2)-j-1);
		}
		this->lines[i].shrink_to_fit();
		this->lru[i].shrink_to_fit();
	}
	this->lines.shrink_to_fit();
	this->lru.shrink_to_fit();
}

void FACH::setSizesAndMaxes(){
	this->index_max=1;
	this->index_size=0;
	/*unsigned int num_lines = this->numLines();
	while(this->index_max<num_lines){
		this->index_size++;
		this->index_max=this->index_max*2;
	}*/
	this->offset_size=0;
	for(unsigned int i=1; i<this->line_size; i*=2)
		this->offset_size++;
	this->offset_max=1;
	for(unsigned int i=1; i<=this->offset_size; i++)
		this->offset_max*=2;
	this->tag_size=32-this->index_size-this->offset_size;
	this->tag_max=1;
	for(unsigned int i=1; i<=this->tag_size; i++)
		this->tag_max=this->tag_max*2;
}

unsigned long FACH::maxAddress() const {
	unsigned long toReturn=1;
	for(unsigned int i=1; i<=this->line_size; i++)
		toReturn*=2;
	return toReturn;
}

double FACH::run(){
	if(!this->reader->isRead())
		this->reader->readFile();
	else
		this->reader->start();
	do{
		this->step();
	} while (this->reader->next());
	if(DEBUG){
		if(FINEDEB)
			cout << endl;
		this->printCache();
		this->printVars();
	}
	return this->tracker.percent();
}

bool FACH::step(){
	Line current = this->reader->current();
	unsigned long index;
	unsigned long tag = current.getAddress()>>(this->offset_size);
	bool hit=false;
	unsigned long inner_index;
	for(index=0; index<this->lines.size(); index++){
		for(inner_index=0; inner_index<this->lines[index].size(); inner_index++){
			if(this->lines[index][inner_index].valid && this->lines[index][inner_index].tag==tag){
				hit=true;
				break;
			}
		}
		if(hit)
			break;
	}
	if(hit){
		if(FINEDEB){
				cout << "Hit:  \t0x" << hex << current.getAddress() << "->" << dec;
				this->lines[index][inner_index].printLine();
				cout << "\t";
				this->fdb_looper++;
				if(this->fdb_looper%4==0){
					if(this->fdb_looper==20){
						this->fdb_looper=0;
						cout << endl;
					} else
						cout << "\n";
				}
			}
			if(this->lru[index][inner_index]!=0){
				for(unsigned int i=0; i<this->lru[index].size(); i++){
					this->lru[index][i]++;
				}
				this->lru[index][inner_index]=0;
			}
			this->hot=index;
			this->tracker.addHit();
			return true;
	}
	if(FINEDEB){
		cout << "Miss: \t0x" << hex << current.getAddress() << dec << "\t";
		this->fdb_looper++;
		if(this->fdb_looper%4==0){
			if(this->fdb_looper==20){
				this->fdb_looper=0;
				cout << endl;
			} else
				cout << "\n";
		}
	}
	inner_index=0;
	index=(this->hot+index)%2;
	for(unsigned int i=0; i<this->lru[index].size(); i++){
		if(this->lru[index][i]>this->lru[index][inner_index])
			inner_index=i;
	}
	if(this->lru[index][inner_index]!=0){
		for(unsigned int i=0; i<this->lru[index].size(); i++){
			this->lru[index][i]++;
		}
		this->lru[index][inner_index]=0;
	}
	this->lines[index][inner_index].tag=tag;
	this->lines[index][inner_index].address=current.getAddress(); //used for debugging
	this->lines[index][inner_index].valid=true;
	this->hot=index;
	this->tracker.addMiss();
	return false;
}

void FACH::printCache(){
	for(unsigned long i=0; i<this->lines.size(); i++){
		for(unsigned int j=0; j<this->lines[i].size(); j++)
			cout << "0x" << hex << this->lines[i][j].address << ";   \t" << dec;
		cout << endl;
	}
	cout << endl;
}

void FACH::printVars(){
	cout << "cache_size: \t" << dec << this->cache_size << "\tkB\t| numLines():\t" << dec << this->numLines() << "\tlines\t" << dec << endl;
	cout << "--------------------------------+---------------------------" << endl;
	cout << "line_size:  \t" << dec << this->line_size << "\tbits\t| maxAddress:\t0x" << hex << this->maxAddress() << dec << endl;
	cout << "index_size: \t" << dec << this->index_size << "\tbits\t| index_max: \t0x" << hex << this->index_max << dec << endl;
	cout << "tag_size:   \t" << dec << this->tag_size << "\tbits\t| tag_max:   \t0x" << hex << this->tag_max << dec << endl;
	cout << "offset_size:\t" << dec << this->offset_size << "\tbits\t| offset_max: \t0x" << hex << this->offset_max << dec << endl;
}

