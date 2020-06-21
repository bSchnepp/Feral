include ../Makerules

all:	test_cases

.PHONY:	all clean

test_cases:
	$(CC) $(CFLAGS) -I./inc -I../inc -O0 -g3 ./*.c
	cp ../*.o .
	rm -rf init*.o stackcheck.o
	$(CC) ./*.o -o ./test_cases
	./test_cases

clean:
	rm -rf ./*.o ./test_cases

