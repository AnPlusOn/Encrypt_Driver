MapReduce Engineer: Lorenzo Gomez                                                                                       Encryption Engineers: Anton Ryjov, Michelle Dry-Moran                                                                 Documentation by Michelle Dry-Moran



NOTE:  Please see README_CS416_Proj2.rtf for the formatted version of this README file:


CS416 Operating Systems, Fall 2018
Assignment #2: Encrypted “Pipe” Pseudo-Device Driver

Group:
Lorenzo Gomez (netid: lbg65)
Anton Ryjov (netid: aar188)
Michelle Dry-Moran (netid: mdry)

NOTE: The instructions for running the code is the 3rd Section on this report.

*********************************************************************************************
*********************************************************************************************
*********************************************************************************************
Experience Writing Code

This experience was challenging as we did not understand much about devices / ioctl.
The three of us were all fairly well versed in writing in c; however understanding how to 
create devices was difficult.


Key Issues that were encountered

PROBLEM: We initially had issues understanding the way that the kernel represents devices
RESOLVED BY: Reading the documentation that was given, googling what’s out there, and trial and error

PROBLEM: We crashed the kernel a couple of times
RESOLVED BY: sending “help” emails to our lovely Rutgers IT guys to rebuild kernel.

PROBLEM: When deleting the device, an error would occur when rebuilding the kernel saying that it already existed. 
Somehow we are not properly deleting the device.  When we could not figure out the issue - we had to use sudo reboot to fix this. 
We knew there are multiple steps for delete and tried them in different orders - all which did not work.
RESOLVED BY: Issue turned out to be the Makefile.  We were using an old version of makefile.

PROBLEM: Cipher was not printing out correctly and changing randomly.
RESOLVED BY: We did not initiate variables - after memsetting variables to NULL (and ensuring strings were NULL terminated),
the cipher worked correctly without errors.


*********************************************************************************************
*********************************************************************************************
*********************************************************************************************
Description of Implementation

1) Cipher Algorithm

We implemented Vigenere Cipher, using a key string with max length of 32 characters.
The key is a string such that any ASCII character between ASCII 32 (space) to ASCII 126 (tilde ~) can be used in the key.
	* ASCII 32 will shift a letter by 0
	* ASCII 126 will shift a letter by 94 (essentially 126-32)
	* all characters in between will shift accordingly between 0 and 94 based on ASCII order
If a character is shifted past ASCII 126, the shift will wrap back around to ASCII 32.
	** Example: If you want to encode the letter z by a key of 1 (ASCII 49),
	then you would be shifting that letter 17 spaces (49-32), such that z would turn into ASCII 44 (or comma ,)
	because z is ASCII 122, you add 4 and you get to 126, add 1 more, you get back to ASCII 32, and then you have 12 more which brings you to ASCII 44.

Additionally, since this is a key string, each character in the key will shift a letter in the message to be encoded, such that:
	* the first character in the key will encode the first character in the message
	* the second character in the key will encode the second character in the message
	* the nth character in the key will encode the nth character in the message
If there are more characters in the message than there are in the key, then the key will start again, such that:
	* the nth + 1 character in the message will be encoded with the first character in the key
	* the nth + 2 character in the message will be encoded with the second character in the key
	* the nth + nth character in the message will be encoded with the nth character in the key
This will repeat until there are no longer any characters in the message to be encoded.

If there are less characters in the message than there are characters in the key, then the key will only use the max characters of the message - and any additional key characters will be dropped.  Example: Message is “hi” and key is “12345”.  Since the characters in messages are only two, then the characters 12 of the key will be used, and 345 will be dropped.


2) Overall Control Flow within Driver

1. We run: sudo insmod cryptctl_driver.ko
	This calls init_module which does the following:
		* institiate and initiate the device_table (struct device_record) which provides an interface to allow communication between
		  the user space and the kernel space.
		* creates device controller/driver

