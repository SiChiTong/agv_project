#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <ctime>
#include <cstdint>
#include <iostream>       // std::cout
#include <thread>         // std::thread

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */


int open_port(void)
{
  int fd; /* File descriptor for the port */
  
  
  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
  if (fd == -1)
    {
      /*
       * Could not open the port.
       */
      
      perror("open_port: Unable to open /dev/ttyS0 - ");
    }
  else
    fcntl(fd, F_SETFL, 0);
  
  return (fd);
}

void config_port(int fd)
{
  struct termios options;
  
  /*
   * Get the current options for the port...
   */
  
  
  /*
   * Set the baud rates to 115200
   */
  
  /*
   * Enable the receiver and set local mode...
   */
  bzero(&options, sizeof(options));
 
  /*
   * Setting Parity Checking
   */
    
	
	options.c_cflag |= PARENB;
	options.c_cflag |= CS8;
	options.c_lflag = ICANON;     //process input as lines

	options.c_iflag &= ~ICRNL;
	options.c_cflag = options.c_cflag | CLOCAL | CREAD;
	options.c_oflag = 0;
	options.c_lflag = 0; /*ICANON;*/
	options.c_cc[VMIN]=1;
	options.c_cc[VTIME]=0;


	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	// activate new settings
	tcflush(fd, TCIFLUSH);

	tcsetattr(fd, TCSANOW, &options);
	//tcflush(fd, TCIOFLUSH);
}

void thread1(int fd) 
{
	int num;
	unsigned char buff[256];
	memset(&buff, '\0', sizeof(buff));
	
	tcflush(fd, TCIFLUSH);
	usleep(20000); //delay 1,75ms
	uint8_t analog_mode[8]= {0x01, 0x06, 0x10, 0xE3, 0x00, 0x01, 0xBD, 0x3C};
	write(fd, analog_mode, 8);
	

	usleep(20000); //delay 1,75ms
	uint8_t Min_speed[8]= {0x01, 0x06, 0x04, 0x81, 0x00, 0x50, 0xD8, 0xEE};
	write(fd, Min_speed, 8);

	usleep(20000); //delay 1,75ms
	uint8_t Write_stop[8]= {0x01, 0x06, 0x01, 0x8D, 0x00, 0x01, 0xD9, 0xDD};
	write(fd, Write_stop, 8);

	usleep(20000); //delay 1,75ms
	uint8_t Speed[8]= {0x01, 0x06, 0x04, 0x81, 0x01, 0x4F, 0x98, 0xB6};
	write(fd, Speed, 8);
	
	while(true)
	{
		uint8_t forward[8]= {0x01, 0x06, 0x00, 0x7D, 0x00, 0xA8, 0x18, 0x6C};
		num = 0;
		write(fd, forward, 8);
		
		while(1) 
		{
			num = read(fd,&buff,sizeof(buff));
			if(num) break;
		} 
		
		for(int i = 0; i < num; i++) {
			std::cout << std::hex << (int)buff[i] << " ";
		}
		std::cout << std::endl;
		//printf("Read %i bytes. Received message: %s", num, buff);
		
		usleep(20000); //delay 1,75ms
	}
	
  // do stuff...
}



int main(void)
{
	
	int fd;
	fd = open_port();
	config_port(fd);
	
	std::thread first(thread1,fd); 
	//std::thread second(thread2,fd); 
	
	first.join();                // pauses until first finishes
	//second.join();  
	close(fd);
	return 0;
}


