#include "syscall.h"
#include "types.h"
#include "fcntl.h"
#include "ns_types.h"
#include "user.h"

/* Verify init
  - verify the creation of tty devices
  - on the init we are able to open all tty devices
  - make sure no tty devices in use, e.g. make sure pouch containers occupy no tty device.
  - no re-run tests afterwards
*/
int init_tests() {
  int i;
  int tty_fd;
  char tty[] = "/ttyX";

  for(i = 0; i<3; i++){
    tty[4] = '0' + i;
    if((tty_fd = open(tty, O_RDWR))<0){
      printf(stderr, "failed to open %s\n",tty);
      return -1;
    }
    close(tty_fd);
  }
  return 0;
}


/* Verify ioctl syscall attach/detatch
   verify TTYGETS/TTY_SETS operations on attaching and detatching a tty device

   EXAMPLE: if given tty is not attached, TTY_GET/DEV_ATTACH will return "0"
   but TTY_GET/DEV_DETATCH will return "1", when we attach the given tty
   using TTY_SET/DEV_ATTACH : TTY_GET/DEV_ATTACH will return "1" and TTY_GET/DEV_DETATCH
   will return "0".
   TTYSETS return "0" on operation success.
*/
int ioctl_attach_detach_test() {
  int tty_fd;
  char * tty_name = "tty0";

  if((tty_fd = open(tty_name, O_RDWR)) < 0){
    printf(stderr, "failed to open %s\n",tty_name);
    return -1;
  }

  // pre condition, verify tty is not attached
  if(ioctl(tty_fd, TTYGETS, DEV_ATTACH) != 0){
      printf(stderr, "step 1. %s atach precondition failed  \n",tty_name);
      goto fail;
  }

  // test - attach tty
  if(ioctl(tty_fd, TTYSETS, DEV_ATTACH) < 0){
    printf(stderr, "step 2. %s failed TTYSETS & DEV_ATTACH \n",tty_name);
    goto fail;
  }

  int status = ioctl(tty_fd, TTYGETS, DEV_ATTACH);

  if(status == 0){
    printf(stderr, "step 3. %s failed TTYGETS & DEV_ATTACH, expected: >0 \n",tty_name);
    goto fail;
  }
  else if(status == -1){
    printf(stderr, "step 3. ioctl failed");
    goto fail;
  }
  else if(status == 1){

    if(ioctl(tty_fd, TTYSETS, DEV_DETACH) == 0){
      printf(stderr, "%s detached \n",tty_name);
    }else{
      printf(stderr, "step 3. %s detach failed \n",tty_name);
      goto fail;
    }

    status = ioctl(tty_fd, TTYGETS, DEV_DETACH);

    if(status == 0){
      close(tty_fd);
      return 0;
    }
    else if(status == -1){
      printf(stderr, "step 3. ioctl GETS & DETACH failed");
      goto fail;
    }
  }
fail:
  close(tty_fd);
  return -1;
}