2. The device table is created as follows:
	typedef struct
	{
	  char  free; //  1 if it's free, 0 otherwise.                                                                                                                                       
	  short old_device_id; //useful for swapping operations, like renaming devices                                                                                                       
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

3. We run: sudo ./crypt_main <command> <args>

configuration_ioctl():
	case: CREATE_DEV_CODE …. Creates device pairs
	case: DESTROY_DEV_CODE …. Destroys a specific device pairs (given a device number)
	case: CHANGE_KEY_DEV_CODE …. Changes the key in the devices
	case: DOOM_DEV …. Destroys ALL device pairs
	case: RENAME_DEV_CODE …. Renames a given device pair to a new device name

encrypt_ioctl():
	case: ENCRYPT_DEV_CODE …. Sets the global current_key for encrypt/decrypt operations.

cleanup_module: 
	* gives back resources to kernel 
	* destroys all pairs by calling Doomsday
	* destroys driver

*********************************************************************************************
*********************************************************************************************
*********************************************************************************************
Instructions for Running Module

1. make/compile kernel module with the "make" command

2. insert the module with "sudo insmod cryptctl_driver.ko"

3. Check the kernel's log with the "dmesg" command:
	If the module was loaded successfully, you should see something like this at the end of the log:
				[  163.501375] cdev_init
				[  163.501381] cdev_add
				[  163.502710] I think the device should be registered now:244, 0


The numbers inside the [] are just timestamps; these will be different for you.
If you don't see these messages anywhere, then reboot your kernel with "sudo reboot" immediately.

4. Assuming that you saw the right log messages, the driver(cryptctl) should be registered with the kernel. To confirm that the driver is loaded/registered in the kernel, run the following commands:
	4a.”ls /dev":
		This will show you a list of the special files that are linked to drivers and devces. In this directory you should see a file with the name "crypctl".
		This means that there is a file associated with the cryptctl devices and our user program (crypt_main.c) can open it and send commands
		to it via ioctl() calls.

	4b.”cat /proc/devices":
		This file has all of the drivers registered in the system. After you insert the module, you should see the name of the device along with its major number.
		It should look something like "cryptctl registration_number". In the machine we are using, the major number has always been 244.
		However, keep in mind, that if the kernel deems it so this could be a different number. That major_number is the major number
		for our driver and all of the devices that the driver manages. As you can see in the third log message from step 3
		the string "I think the device should be registered now:244, 0"  has a 244 followed by a 0.
		These two are the major and minor. In this case, the major is 244 and the minor is 0.
		The minor should always be 0 as it is the first device that is registered with this major number

	4c. "ls /sys/dev/char":
		This directory is the MOST IMPORTANT of the three. This is the directory that the kernel uses to keep track of ALL of the devices
		currently registered in the system.
		When you look at the contents of this directory, there should a bunch of special directories with the format "major: minor".
		These directories represent all of the devices currently registered in the system. After loading the kernel, you should see
		the major number that you saw in /proc/devices.

5. Now we're ready to use our user program. Compile the user program with "gcc crypt_main.c".
Once you've compiled crypt_main.c, run the executable with "sudo”:

	5A) CREATE
		To create a new device pair, we enter data in the following format "sudo ./a.out c id key_stream”
		command
			c			… creates a new device pair (can create a total 128 device pairs - or 256 devices) 
		arguments
			id			… id is a number between 0 and 127 (if a device number has already been taken,
						    the program will let the user know that device already exists)
			key_stream	… a key string that the user wants to use (Note: the key_string has to be enclosed by quotation marks)
						     example keys “123”   or   “ab1xz”
						    The key_stream can be up to a max of 32 characters, with any ASCII character between
						    ASCII 32 (space) to ASCII 126 (tilde ~), such that ASCII 32 shifts 0 spaces, and ASCII 126 shifts 94 spaces  

		EXAMPLE: 	sudo ./a.out c 0 “123" 
					- The  driver will create the following devices with key 123:
						/dev/cryptEncrypt0
						/dev/cryptDecrypt0

		To confirm that the devices have been created/registered:
			* Check all of the directories from Step 4.
			* You should also see log messages similar to the ones in step 1.

	5B) DESTROY
		To destroy a single specific device pair, we enter data in the following format "sudo ./a.out d id”
		command
			d			… destroys a single device pair (the device designated by id) 
		argument
			id			… id is a number between 0 and 127 (of a device that already exists)

		EXAMPLE: 	sudo ./a.out d 1 
					- The  driver will destroy the following devices:
						/dev/cryptEncrypt1
						/dev/cryptDecrypt1

		To confirm that the devices have been destroyed:
			* Check all of the directories and files from step 4.
			* There should be no trace of your cryptEncrypt_id and cryptDecrypt_id devices
			* Your cryptctl with minor number 0 device SHOULD still be there as we have not destroyed/unregistered it yet

	5C) ENCRYPT
		To encrypt a message, we enter data in the following format "sudo ./a.out en id message”
		command
			en			… encrypts a given message (message is only limited in size by the space of the control prompt) 
		arguments
			id			… id is a number between 0 and 127 (NOTE: the device has to be created first before it can be used to encrypt the message)
			message		… a message that the user wants to have encrypted.  (Note: the message has to be enclosed by quotation marks)
						     example message:	“Hi, how are you?”
						    The message is limited in size by the size of the command line.
						    The message should only consist of characters between ASCII 32 (space) to ASCII 126 (tilde ~). 

		EXAMPLE: 	sudo ./a.out en 0 “abcdefg”
					If id 0 contains key_string “12”, then:
					the encryption of abcdefg would be rttvvxx

	5D) DECRYPT
		To decrypt a message, we enter data in the following format "sudo ./a.out de id message”
		command
			de			… decrypts a given message (message is only limited in size by the space of the control prompt)
		arguments
			id			… id is a number between 0 and 127 (NOTE: the device has to be created first before it can be used to decrypt the message)
			message		… an ecrypted message that the user wants to have decrypted.  (Note: the message has to be enclosed by quotation marks)
						     example encrypted message:      “Zljg 6d3 r4saXZw”
						    The message is limited in size by the size of the command line.
						    The message should only consist of characters between ASCII 32 (space) to ASCII 126 (tilde ~). 

		EXAMPLE: 	sudo ./a.out de 0 “rttvvxx”
					If id 0 contains key_string “12”, then:
					the decryption of rttvvxx would be abcdefg

	5E) DOOMSDAY
		To  annihilate (wipe out / destroy) ALL of the device pairs, we enter data in the following format "sudo ./a.out doom”
		command
			doom		… destroys all device pairs 
		argument
			N/A			… there are no arguments for doomsday

		EXAMPLE: 	sudo ./a.out doom
					- The  driver will destroy all devices:
						/dev/cryptEncrypt[id]
						/dev/cryptDecrypt[id]
					  … such that [id] is a number from 0 to 127

		To confirm that the devices have been destroyed:
			* Check all of the directories and files from step 4.
			* There should be no trace of your cryptEncrypt_id and cryptDecrypt_id devices
			* Your cryptctl with minor number 0 device SHOULD still be there as we have not destroyed/unregistered it yet


