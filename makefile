
all: node1 node2

node1:
	$(MAKE) -C Node1/src/

node2:
	$(MAKE) -C Node2/src/


.PHONY:
clean:
	make cleanup
	rm *.out

.PHONY:
cleanup:
	rm -f Node1/src/*.o
	rm -f Node2/src/*.o

