mincc: mincc.c
	gcc -o mincc mincc.c

test:
	./test.sh

clean:
	rm -f mincc tmp*

.PHONY: test clean
