/*ptrdiff_t is the signed integer type returned by subtracting two pointers that point into the same array.*/
/*size_t is the unsigned integral type and is the result of the "sizeof" keyword.*/


#include <stdio.h>
#include <stddef.h>



int main()
{

	int array[5] = {10, 20, 30, 40, 50};

	int *p1 = &array[1];
	int *p2 = &array[4];

	printf("p1: %p\n", (void *)p1);		//%p expects void *
    	printf("p2: %p\n", (void *)p2);

	ptrdiff_t difference = p2 - p1;
	printf("Difference: %td\n", difference);

return 0;
}
