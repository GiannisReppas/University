#include "itoa_plus_nod.h"

void integer_to_string(long int number, char *str)
{
	int i = 0,j;
	int temp;

	//number to string , but reversed

	int last_digit;
	while( number )
	{
		// r --> the last digit of the integer
		last_digit = number%10;

		// at ascii numbers start from 48
		str[i] = 48 + last_digit;

		// next digit for string
		i++;

		// cut the last digit from the number
		number = number / 10;
	}

	// if number is 0
	if(i == 0)
	{
		str[i] = '0';
		i++;
	}

	// If number is negative
	if( number < 0)
	{
		str[i] = '-';
		i++;
	}

	str[i] = '\0';

	// reverse the string and return it

	// j --> last character
	// i --> first character
	j = i-1;
	i=0;
	while(i < j)
	{
		//swap
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;

		//next pair of characters
		i++;
		j--;
	}
}

int number_of_digits( long int number)
{
	int digits=0;
	while( number >= 10)
	{
		number = number/10;
		digits++;
	}
	digits++;

	return digits;
}
