P=cache-sim
CFLAGS= -g -Wall

all: $(P)

$(P): $(P).o
	g++ $(CFLAGS) $(P).o -o $(P)

$(P).o: $(P).cpp $(P).h
	g++ $(CFLAGS) -c $(P).cpp -o $(P).o

clear:
	clear
	clear
	make run
	
clearmem:
	clear
	clear
	make mem
	
cleargdb:
	git pull
	clear
	clear
	make all
	gdb --args ./$(P) trace1.txt output.txt
	
run: all
	rm -rf st1_output.txt
	./$(P) trace1.txt output.txt

mem: all
	rm -rf st1_output.txt
	valgrind ./$(P) trace1.txt output.txt 
	
	
checkmem:  all
	valgrind ./$(P)

clean:
	rm -rf $(P)
	rm -rf $(P).o
	
