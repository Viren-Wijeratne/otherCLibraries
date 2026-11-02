#include <stdio.h>
#include <errno.h>
#include <math.h>

int main()
{

	double x = 1000;

	double result = exp(x);
	if(errno == ERANGE)
	{
		printf("exp(%f) is out of range\n", x);
	}
	else
	{
		printf("exp(%f) = %f\n", x, result);
	}

return 0;
}

/*Note:
 *
 * If you call exp(10000), the mathematical result is far greater than the largest number a double can hold (DBL_MAX ~ approx 1.79 \times 10^{308}\)). 
 * The function detects this internally, sets errno = ERANGE, and returns the special value HUGE_VAL (which represents infinity).
 * 
 */
