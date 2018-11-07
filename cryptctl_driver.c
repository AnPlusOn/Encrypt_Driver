#include <linux/init.h>
#include <linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/vmalloc.h>
#include<linux/string.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include "kernel_space.h"
#include "cryptctl_driver.h"

//#include<stdio.h>
//#inc
static int encrypt_open(struct inode *, struct file *);
static int create_char_dev(unsigned int ,unsigned int , const char* ,struct cdev*, const struct  file_operations* );
static int destroy_char_dev( unsigned int, unsigned int , struct cdev* );
static long char_dev_ctl(struct file *, unsigned int, unsigned long );
static int create_pair(device_record*,const struct file_operations* );
static long encrypt_dev_ctl(struct file *, unsigned int, unsigned long);
static  int destroy_pair(device_record*);
static ssize_t encrypt(struct file *, const char __user  *, size_t, loff_t*);
static int driver_major  = 0;
static int driver_minor  = 0;
static int Pair_Major = 0;
static int open_count = 0;
static char* current_key = NULL;
static dev_t cryptctl_dev = 0;
static struct class* crypt_class;
static struct cdev cryptctl;
static device_record* device_table = NULL;
MODULE_LICENSE("Dual BSD/GPL");
static const struct file_operations fops =
  {
    .owner = THIS_MODULE,
    //    .open  = encrypt_open,
    .unlocked_ioctl = char_dev_ctl //(struct file * open_file, unsigned int request, unsigned long dev_info)
  };
static const struct file_operations fops_encrypt =
  {
    .owner = THIS_MODULE,
     .write = encrypt,
    .unlocked_ioctl = encrypt_dev_ctl //(struct file * open_file, unsigned int request, unsigned long dev_info)
  };
static long encrypt_dev_ctl(struct file* open_file, unsigned int request, unsigned long dev_id)
{
  
  if (request == 0 || dev_id <0)
    {
      printk("Process attempting to acess address 0. Don't try to kill the kernel, please!");
      return -1;
    }
  switch(request)
    {
    case ENCRYPT_DEV_CODE:
      {
	//      printk("ENCRYPT_DEV_CODE case running");
      current_key = device_table[dev_id].key_stream;
      // printk( "current_key: %s", current_key);
      // printk("key inside device table: %s", device_table[dev_id].key_stream);
      }
      break;
    default:
      break;
    }
}

static ssize_t encrypt(struct file * user_file, const char __user *user_message, size_t message_size,  loff_t* inisde)
{
  if(current_key == NULL)
    {
      printk(KERN_INFO "current_key is not valid");
      return -1;
    }
  char message[message_size+ 1];
  copy_from_user(&message,user_message, message_size);
  message[message_size] = 0;
  //  int message_size = strlen(message);
  int key_size = strlen(current_key);
  char msgcpy[message_size];
  strcpy(msgcpy, message);
  int i = 0;
  for (i = 0; i < message_size; i++)
    {
    message[i] = msgcpy[i] + current_key[i%key_size];
    }
  copy_to_user(user_message, &message, strlen(message) + 1);
  printk("encryption is done!");
  return 0;
}

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
   copy_from_user( &user_dev_info,(const device_record*) dev_info, sizeof(device_record) );
   //   printk("copy_from_user: %s", user_dev_info.decrypt_name);
  switch(request)
    {
    case CREATE_DEV_CODE:
      {
      create_pair(&user_dev_info, &fops_encrypt);
      }
      break;
    case DESTROY_DEV_CODE:
      destroy_pair(&user_dev_info);
      break;
      //    case ENCRYPT:
      //  current_key = device_table[dev_info].key_stream;
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
  int minor = 1;
  while(i<DEVICE_RECORDS_SIZE)
    {
      device_table[i].device_id =  i;
      device_table[i].major = driver_major;
      device_table[i].encrypt_minor = minor;
      device_table[i].decrypt_minor = minor +1;
      device_table[i].free =  1;
      printk(KERN_INFO"minors: (%d,%d)",device_table[i].encrypt_minor,device_table[i].decrypt_minor );
      minor += 2;
      i++;
    }
  return;
}
int init_module(void)
{
  init_device_table();
  if(alloc_chrdev_region(&cryptctl_dev,driver_major , DEVICE_RECORDS_SIZE * 2, CRYPTCTL_NAME)<0) //this writes  the device to /proc/devices
   {
     printk("There was an error registering this device\n");
     return -1;
   }
  cdev_init(&cryptctl,&fops);
  printk("cdev_init");
  if(cdev_add(&cryptctl, cryptctl_dev, 1)<0)
    {
      printk(KERN_INFO"There was a problem adding this device :(");
      return -1;
    }
  printk("cdev_add");
  if( (crypt_class = class_create(THIS_MODULE, "crypt_class")) == NULL)
    {
      printk(KERN_INFO "There was a problem creating the class");
      return -1;
    }
  
  if ((device_create(crypt_class, NULL, cryptctl_dev, NULL, "cryptctl")) == NULL) //adds the device file to /dev
    {
      printk(KERN_INFO "There was a problem creating the device ");
      return -1;
    }
  printk(KERN_INFO"I think the device should be registered now:%d, %d\n", MAJOR(cryptctl_dev), MINOR(cryptctl_dev ));
  driver_major = MAJOR(cryptctl_dev);
  driver_minor  = MINOR(cryptctl_dev);
 return 0;
}
void cleanup_module(void)
{
  //  unregister_chrdev(driver_major, CRYPTCTL_NAME);
  destroy_char_dev(driver_major,driver_minor,&cryptctl);
  printk("Device was unregustered. See ya later alligator");
  return 0;
}

