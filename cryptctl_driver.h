#ifndef CRYPTCTL_H
#define CRYPTCTL_H
#define CRYPTCTL_NAME "cryptctl"
#define CRYPTCTL_NAME_SIZE 8
#define CREATE_DEV_CODE 5001
#define DESTROY_DEV_CODE 5002
#define ENCRYPT_DEV_CODE 5003
#define DECRYPT_DEV_CODE 5004
#define CHANGE_KEY_DEV_CODE 5005
#define ENCRYPT_DEV_NAME  "cryptEncrypt"
#define DECRYPT_DEV_NAME "cryptDecrypt"
#define ENCRYPTCTL_PATH "/dev/cryptctl"
#define ENCRYPT_DEV_PATH "/dev/cryptEncrypt"
#define  DECRYPT_DEV_PATH "/dev/cryptDecrypt"
#define DESTROY "d"
#define CREATE "c"
#define ENCRYPT "en"
#define DECRYPT "de"
#define CHANGE_KEY "ck"
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
