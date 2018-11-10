#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "cryptctl_driver.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
//#include "io_api.h"


//int write_unicode(char* buffer´, int file

int main(int argc, char** argv)
{
  
  if(argc<2)
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
  int new_id  = ioctl(control_file,CREATE_DEV_CODE, &my_encrypt);
  if( new_id < 0)
   {
      perror("ioctl");
      return -1;
   }
   // sleep(3);
   if( close(control_file)<0)
     {
       perror("close");
       return -1;
     }
   printf("A new device with id \" %d \" has been created.", new_id);
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
      //  sleep(3);
      if( close(control_file)<0)
	{
	  perror("close");
	  return -1;
	}
      printf("File closed. Done.\n");
    }
  else if(strcmp(command, ENCRYPT) == 0)
    {
      int id  = atoi(argv[2]);
      char dev_name1[1024];
      char dev_name2[1024];
      char input[3072];
      strcpy(dev_name1,ENCRYPT_DEV_PATH);
      strcpy(input, argv[3]);
      strcat(dev_name1, argv[2]);
      printf("opening this file: %s\n", dev_name1);
      int control_file = open(dev_name1, O_RDWR  | O_NONBLOCK);
      if(control_file< 0)
	{
	  perror("open");
	  return -1;
	}
      printf("open file: %d\n", control_file);
      if( ioctl(control_file,ENCRYPT_DEV_CODE, id) == -1)
	{
	  perror("ioctl");
	  close(control_file);
	  return -1;
	}
      //      sleep(3);
      if(write(control_file, input, strlen(input)) == -1 )
	{
	  perror("write");
	  close(control_file);
	  return -1;
	}
      if( close(control_file)<0)
	{
	  perror("close");
	  return -1;
	}
      //      printf("write output:%s\n", input);
      //      int i = 0;
      // while(i<strlen(input))
      //	{
      printf("Encrypted message: %s\n",input);
	  //  i++;
	  //	}
      printf("File closed. Done.\n");
      
    }
  else if(strcmp(command, DECRYPT) == 0)
    {
  int id  = atoi(argv[2]);
  char dev_name1[1024];
  char dev_name2[1024];
  char input[3072];
  strcpy(dev_name1,ENCRYPT_DEV_PATH);
  strcpy( input ,argv[3]);
  strcat(dev_name1, argv[2]);
  printf("opening this file: %s\n", dev_name1);
  int control_file = open(dev_name1, O_RDWR  | O_NONBLOCK);
  if(control_file< 0)
    {
      perror("open");
      return -1;
    }
  printf("open file: %d\n", control_file);
  if( ioctl(control_file,ENCRYPT_DEV_CODE, id) < 0)
    {
      perror("ioctl");
      close(control_file);
      return -1;
    }
  //      sleep(3);
  printf("input received:%s", input);
  if(read(control_file, input, strlen(input)) == -1 )
    {
      perror("write");
      close(control_file);
      return -1;
    }
  if( close(control_file)<0)
    {
      perror("close");
      return -1;
    }
  //  printf("write output:%s\n", input);
  //  int i =0;
  // while(i<strlen(input))
        printf("Decrypted message: %s\n",input);
	//  i++;
  printf("File closed. Done.\n");

}
  else if(strcmp(command, CHANGE_KEY) == 0)
    {
      int id  = atoi(argv[2]);
      device_record my_encrypt;
      my_encrypt.device_id = id;
      strcpy( my_encrypt.key_stream, argv[3]);
      int control_file = open( ENCRYPTCTL_PATH, O_RDWR );
      if(control_file< 0)
	{
	  perror("open");
	  return -1;
	}
      printf("open file: %d\n", control_file);
      if( ioctl(control_file,CHANGE_KEY_DEV_CODE, &my_encrypt) < 0)
	{
	  perror("ioctl");
	  return -1;
	}
      printf("key change done!\n");
      printf("New key: %s\n", my_encrypt.key_stream);
    }
  else if(strcmp(command, DOOM) == 0)
    {
      device_record my_encrypt;
      int control_file = open( ENCRYPTCTL_PATH, O_RDWR );
      if(control_file< 0)
	{
	  perror("open");
	  return -1;
	}
      if( ioctl(control_file,DOOM_DEV_CODE, &my_encrypt) < 0)
	{
	  perror("ioctl");
	  return -1;
	}
      printf("One day doom fell upon the world, and thus, all entities were wiped out.");
    }
  else if(strcmp(command, RENAME) == 0)
    {
      char new_dev_id[1024];
      char dev_name1[1024];
      char dev_name2[1024];
      strcpy(dev_name1,ENCRYPT_DEV_NAME );
      strcpy(dev_name2,DECRYPT_DEV_NAME );
      strcpy(new_dev_id, argv[3]);
      strcat(dev_name1, new_dev_id);
      strcat(dev_name2, new_dev_id);
      device_record my_encrypt;
      strcpy( my_encrypt.encrypt_name, dev_name1);
      strcpy( my_encrypt.decrypt_name, dev_name2);
      my_encrypt.device_id = atoi(new_dev_id);
      my_encrypt.old_device_id  = atoi(argv[2]);
      printf("device names %s, %s\n",my_encrypt.encrypt_name, my_encrypt.decrypt_name);
      int control_file = open( ENCRYPTCTL_PATH, O_RDWR );
      if(control_file< 0)
	{
	  perror("open");
	  return -1;
	}
      printf("open file: %d\n", control_file);
      int new_id  = ioctl(control_file,RENAME_DEV_CODE, &my_encrypt);
      if( new_id < 0)
	{
	  perror("ioctl");
	  return -1;
	}
      // sleep(3);
      if( close(control_file)<0)
	{
	  perror("close");
	  return -1;
	}
      printf("A new device with id \"%d\" has been created.\n", new_id);
      printf("Device with id \"%d\" has been deleted. ",  my_encrypt.old_device_id);
      printf("File closed. Done.\n");
    
    }
  return 0;
}
