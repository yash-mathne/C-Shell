TARGETS = msnshell

.PHONY: clean

all: $(TARGETS)

%o: %.c
				gcc -w -g -c -o  $@ $<
msnshell: msnshell.o utilities.o builtins.o env.o jobs.o pinfo.o ls.o
				gcc -w -o  msnshell msnshell.o utilities.o builtins.o env.o jobs.o pinfo.o ls.o

clean:
	rm -f $(TARGETS) *.o
