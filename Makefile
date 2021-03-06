SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mincc: $(OBJS)
	gcc -o $@ $^

OBJS: mincc.h

test: mincc ./test.sh
	./test.sh

clean:
	rm -f mincc tmp* *.o

.PHONY: test clean
