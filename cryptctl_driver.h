#ifndef CRYPTCTL_H
#define CRYPTCTL_H
#define CRYPTCTL_NAME "cryptctl"
#define CREATE_DEV_CODE 5001
#define DESTROY_DEV_CODE 5002
#define ENCRYPT_DEV_NAME  "cryptEncrypt"
#define DECRYPT_DEV_NAME "cryptDecrypt"
#define ENCRYPTCTL_PATH "/dev/cryptctl" 

typedef  struct
{
  char encrypt_name[1024];
  char decrypt_name[1024];
  char key[1024];
} encryptctl_struct;
#endif
