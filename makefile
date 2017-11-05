
all: node1 node2

node1:
	$(MAKE) -C Node1/src/

node2:
	$(MAKE) -C Node2/src/


.PHONY:
clean:
	rm -f Node1/src/*.o Node1/src/*.out
	rm -f Node2/src/*.o Node2/src/*.out

