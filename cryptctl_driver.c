#include <linux/init.h>
#include <linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/vmalloc.h>
#include<linux/string.h>
#include "cryptctl_driver.h" 

//#include<stdio.h>
//#inc
static int encrypt_open(struct inode *, struct file *);
static int create_char_dev(unsigned int , const char* , const struct file_operations* );
static int destroy_char_dev(unsigned int, const char* );
static long char_dev_ctl(struct file *, unsigned int, unsigned long );
int create_pair(device_record*,const struct file_operations* );
static int Major;
static int Pair_Major = 0;
static int open_count = 0;
static device_record* device_table = NULL;
MODULE_LICENSE("Dual BSD/GPL");
static const struct file_operations fops =
  {
    .owner = THIS_MODULE,
    //    .open  = encrypt_open,
    .unlocked_ioctl = char_dev_ctl, //(struct file * open_file, unsigned int request, unsigned long dev_info),
    //    .write = encrypt 
  };
//static int encrypt()
//{
  //encryption code
//}
static int encrypt_open(struct inode * file_data, struct file * open_file)
{
  return 0;
}
static long char_dev_ctl(struct file * open_file, unsigned int request, unsigned long dev_info)
{
  printk("kernel control running");
  
  if (request == 0 || dev_info == 0)
    {
      printk("Process attempting to acess address 0. Don't try to kill the kernel, please!");
      return -1;
    }
  device_record  user_dev_info;
  printk("sizeof:%d\n",sizeof(device_record) );
   int bytes_read =   copy_from_user( &user_dev_info,(const device_record*) dev_info, sizeof(device_record) );
   // while(bytes_read<sizeof(encryptctl_struct))
   // {
       //  bytes_read +=   copy_from_user( &user_dev_info,(const encryptctl_struct*) dev_info, sizeof(encryptctl_struct) );
       // }
   printk("copy_from_user: %s", user_dev_info.decrypt_name);
  switch(request)
    {
    case CREATE_DEV_CODE:
      printk("ioctl LIVES!");
      //      create_char_dev(0,user_dev_info.encrypt_name, &fops);
      // create_char_dev(0,user_dev_info.decrypt_name, &fops);
      create_pair(&user_dev_info, &fops);
      break;
    case DESTROY_DEV_CODE:
      destroy_char_dev(user_dev_info.major ,user_dev_info.decrypt_name);
      destroy_char_dev(user_dev_info.major ,user_dev_info.encrypt_name);
      break;
    default:
      printk("Invalid request for cryptctl");
      break;
    }
  printk("ioctl is done!");
  return 0;
}



static  void init_device_table(void)
{
  int i  = 0;
  device_table  = (device_record*) vmalloc(sizeof(device_record) * DEVICE_RECORDS_SIZE);
  if (device_table == NULL)
    {
      printk("vamalloc() messed up, sorry.");
      return -1;;
    }
  while(i<DEVICE_RECORDS_SIZE)
    {
      device_table[i].device_id =  i;
      device_table[i].free =  1;
      i++;
    }
  return;
}
int init_module(void)
{
  init_device_table();
  Major =  create_char_dev(0,CRYPTCTL_NAME, &fops);
 if(Major<0)
   {
     printk("There was an error registering this device\n");
     return -1;
   }
 printk("I think the device should be registered now:%d\n", Major);
 return 0;
}
void cleanup_module(void)
{
  unregister_chrdev(Major, CRYPTCTL_NAME);
  printk("Device was unregustered. See ya later alligator");
  return 0;
}

int create_pair(device_record* pair_info,const struct file_operations* fops )
{
  create_char_dev(Pair_Major,pair_info->encrypt_name, fops );
  create_char_dev(Pair_Major,pair_info->decrypt_name, fops );
  int  dev_id = pair_info->device_id;
  memcpy(&(device_table[dev_id]), pair_info, sizeof(device_record));
  device_table[dev_id].free = 0;
  // device_table[dev_id].major = Pair_Major;
  // strcpy( device_table[dev_id].encrypt_name, pair_info->encrypt_name;
  // device_table[dev_id].decrypt_name = pair_info->decrypt_name;
  // device_table[dev_id].key_stream = pair_info->key_stream;
  printk("pair:(%d,%s,%s,%s)",device_table[dev_id].major, device_table[dev_id].encrypt_name,device_table[dev_id].decrypt_name,device_table[dev_id].key_stream  );
  return 0;
}
int destroy_pair(device_record* pair_info)
{
  
}
static int create_char_dev(unsigned int major, const char* dev_name, const struct  file_operations* fops)
{
 Major =  register_chrdev(major,dev_name, fops);
 if(Major<0)
   {
     printk("There was an error registering this device. Please try again.\n");
     return Major;
   }
 printk("New Registered device:(%s, %d)", dev_name, Major);
 return Major;
}
static int destroy_char_dev(unsigned int major, const char* dev_name)
{
  // char device_name[1024];
  // copy_from_user(device_name, dev_name, dev_name_size);
    unregister_chrdev(Major, dev_name);
  //  if(ret<0)
  // {
      //  printk("There was a problem unregistering the device");
      return -1;
      // }
  printk("Device was successfully destroyed");
  return 0;
}

//module_init(char_dev_ctl);
//module_exit(hello_exit);
//printf("Hi Kernel\n");
