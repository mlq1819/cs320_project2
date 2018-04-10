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
	FileReader reader = FileReader(file);
	cout << "Reading from" << argv[1] << endl;
	if(!reader.readFile()){
		cout << "Failed to read file" << endl;
		return false;
	}
	if(OUTPUT)
		cout << "Outputting to " << argv[2] << endl;
	
	return 0;
}

Line::Line(bool isStore, unsigned long address){
	this->isStore=isStore;
	this->address=address;
}

Line::Line(string line){
	this->isStore=(line[0]=='S');
	this->address=stol(str.substr(4,12), 0, 16);
}

FileReader::FileReader(ifstream file){
	this->file=file;
	this->read=false;
	this->lines=vector<Line>();
}

FileReader::FileReader(const FileReader & reader){
	this->file=ifstream(reader.getFile());
	this->lines=vector<Line>(reader.getLines());
}

bool FileReader::readFile(){
	try{
		if(this->file.isOpen){
			this->file.clear();
			this->file.seekg(0, ios_base::beg);
		} else
			this->file.open();
		string str;
		cout << "Reading file..." << endl;
		while(getline(*file, str))
			this->lines.push_back(Line(str));
		cout << "File read" << endl;
		this->lines.shrink_to_fit();
		this->file.close;
	} catch (exception e){
		return false;
	}
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

unsigned int DMC::tag_size(){
	unsigned int num=1;
	int tag_size=0;
	unsigned int num_lines = this->numLines();
	while(num<num_lines){
		tag_size++;
		num << 1;
	}
	return tag_size;
}