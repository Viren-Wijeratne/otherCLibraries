#include <stdio.h>

enum Bool {FALSE, TRUE};

int main()
{
	enum Bool isTrue = TRUE;
	enum Bool isFalse = FALSE;

	printf("isTrue: %d\n", isTrue);
	printf("isFalse: %d\n", isFalse);

return 0;
}

/*
 *In an enum, either or scenario is used, here "enum Bool = FALSE" or "enum Bool = TRUE"
 *
 * */
