all:
	clear
# handle ifthe bin does not exists
	rm -rf bin
	mkdir -p bin
	gcc src/*.c app/transmitter.c -Wall -O2 -Iinc -o bin/transmitter
	gcc src/*.c app/receiver.c    -Wall -O2 -Iinc -o bin/receiver