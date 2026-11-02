/*The C library EDOM Macro represents a domain error, which occurs if an input argument is outside the domain, over which the mathematical function is defined and errno is set to EDOM.*/

#include <stdio.h>
#include <errno.h>
#include <math.h>

int main()
{
	double val;

	errno = 0;
	val = sqrt(-10);

	if(errno == EDOM)
	{
		printf("Invalid value\n");	//Not printed to the standard error as the program will exit after the error is displayed in the console
	}
	else
	{
		printf("Valid value\n");
	}

	errno = 0;
	val = sqrt(10);

	if(errno == EDOM)
	{
		printf("Invalid value\n");
	}
	else
	{
		printf("Valid value\n");
	}


return 0;
}

/*Note:
 *
 * Remember to use the "-lm" flag at compilation to include the math library
 *
 *
 * */
