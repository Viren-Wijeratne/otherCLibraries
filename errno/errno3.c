/*The C library ERANGE Macro represents a range error, which occurs if an input argument is outside the range, over which the mathematical function is defined and errno is set to ERANGE.*/
/*The ERANGE associated with mathematical functions that set to input ranges. By setting errno to ERANGE, it represents the result of operation which is not representable within the valid range.*/
/*The range mentioned above comes from the data type used to save the variable, errno checks if the result underflows or overflows*/

#include <stdio.h>
#include <errno.h>
#include <math.h>

int main()
{
	//The boundary is fixed by the maximum and minimum representable values of the floating-point type that the function returns.
	double x;
	double value;


	x = 2.000000;
	value = log(x);

	if(errno == ERANGE)
	{
		printf("Log(%f) is out of range\n", x);
	}
	else
	{
		printf("Log(%f) = %f\n", x, value);
	}

	x = 1.000000;
	value = log(x);

	if(errno == ERANGE)
	{
		printf("Log(%f is out of range\n)", x);
	}
	else
	{
		printf("Log(%f) = %f\n", x, value);
	}

	x = 0.000000;
	value = log(x);

	if(errno == ERANGE)
	{
		printf("Log(%f) is out of range\n", x);
	}
	else
	{
		printf("Log(%f) = %f\n", x, value);
	}

return 0;
}

/*Note:
 *
 *The range of value that can be held within a float can be found using the float.h header file which contains macros such as DBL_MAX that can tell the sizes of specific float type
 *
 *
 * */
