# cs320_project2
The goal of this project is to measure the effectiveness of cache subsystem
organizations using traces of memory instructions obtained from the realistic programs.
Each trace contains about 1 million memory instructions with two values provided for
each instruction: a flag indicating whether this is a load or a store (L stands for a load, S
stands for a store), and the byte memory address targeted by this instruction. Three
traces are provided.

Submission for: 
- Michael Quinn (mquinn5; B00639358)

"Was interesting that being able to properly implement a functional storage type aided to the 
success of the project."

Submission requirements:

Please submit ONLY the following items, and pay close attention to naming.
Remove any trace files or test output files. Make sure your Makefile builds a
correctly named executable. Ensure that you have included an ASCII text file
named README (not README.txt just README, exactly like that no lowercase!)

You will need to submit your source code, so that we can compile it and test for
correctness. For checking your code, we will be using the same three traces that have
been provided to you, plus one more trace that you will not have access to.
The code that you submit should compile into a single executable called cache-sim
with a simple `make` command. This executable should run all of the caches on the
given trace, which will be specified via command line options as follows:

./cache-sim input_trace.txt output.txt


Submissions will be checked using a script that will compare your output file to the
correct output file using the UNIX `diff` tool, so if your output does not EXACTLY match
the correct output the grading program will mark it as wrong. I will have to check such
submissions by hand which will result in at least a few points being deducted.


Submission Rules:

You must submit all of the following:

1.) All source code
2.) A Makefile
3.) A README, which minimally contains the Name, BU-ID (everything before the @
in your Binghamton University e-mail) and B Number of each person in the
group. Other things to include might be: what works/what doesn&#39;t, things you
found interesting, etc.