static int create_pair(device_record* pair_info,const struct file_operations* fops )
{
   int  dev_id = pair_info->device_id;
   memcpy(&(device_table[dev_id].encrypt_name), pair_info->encrypt_name, 16);
   memcpy(&(device_table[dev_id].decrypt_name), pair_info->decrypt_name, 16);
   memcpy(&(device_table[dev_id].key_stream), pair_info->key_stream, 32);
   device_table[dev_id].major =  driver_major;
   create_char_dev(MAJOR(cryptctl_dev),device_table[dev_id].encrypt_minor,device_table[dev_id].encrypt_name ,&(device_table[dev_id].encrypt_device), fops );
   create_char_dev(MAJOR(cryptctl_dev),device_table[dev_id].decrypt_minor, device_table[dev_id].decrypt_name,&(device_table[dev_id].decrypt_device), fops );
   device_table[dev_id].free = 0;
   printk("pair:(major: %d,encrypt_minor: %d,decrypt_minor %d,encrypt_name: %s,decrypt_name: %s, key_stream:%s)",device_table[dev_id].major,device_table[dev_id].encrypt_minor,device_table[dev_id].decrypt_minor ,  device_table[dev_id].encrypt_name,device_table[dev_id].decrypt_name,device_table[dev_id].key_stream  );
   return 0;
}
static int destroy_pair(device_record* pair_info)
{
  int dev_id  = pair_info->device_id;
  printk(KERN_INFO "id passed: %d", dev_id);
  device_table[dev_id].free = 1;
  destroy_char_dev(device_table[dev_id].major, device_table[dev_id].encrypt_minor, &(device_table[dev_id].encrypt_device));
  destroy_char_dev(device_table[dev_id].major, device_table[dev_id].decrypt_minor, &(device_table[dev_id].decrypt_device));
  return 0;
  
}
static int create_char_dev(unsigned int major,unsigned int minor , const char* dev_name,struct cdev* new_device, const struct  file_operations* fops)
{
  // struct  cdev new_device;
  dev_t new_dev = MKDEV(major,minor);
  cdev_init(new_device, fops);
  printk("minor passed: %d\n", minor);
  if( cdev_add(new_device,new_dev, 1 ) < 0)
    {
      printk(KERN_INFO "There was a problem with cdev_init pair#2");
      return -1;
    }
  if ( (device_create(crypt_class, NULL, new_dev, NULL, dev_name)) == NULL) //adds the device file to /dev
    {
      printk(KERN_INFO "There was a problem creating the device ");
      return -1;
    }
  printk("New Registered device:(%s, %d,%d)", dev_name, major, minor);
  return 0;
}
static int destroy_char_dev( unsigned int major, unsigned int minor, struct cdev* device)
{
      cdev_del(device );
      device_destroy(crypt_class, MKDEV(major,minor));
      // unregister_chrdev_region(MKDEV(major, minor), 1);
  printk(KERN_INFO "Device was successfully destroyed");
  return 0;
}

//module_init(char_dev_ctl);
//module_exit(hello_exit);
//printf("Hi Kernel\n");
