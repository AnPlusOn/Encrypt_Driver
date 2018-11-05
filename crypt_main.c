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
  
  if(argc<3)
    {
      printf("Not enough arguments\n");
      return -1;
    }
  

  char command[1024];
  strcpy(command, argv[1]);
  if(strcmp(command, CREATE) ==  0)
    {
      if(argc<4)
	{
	  printf("Not enough arguments\n");
	  return -1;
	} 
  char dev_id[1024];
  char dev_name1[1024];
  char dev_name2[1024];
  char key[32];
  strcpy(dev_name1,ENCRYPT_DEV_NAME );
  strcpy(dev_name2,DECRYPT_DEV_NAME );
  strcpy(dev_id, argv[2]);
  strcpy(key, argv[3]);
  strcat(dev_name1, dev_id);
  strcat(dev_name2, dev_id);
  device_record my_encrypt;
  strcpy( my_encrypt.encrypt_name, dev_name1);
  strcpy( my_encrypt.decrypt_name, dev_name2);
  strcpy(my_encrypt.key_stream, key);
  my_encrypt.device_id = atoi(dev_id);
  
  printf("device names %s, %s\n",my_encrypt.encrypt_name, my_encrypt.decrypt_name);
   int control_file = open( ENCRYPTCTL_PATH, O_RDWR );
  if(control_file< 0)
    {
     perror("open");
     return -1;
    }
  printf("open file: %d\n", control_file);
   if( ioctl(control_file,CREATE_DEV_CODE, &my_encrypt) < 0)
   {
      perror("ioctl");
      return -1;
   }
   sleep(3);
   if( close(control_file)<0)
     {
       perror("close");
       return -1;
     }
   printf("File closed. Done.\n");
    }
  else if(strcmp(command, DESTROY) ==  0 )
    {
      if(argc<3 || argc>3)
	{
	  printf("Not the right amoount of arguments\n");
	  return -1;
	}
      int id  = atoi(argv[2]);
      device_record my_encrypt;
      my_encrypt.device_id = id;
      char dev_name1[1024];
      char dev_name2[1024];
      strcpy(dev_name1,ENCRYPT_DEV_NAME );
      strcpy(dev_name2,DECRYPT_DEV_NAME );
      strcat(dev_name1, argv[2]);
      strcat(dev_name2, argv[2]);
      int control_file = open( ENCRYPTCTL_PATH, O_RDWR );
      if(control_file< 0)
	{
	  perror("open");
	  return -1;
	}
      printf("open file: %d\n", control_file);
      if( ioctl(control_file,DESTROY_DEV_CODE, &my_encrypt) < 0)
	{
	  perror("ioctl");
	  return -1;
	}
      sleep(3);
      if( close(control_file)<0)
	{
	  perror("close");
	  return -1;
	}
      printf("File closed. Done.\n");
    }
   return 0;
  //  return 0;
}
