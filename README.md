### ASSEMBLER AND LINKER

##SINGLE-PASS ASSEMBLER WRITTEN IN C++
This script supports up to four assembly files.
The assembler will generate intermediate assembly files after preprocess stage it will also generate obj files for each inputed file.
The linker will generate one executable with the same name of the first input file.

#HOW TO RUN

```
g++ -o assembler assembler.cpp
./assembler file1 file2 file3 file4

```

#SUPPORTED ASSEMBLY INSTRUCTIONS
![SUPPORTED ASSEMBLY INSTRUCTIONS](/instructions.png)