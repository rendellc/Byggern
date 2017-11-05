

all: node1.out node2.out


node1.out:
	make -f ./Node1/src/makefile

node2.out:
	make -f ./Node1/src/makefile