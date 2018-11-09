#ifndef CRYPTCTL_H
#define CRYPTCTL_H
#define CRYPTCTL_NAME "cryptctl"
#define CRYPTCTL_NAME_SIZE 8
#define CREATE_DEV_CODE 5001
#define DESTROY_DEV_CODE 5002
#define ENCRYPT_DEV_CODE 5003
#define DECRYPT_DEV_CODE 5004
#define CHANGE_KEY_DEV_CODE 5005
#define RENAME_DEV_CODE 5006
#define DOOM_DEV_CODE 5007
#define DOES_EXIST 600
#define DOES_NOT_EXIST 6001
#define TABLE_CAPACITY_OVERFLOW 6002
#define ENCRYPT_DEV_NAME  "cryptEncrypt"
#define DECRYPT_DEV_NAME "cryptDecrypt"
#define ENCRYPTCTL_PATH "/dev/cryptctl"
#define ENCRYPT_DEV_PATH "/dev/cryptEncrypt"
#define  DECRYPT_DEV_PATH "/dev/cryptDecrypt"
#define DESTROY "d" // destroys a device pair
#define CREATE "c" // creates a device pair
#define ENCRYPT "en" //encrypts input using a device with a certain id
#define DECRYPT "de" //decrypts input using a device with a certain id
#define CHANGE_KEY "ck" //changes the key of a specific device pair
#define RENAME "re" //rename a device pair
#define DOOM "doom" //destroys all of the devices 
#define DEVICE_RECORDS_SIZE 128
#define FREE_RECORD -1
typedef struct
{
  char  free; //  1 if it's free, 0 otherwise.
  short old_device_id; //useful for sawpping operations, like renaming devices
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
