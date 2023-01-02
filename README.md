# Out-Of-Order-Pipeline
Simulates a 32 bit instruction set pipeline for the RISC V architecture. 

To run the code, run the following commands on the terminal:
1. make
2. make asm
3. asm simple.s simple.bin (Replace simple.s with the relevant assembly file)
4. rvsim -b simple.bin -o ooo_pipe.fu -wbpi 2 -wbpf 2 (Replace the .bin file with the relevant binary file, wbpi and wbpf are integer and floating point wb ports, they can be either 1 or 2.)
