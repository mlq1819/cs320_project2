#include "cache-sim.h"
#define FORCE false
#define OUTPUT true


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
	
	cout << "Reading from" << argv[1] << endl;
	if(OUTPUT)
		cout << "Outputting to " << argv[2] << endl;
	
	return 0;
}
