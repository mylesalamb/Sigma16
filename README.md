# Sigma16
An implementation of a Sigma16 emulator in C.

## What is Sigma16

Sigma16 is a computer architecture built exclusively for research and teaching in computer systems.
Sigma16 utilises a word size of 16 bits and a register file of 16 registers _R0 - R15_ however _R0_ 
consistently contains the value 0.

This program will initially provide an emulator with future plans to provide a gdbtui style debugger

## Compatability

This program targets x86_64 machines running GNU/Linux, however assumptions of hardware specific data types will be minimised as to ensure compatability between alternate architectures.
