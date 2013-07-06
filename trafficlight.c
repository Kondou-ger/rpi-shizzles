#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void
all_output()
{
	int i;
	for (i=1; i<=30; i++)
	{
		bcm2835_gpio_fsel(i, BCM2835_GPIO_FSEL_OUTP);
	}
}

int
GPIO_on(int GPIOnum)
{
	bcm2835_gpio_write(GPIOnum, HIGH);
	return 0;
}
int
GPIO_off(int GPIOnum)
{
	bcm2835_gpio_write(GPIOnum, LOW);
	return 0;
}

volatile int run = 1;

void
loopsleep(int time)
{
	if (run == 1) {
		bcm2835_delay(time);
	}
}

void
shutdown(int signal)
{
	GPIO_off(7);
	GPIO_off(10);
	GPIO_off(14);
	GPIO_off(15);
	GPIO_off(27);
	run = 0;
}

int
main()
{
	signal(SIGINT, shutdown);
	if (!bcm2835_init())
		return 1;
	all_output();
	while (run == 1) {
		GPIO_on(7);
		loopsleep(10000);
		GPIO_on(10);
		loopsleep(1000);
		GPIO_off(7);
		GPIO_off(10);
		GPIO_on(27);
		loopsleep(10000);
		GPIO_off(27);
		GPIO_on(10);
		loopsleep(1000);
		GPIO_off(10);
	}
	bcm2835_close();
	return 0;
}
