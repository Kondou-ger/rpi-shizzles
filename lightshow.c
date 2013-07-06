#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
//#include <unistd.h>

void
all_output()
{
	int i;
	for (i=2; i<=27; i++)
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
	GPIO_off(11);
	GPIO_off(14);
	GPIO_off(15);
	GPIO_off(27);
	run = 0;
}

int program1counter = 0;
int program1memory = 0;

void
program1()
{
	if (program1counter == 10) {
		if (program1memory == 1) {
			GPIO_on(7);
			GPIO_off(11);
			program1memory = 0;
		} else if (program1memory == 0) {
			GPIO_on(11);
			GPIO_off(7);
			program1memory = 1;
		}
		program1counter = 0;
	} else {
		program1counter++;
	}
}

int program2counter = 0;
int program2memory = 0;

void
program2()
{
	if (program2counter == 3) {
		switch (program2memory) {
			case 0:
				GPIO_off(10);
				GPIO_on(27);
				program2memory++;
				break;
			case 1:
				GPIO_off(27);
				GPIO_on(15);
				program2memory++;
				break;
			case 2:
				GPIO_off(15);
				GPIO_on(14);
				program2memory++;
				break;
			case 3:
				GPIO_off(14);
				GPIO_on(15);
				program2memory++;
				break;
			case 4:
				GPIO_off(15);
				GPIO_on(27);
				program2memory++;
				break;
			case 5:
				GPIO_off(27);
				GPIO_on(10);
				program2memory = 0;
				break;
		}
		program2counter = 0;
	} else {
		program2counter++;
	}
}

int program3counter = 0;
int program3memory = 0;

void
program3()
{
	if (program3counter == 10) {
		if (program3memory == 0) {
			GPIO_on(7);
			GPIO_on(11);
			GPIO_on(10);
			GPIO_on(27);
			GPIO_on(15);
			GPIO_on(14);
			program3memory = 1;
		} else if (program3memory == 1) {
			GPIO_off(7);
			GPIO_off(11);
			GPIO_off(10);
			GPIO_off(27);
			GPIO_off(15);
			GPIO_off(14);
			program3memory = 0;
		}
		program3counter = 0;
	} else {
		program3counter++;
	}
}

int
main()
{
	int counter = 0;
	signal(SIGINT, shutdown);
	if (!bcm2835_init())
		return 1;

	all_output();
	while (run == 1) {
		if (counter < 200) {
			program1();
			program2();
		} else if (counter >= 200) {
			program3();
			if (counter == 300) {
				counter = 0;
			}
		}
		loopsleep(50);
		counter++;
	}
	bcm2835_close();
	return 0;
}
