/*Using assert.h and its relavent two MACROS*/

/* 
* The <assert.h> header file of the C Standard Library provides a macro called assert which can be used to verify assumptions made by the program and print a diagnostic message if this assumption is false.
* 
* Syntax of the MACRO: void assert(int expression);	//This is actually a macro and not a function, which can be used to add diagnostics in your C program. Used for runtime error handling 
*
* expression − It is a conditional statement that evaluates to either true or false. If the expression evaluates to false (0), the assert() function triggers an assertion failure, leading to termination of
*	      the program.
* 
* This macro does not return any value. If the assertion fails (expression evaluates to false), it triggers a error message and aborts the program execution.
*
==============================================================================================================================================================================================================
*
* The C assert library static_assert() macro is a powerful tool introduced in the C11 standard that allows you to perform compile-time assertions. This means that you can check certain conditions at compile
*  time rather than at runtime, catching potential errors early in the development process.
*
* Syntax of the MACRO: static_assert (boolean_expression, message);	//Used for compile time error handling 
*
* boolean_expression − This is a constant expression that the compiler evaluates at compile time. If this expression evaluates to 0 (false), the compilation fails.
* message − This is a string literal that provides a description of the error when the assertion fails. This message is displayed during compilation if the assertion fails.
*
* This macro does not return a value. Instead, it either allows the compilation to proceed if the assertion is true or causes a compile-time error if the assertion is false.
*
*/

#include <stdio.h>
#include <assert.h>

static_assert(sizeof(int) == 4, "int must be 4 bytes");		//Checks the condition at compile time and give a compiler error if false + error message

int main()
{

	int a = 5;

	printf("This is before the assert function!\n");

	assert(a == 5);						//Checks at runtime and abort if false + error message

	printf("This is after the assert function!\n");


/**************************************************/

	int x = 5; 
	int y = 3;

//Check if the sum of x and y is greater than 5, if program aborts with error message "Sum is not greater than 5"
	assert((x + y) > 5 && "Sum is not greater than 5");	////Checks at runtime and abort if false + error message
	
	printf("2nd assertion function passed!\n");

return 0;
}
