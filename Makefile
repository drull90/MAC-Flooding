
EXES	:= main.o ethernet.o

all: $(EXES)
	gcc -o run main.o ethernetFrame.o

main.o: main.c ethernetFrame.c
	gcc -c main.c

ethernet.o: ethernetFrame.c ethernetFrame.h
	gcc -c ethernetFrame.c

clean:
	rm -rf run *.o