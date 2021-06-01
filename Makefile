CC=clang
OBJS=main.o tokenize.o parse.o codegen.o

mincc: $(OBJS)
	$(CC) -o $@ $^

$(OBJS): mincc.h

.PHONY: test
test: mincc
	./test.sh

.PHONY: clean
clean:
	rm -f mincc *.o tmp*
