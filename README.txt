In the folder you should find the following four files: bpt.h bpt.cpp cbpt.cpp dbio.cpp
The source code is written in C++, and can be compiled by setting the .cpp files as target.
e.g. running "g++ -o bpt dbio.cpp bpt.cpp cbpt.cpp" under a Linux shell to get executable file "bpt".
For simplicity a Makefile is included, which does exactly that.

The executable file can be run as is, like so: ($ represents the shell)

$ ./bpt

It will take file "ProjectB_data.txt" as input and output to file "log.txt".
Another way to use it is to run it like so:

$ ./bpt ProjectB_data.txt log.txt

The first argument to the file is the input, while the second is the output file.
You can supply your own filenames for it.
There is one last way to execute it, which is also an exception to the previous way:

$ ./bpt ProjectB_data.txt stdout

By putting "stdout" as second argument, the file outputs to terminal instead.
Yes, this means you can't specify "stdout" as your output file name.

One last feature: in the dbio.cpp file, if you modify the code by removing "//" before the eleventh line,

//#define DBIO_LOG_OUTPUT

The output will now become a lot more verbose, and print out something everytime it finishes a R, I, or D instruction.
It is commented-out by default since it is very verbose.
