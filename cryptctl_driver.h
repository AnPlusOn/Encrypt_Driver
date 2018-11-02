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
#define DEVICE_RECORDS_SIZE 1024
#define FREE_RECORD -1
typedef struct
{
  char  free; //  1 if it's free, 0 otherwise.
  short device_id; // This could be used as the index for that device!!! This will also be used as the minor of the device
  short major;
  char encrypt_name[16];
  char decrypt_name[16];
  char key_stream[32];  //Size to be decided
}device_record;

#endif
