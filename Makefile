CC	= gcc
CFLAGS	= -Wall

all:
	$(CC) $(CFLAGS) trafficlight.c -o trafficlight -l bcm2835
	$(CC) $(CFLAGS) random.c -o random -l bcm2835
	$(CC) $(CFLAGS) lightshow.c -o lightshow -l bcm2835

clean:
	rm -f trafficlight random lightshow
