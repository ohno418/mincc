mincc: mincc.c
	gcc -o mincc mincc.c

clean:
	rm -f mincc

.PHONY: clean
