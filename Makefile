CC	= gcc
CFLAGS	= -Wall

all:
	$(CC) $(CFLAGS) -c trafficlight.c -o trafficlight
	$(CC) $(CFLAGS) -c random.c -o random

clean:
	rm -f trafficlight random
