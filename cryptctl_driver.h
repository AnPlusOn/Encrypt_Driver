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

typedef  struct
{
  char encrypt_name[512];
  char decrypt_name[512];
  int  encrypt_name_size;
  int decrypt_name_size;
  int Major;
  char key[512];
} encryptctl_struct;
#endif
