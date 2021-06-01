CC=gcc

mincc: main.c
	${CC} -o $@ $^

.PHONY: test
test: mincc
	./test.sh

.PHONY: clean
clean:
	rm -f mincc tmp*
