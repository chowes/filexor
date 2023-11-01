.PHONY: all filexor clean

all: filexor

filexor:
	gcc xor.c -Wall -O2 -o filexor
