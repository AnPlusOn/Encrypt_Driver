#include <linux/string.h>
//#include <stdlib.h>
int vcEncrypt(char* message, char* key){
  int msize = strlen(message);
  int ksize = strlen(key);
  char msgcpy[msize];
  strcpy(msgcpy, message);

  //char* ans = malloc((msize+1) * sizeof(char));
  int i = 0;
  for (i = 0; i < msize; i++) {
    message[i] = msgcpy[i] + key[i%ksize];

  }
  return 10;
}

int vcDecrypt(char* message, char* key){
  int msize = strlen(message);
  int ksize = strlen(key);
  char msgcpy[msize];
  strcpy(msgcpy, message);

  //char* ans = malloc((msize+1) * sizeof(char));
  int i = 0;
  for (i = 0; i < msize; i++) {
    message[i] = msgcpy[i] - key[i%ksize];

  }
  return 10;
}



int main(){
  char message[1024];
  strcpy(message, "Brains");
  printf("%s\n", message );
  char* key = "\n";
  vcEncrypt(message,key);
  printf("encrypted - %s\n", message);
  vcDecrypt(message,key);
  printf("%s\n", message );
  //printf("%s -> %s -> %s using %s \n", message, enc, dec, key);
}
