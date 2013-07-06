#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PAGE_SIZE		(4*1024)
#define BLOCK_SIZE		(4*1024)

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

void setup_io();

void all_output();

int GPIO_on(int GPIOnum);
int GPIO_off(int GPIOnum);

//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
	printf("can't open /dev/mem \n");
	exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
	NULL,             //Any adddress in our space will do
	BLOCK_SIZE,       //Map length
	PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
	MAP_SHARED,       //Shared with other processes
	mem_fd,           //File to map
	GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
	printf("mmap error %d\n", (int)gpio_map);//errno also set!
	exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
} // setup_io

void
all_output()
{
	int i;
	for (i=1; i<=30; i++)
	{
		INP_GPIO(i);
		OUT_GPIO(i);
	}
}

int
GPIO_on(int GPIOnum)
{
	GPIO_SET = 1<<GPIOnum;
	return 0;
}
int
GPIO_off(int GPIOnum)
{
	GPIO_CLR = 1<<GPIOnum;
	return 0;
}

volatile int run = 1;

void
loopsleep(int time)
{
	if (run == 1) {
		struct timespec t;
		t.tv_sec = 0;
		t.tv_nsec = time*1000000;
		nanosleep(&t, NULL);
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
	srand(time(NULL));
	setup_io();
	all_output();
	int led, GPIO;
	while (run == 1) {
		led = rand() % 4;
		if (led == 0) {
			GPIO = 7;
		} else if (led == 1) {
			GPIO = 10;
		} else if (led == 2) {
			GPIO = 14;
		} else if (led == 3) {
			GPIO = 15;
		} else if (led == 4) {
			GPIO = 27;
		}

		GPIO_on(GPIO);
		loopsleep(100);
		GPIO_off(GPIO);
	}
	return 0;
}
