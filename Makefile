# Définitions de macros
CC = gcc
CFLAGS = -c -Wall
LFLAGS = -pthread
CSRC = *.c
OBJS = pattern-finder.o occurences.o
EXEC = pattern-finder

# .PHONY run :pour dire que même s'il y a un fichier run,
# on en tient pas compte
.PHONY: run
run: $(EXEC)
	./$(EXEC) 2 lor lorem.txt

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

pattern-finder.o: pattern-finder.c *.h
	$(CC) $(CFLAGS) $<

%.o: %.c %.h
	$(CC) $(CFLAGS) $<

%.o: %.c
	$(CC) $(CFLAGS) $<


.PHONY: clean
clean:
	rm -f *.o
	rm -f $(EXEC)
	ls -l


DOSSIER = BEKHIRA_HADJER
.PHONY: zip
tar:
	rm -rf $(DOSSIER)
	rm -f $(DOSSIER).tar
	mkdir $(DOSSIER)
	cp -rf *.c *.h Makefile README.md *.txt test_basic.sh $(DOSSIER)
	tar cvf $(DOSSIER).tar $(DOSSIER)
	rm -rf $(DOSSIER)
	ls -l
