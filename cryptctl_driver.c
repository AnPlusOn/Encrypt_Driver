#include <linux/init.h>
#include <linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include "cryptctl_driver.h" 
//#include<stdio.h>
//#inc
static int encrypt_open(struct inode *, struct file *);
static int create_char_dev(unsigned int , const char* , const struct file_operations* );
static int destroy_char_dev(unsigned int, const char* );
static long char_dev_ctl(struct file *, unsigned int, unsigned long );
static int Major;
static int open_count = 0;
static char paired_devices[36][256];
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
  const encryptctl_struct user_dev_info[sizeof(encryptctl_struct)];
   int bytes_read =   copy_from_user( &user_dev_info,(const encryptctl_struct*) dev_info, sizeof(encryptctl_struct) );
   while(bytes_read<sizeof(encryptctl_struct))
     {
       bytes_read +=   copy_from_user( &user_dev_info,(const encryptctl_struct*) dev_info, sizeof(encryptctl_struct) );
     }
   printk("copy_from_user: %d", bytes_read);
  switch(request)
    {
    case CREATE_DEV_CODE:
      printk("ioctl LIVES!");
      create_char_dev(0,user_dev_info->encrypt_name, &fops);
      create_char_dev(0,user_dev_info->decrypt_name, &fops);
      break;
    case DESTROY_DEV_CODE:
      destroy_char_dev(user_dev_info->Major ,user_dev_info->decrypt_name);
      destroy_char_dev(user_dev_info->Major ,user_dev_info->encrypt_name);
      break;
    default:
      printk("Invalid request for cryptctl");
      break;
    }
  printk("ioctl is done!");
  return 0;
}

int init_module(void)
{
  Major =  register_chrdev(0,CRYPTCTL_NAME, &fops);
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
static int  create_char_dev(unsigned int major, const char* dev_name, const struct  file_operations* fops)
{
  // char device_name[1024];
  //copy_from_user(device_name, dev_name, dev_name_size);
  //  device_name[dev_name_size] = '\0';
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
  //  device_name[dev_name_size] = '\0';
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
