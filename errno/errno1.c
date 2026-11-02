
/*The errno is set to zero at program startup. Certain functions of the standard C library modify its value to other than zero to signal some types of error. You can also modify its value or reset to zero at your convenience.*/

#include <stdio.h>
#include <errno.h>
#include <string.h>	//Used for strerror (which searches an internal array for the error number errnum and returns a pointer to an error message string)

extern int errno; //Macro from errno.h which produces an int in the event of an error by system calls

int main()
{
	FILE *fp;

	fp = fopen("file.txt", "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "Value of errno: %d\n", errno);
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
	}
	
	else{
		fclose(fp);	
	     }


return 0;
}

/*Note:
 *
 *The term "extern" is used to declare variables and functions whose definitions are located in another source file
 *	- Does not allocate memory for a variable or create a function definition.
 *	- Helps the linker resolve references across multiple source files.
 *	- Commonly used in multi-file programs to access shared global variables and function
 *
 *Following are the list of error occurs in C programs −
 *	- File I/O errors (e.g., file not found, permission denied)
 *	- Memory allocation failures (e.g., malloc() returning NULL)
 *	- Invalid function arguments (e.g., passes invalid file)
 *	- Math-related errors (e.g., division by zero)
 *	- Network-related errors (e.g., socket errors)
 */