/* Verify ioctl syscall connect/disconnect
   verify TTYGETS/TTY_SETS operations on connecting and disconnecting a tty device

   EXAMPLE: if given tty is not connected, TTY_GET/DEV_CONNECT will return "0"
   but TTY_GET/DEV_DISCONNECT will return "1", when we connect the given tty
   using TTY_SET/DEV_CONNECT : TTY_GET/DEV_CONNECT will return "1" and TTY_GET/DEV_DISCONNECT
   will return "0".
   TTYSETS return "0" on operation success.
*/
int ioctl_connect_disconnect_test() {
  int tty_fd;
  char * tty_name = "tty0";

  if((tty_fd = open(tty_name, O_RDWR)) < 0){
    printf(stderr, "failed to open %s\n",tty_name);
  }

  // pre condition, verify tty is not connected
  if(ioctl(tty_fd, TTYGETS, DEV_CONNECT) != 0){
      printf(stderr, "step 1. %s connect precondition failed  \n",tty_name);
      goto fail;
  }

  // test connect
  if(ioctl(tty_fd, TTYSETS, DEV_CONNECT) < 0){
    printf(stderr, "step 2. %s failed TTYSETS & DEV_CONNECT \n",tty_name);
    goto fail;
  }

  int status = ioctl(tty_fd, TTYGETS, DEV_CONNECT);

  if(ioctl(tty_fd, TTYSETS, DEV_DISCONNECT) == 0){
      printf(stderr, "%s disconnected \n",tty_name);
      if(ioctl(tty_fd, TTYSETS, DEV_DETACH) == 0){
        printf(stderr, "%s detatched \n",tty_name);
      }else{
          goto fail;
      }
  }else{
      goto fail;
  }

  if(status == 0){
    printf(stderr, "step 3. %s failed TTYGETS & DEV_CONNECT, expected: > 0 \n",tty_name);
    goto fail;
  }
  else if(status == -1){
    printf(stderr, "step 3. ioctl failed");
    goto fail;
  }
  else if(status == 1){
    close(tty_fd);
    return 0;
  }
fail:
  close(tty_fd);
  return -1;
}

/* Verify that any attatch/connect operations to the console device fails
  - console should not be used as a tty for a container.
*/
int ioctl_console_test() {
  int tty_fd = 1; //CONSOLE
  char * tty_name = "console";


  if( ioctl(tty_fd, TTYGETS, DEV_CONNECT) == 0){
    printf(stderr, " %s  connected TTYGETS / DEV_CONNECT, expect: disconnected \n",tty_name);
    return -1;
  }

  if(ioctl(tty_fd, TTYGETS, DEV_ATTACH)  == 0){
    printf(stderr, " %s  attached TTYGETS / DEV_ATTACH, expect: detached \n",tty_name);
    return -1;
  }

  if(ioctl(tty_fd, TTYSETS, DEV_ATTACH)  == 0){
    printf(stderr, " %s  attached TTYSETS / DEV_ATTACH, expect: -1 \n",tty_name);
    return -1;
  }

  if( ioctl(tty_fd, TTYSETS, DEV_CONNECT) == 0){
    printf(stderr, " %s  connected TTYSETS / DEV_CONNECT, expect: -1 \n",tty_name);
    return -1;
  }
  return 0;
}

/* Verify wrong device usage
  - only specified devices created by init (tty0-2) can be used as ttys
  - mknod()is xv6 syscall to create a device, user should specify minor and major numbers
*/
int ioctl_wrong_device_test() {
  int tty_fd;
  char * tty_name = "tty_test";

  if(mknod(tty_name,1,5) < 0){
    printf(stderr, "failed to create test device %s\n",tty_name);
    return -1;
  }

  if((tty_fd = open(tty_name, O_RDWR)) < 0){
    printf(stderr, "failed to open %s\n",tty_name);
    return -1;
  }

  int status = ioctl(tty_fd, TTYGETS, DEV_ATTACH);

  unlink(tty_name);

  if (status == 0){
    printf(stderr, " %s device passed verification? - wrong! Expected: -1 \n",tty_name);
    goto fail;
  }
  else if(status == 1){
    printf(stderr, " %s return status: %d - wrong! Expected: -1 \n",tty_name, status);
    goto fail;
  }else if(status == -1){
    close(tty_fd);
    return 0;
  }

fail:
  close(tty_fd);
  return -1;
}

int main() {

  //TTY INIT TESTS
  if(init_tests() < 0)
    exit(1);

  //ioctl TESTS
  if(ioctl_wrong_device_test() < 0)
    exit(1);

  if(ioctl_console_test() < 0)
    exit(1);

  //ioctl SCENARIO TESTS
  if(ioctl_attach_detach_test() < 0)
    exit(1);

  if(ioctl_connect_disconnect_test() < 0)
    exit(1);

  printf(stderr, "ioctl TESTS PASS:\n");
  exit(0);
}

