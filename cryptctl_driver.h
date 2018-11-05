#ifndef CRYPTCTL_H
#define CRYPTCTL_H
#define CRYPTCTL_NAME "cryptctl"
#define CRYPTCTL_NAME_SIZE 8
#define CREATE_DEV_CODE 5001
#define DESTROY_DEV_CODE 5002
#define ENCRYPT_DEV_NAME  "cryptEncrypt"
#define DECRYPT_DEV_NAME "cryptDecrypt"
#define ENCRYPTCTL_PATH "/dev/cryptctl" 
#define DESTROY "d"
#define CREATE "c"
#define DEVICE_RECORDS_SIZE 128
#define FREE_RECORD -1
typedef struct
{
  char  free; //  1 if it's free, 0 otherwise.
  short device_id; // This could be used as the index for that device!!! This will also be used as the minor of the device
  short major;
  int encrypt_minor;
  int decrypt_minor;
  char encrypt_name[16]; // the user takes care of this
  char decrypt_name[16]; //the user takes care of this
  char key_stream[32];  //Size to be decided; the user takes care of this
  #ifdef KERNEL
  struct  cdev encrypt_device;
  struct  cdev decrypt_device;
  #endif
}device_record;

#endif
