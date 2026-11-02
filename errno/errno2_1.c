/*The C library EDOM Macro represents a domain error, which occurs if an input argument is outside the domain, over which the mathematical function is defined and errno is set to EDOM.*/

#include <stdio.h>
#include <errno.h>
#include <math.h>

int main()
{

	double invalid_value = 2.0;
	
	double result = asin(invalid_value);
	if(errno == EDOM)
	{
		printf("Arcsine of %f is not defined\n", invalid_value);
	}
	else
	{
		printf("Arcsine of %f is: %f radians", invalid_value, result);
	}


return 0;
}

