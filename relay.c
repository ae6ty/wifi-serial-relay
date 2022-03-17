//#define TERMINAL    "/dev/tty.usbserial-A7012VK5"
#define TERMINAL    "/dev/ttyACM0"
#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

char error[1000] = "\0";

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
      fprintf(stderr,"Error from tcgetattr: %s\n", strerror(errno));
      return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;
    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int fd;
int relay_write(char *buf, int len) {
  return write(fd,buf,len);
}
int relay_setup()
{
    char *portname = TERMINAL;
    int wlen;
    char *xstr = "Hello!\n";
    int xlen = strlen(xstr);

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
      fprintf(stderr,"Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */

    return set_interface_attribs(fd, B115200);
}
int relay_receive( int write(char *s, int len)) {
    fprintf(stderr,"starting relay receive\n");
  do {
    char buf[1];
    int rdlen;
    rdlen = read(fd, buf, sizeof(buf));
    //    fprintf(stderr,"relay received %d\n",buf[0]); fflush(stderr);
    if (rdlen > 0) {
      write(buf,1);
    }
  } while (1);
}

#ifdef TESTRELAY
int main(int ac, char **av) {
  relay_setup();
  relay_receive(relay_write);
}
#endif

#ifdef TESTWHOLE
extern int server_receive(int write(char *s, int len), char *msg);
extern int server_write(char *s, int len);

void *serverThread(void *foo) {
  server_receive(relay_write,error);
  return NULL;
}
pthread_t thread;
int main(int ac, char **av) {
  int success = relay_setup();
  if (success) {
    sprintf(error,"couldn't set up serial port %s\n",TERMINAL);
    fprintf(stderr,"ERROR: %s",error);
  }
  pthread_create(&thread,NULL, serverThread,NULL);
  relay_receive(server_write);
}
#endif
