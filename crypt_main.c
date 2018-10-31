#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "cryptctl_driver.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>


int main(int argc, char** argv)
{
  char dev_id[1024];
  char dev_name1[1024];
  char dev_name2[1024];
  strcpy(dev_name1,ENCRYPT_DEV_NAME );
  strcpy(dev_name2,DECRYPT_DEV_NAME );
  strcpy(dev_id, argv[1]);
  strcat(dev_name1, dev_id);
  strcat(dev_name2, dev_id);
  char key[1024];
  encryptctl_struct my_encrypt = 
  {
    .encrypt_name =  dev_name1,
      .decrypt_name = dev_name2,
      .key = 0
  };

  int control_file =open( ENCRYPTCTL_PATH, O_RDWR );
  if(control_file< 0)
    {
      perror("open");
      return -1;
    }
  
  if( ioctl(control_file,CREATE_DEV_CODE, 0 ) < 0)
    {
      perror("ioctl");
      return -1;
    }
  
  close(control_file);
  return 0;
}
