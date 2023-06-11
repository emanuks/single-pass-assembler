# ASSEMBLER AND LINKER

## SINGLE-PASS ASSEMBLER WRITTEN IN C++

This script supports up to four assembly files.

The assembler will generate intermediate assembly files after preprocess stage it will also generate obj files for each inputed file.

The linker will generate one executable with the same name of the first input file.

### HOW TO RUN

```
g++ -o assembler assembler.cpp
./assembler file1 file2 file3 file4
g++ -o linker linker.cpp
./linker file1 file2 file3 file4
```

### SUPPORTED ASSEMBLY INSTRUCTIONS

These are the supported assembly instructions

![SUPPORTED ASSEMBLY INSTRUCTIONS](/instructions.png)

It also supports 
 - CONST **NUM** the const value can be decimal positive, decimal negative and unsigned hexadecimal. 
 - SPACE **QUANTITY**.
 - BEGIN.
 - END.
 - EXTERN.
 - PUBLIC.

Take a look at MOD_A.asm and MOD_B.asm files for examples of how to use it.