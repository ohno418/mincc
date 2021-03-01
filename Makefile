SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mincc: $(OBJS)
	gcc -o $@ $^

OBJS: mincc.h

test:
	./test.sh

clean:
	rm -f mincc tmp* *.o

.PHONY: test clean