6. EXTRA CREDIT - Change the key of a device pair / Rename a device pair
	Compile the user program with "gcc crypt_main.c".  Once you've compiled crypt_main.c, run the executable with "sudo”:

	6A) CHANGE
		To change a key_string to a new key_string for a given device, we enter data in the following format "sudo ./a.out ck id key_stream”
		command
			ck			… changes the key on an already created device pair 
		arguments
			id			… id is a number between 0 and 127, such that the device pair has already been created
			key_stream	… the new key string that the user wants to use (Note: the key_string has to be enclosed by quotation marks)
						     example keys “123”   or   “ab1xz”
						    The key_stream can be up to a max of 32 characters, with any ASCII character between
						    ASCII 32 (space) to ASCII 126 (tilde ~), such that ASCII 32 shifts 0 spaces, and ASCII 126 shifts 94 spaces  

		EXAMPLE: 	sudo ./a.out ck 0 “123" 
					- The driver will change the old key to a new key of 123 in the following devices:
						/dev/cryptEncrypt0
						/dev/cryptDecrypt0

	6B) RENAME
		To rename the id of a given device pair, we enter data in the following format "sudo ./a.out re old_id new_id”
		command
			re			… changes the key on an already created device pair 
		arguments
			old_id		… old_id is a number between 0 and 127, such that the device pair has already been created
			new_id		… new_id is a number between 0 and 127, such that this device pair id has not already been used
	
		EXAMPLE: 	sudo ./a.out 0 1 
					- The driver will change the old device pairs
					  From:
						/dev/cryptEncrypt0
						/dev/cryptDecrypt0
					  To: 	
						/dev/cryptEncrypt1
						/dev/cryptDecrypt1

		Note: The key will stay the same.  Essentially this command will destroy the "old" device and create a new one with the "new" name.


7. Last but not least we unload the module.

	Unload the module with "sudo rmmod cryptctl_driver". This will unregister the driver device--your cryptctl driver.

NOTE: DO NOT unload the module while a pair of cryptDecrypt_id, cryptEncrypt_id is still registered. Make sure that there are no device pairs left when you're about to unload the module. Unloading the module while there is a pair device floating around could put the kernel in a panic state, which we don't want.

Hope these instructions are clear enough.
Don't hesitate to ask any questions

THANKS!!!

END OF REPORT
*********************************************************************************************
*********************************************************************************************
*********************************************************************************************
