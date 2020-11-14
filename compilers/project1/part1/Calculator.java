import java.util.Scanner;
import java.util.*;

public class Calculator
{
	public static String number( Stack<String> stack)
	{
		String number="";

		// num -> digit num
		if ( (!stack.empty()) && ( (stack.peek().charAt(0)=='0') || (stack.peek().charAt(0)=='1') || (stack.peek().charAt(0)=='2') || (stack.peek().charAt(0)=='3') || (stack.peek().charAt(0)=='4') || (stack.peek().charAt(0)=='5') || (stack.peek().charAt(0)=='6') || (stack.peek().charAt(0)=='7') || (stack.peek().charAt(0)=='8') || (stack.peek().charAt(0)=='9') ) )
		{
			number += stack.peek().charAt(0);
			stack.pop();

			number += number(stack);
		}

		// otherwise, do nothing and continue
		// in this case, the returned String is "" (empty String), so that the number wont change ( number + "" = number)
		return number;
	}

	public static double factor( Stack<String> stack) throws ParseError
	{
		double return_value = 0;

		// factor -> num
		if ( (!stack.empty()) && ( (stack.peek().charAt(0)=='0') || (stack.peek().charAt(0)=='1') || (stack.peek().charAt(0)=='2') || (stack.peek().charAt(0)=='3') || (stack.peek().charAt(0)=='4') || (stack.peek().charAt(0)=='5') || (stack.peek().charAt(0)=='6') || (stack.peek().charAt(0)=='7') || (stack.peek().charAt(0)=='8') || (stack.peek().charAt(0)=='9') ) )
		{
			String number="";

			number += number(stack);

			return_value = Float.parseFloat(number);
		}
		// factor -> (exp)
		else if ( (!stack.empty()) && (stack.peek().charAt(0)=='(') )
		{
			stack.pop();

			return_value = expr( stack);
			if ( (!stack.empty()) && (stack.peek().charAt(0)==')') )
			{
				stack.pop();
			}
			else
			{
				throw new ParseError();
			}
		}
		// otherwise, its an error
		else
		{
			throw new ParseError();
		}

		// return
		return return_value;
	}

	public static double term2( Stack<String> stack) throws ParseError
	{

		// term2 -> * factor term2
		if ( (!stack.empty()) && ( stack.peek().charAt(0) == '*') )
		{
			stack.pop();

			return ( factor( stack) * term2( stack) );
		}
		// term2 -> / factor term2
		else if ( (!stack.empty()) && ( stack.peek().charAt(0) == '/') )
		{

			stack.pop();

			return ( 1.0 / (factor( stack) / term2( stack)) );
		}

		// otherwise, do nothing and continue
		// in this case, the returned number is 1, so that the returned number from factor wont change (number * 1 = number)
		return 1;
	}

	public static double expr2( Stack<String> stack) throws ParseError
	{
		// expr2 -> + term expr2
		if ( (!stack.empty()) && ( stack.peek().charAt(0) == '+') )
		{
			stack.pop();

			return ( term( stack) + expr2( stack) );
		}
		// expr2 -> - term expr2
		else if ( (!stack.empty()) && ( stack.peek().charAt(0) == '-') )
		{
			stack.pop();

			return (- ( term( stack) - expr2( stack) ) );
		}

		// otherwise, do nothing and continue
		// in this case, the returned number is 0, so that the returned number from term() wont change (number + 0 = number)
		return 0;
	}

	public static double term( Stack<String> stack) throws ParseError
	{
		// term -> factor term2
		return ( factor( stack) * term2( stack));

	}

	public static double expr( Stack<String> stack) throws ParseError
	{
		// expr -> term expr2
		return ( term( stack) + expr2( stack) );
	}

	public static void goal( Stack<String> stack) throws ParseError
	{
		// goal -> expr
		double result = expr( stack);

		// if whole input wasnt read, then its a parse error
		if ( !stack.empty() )
		{
			throw new ParseError();
		}

		// print the result
		System.out.println(result);
	}

	public static void main(String[] input)
	{
		// read input from command line
		Scanner scan = new Scanner(System.in);
		String string_for_parsing = scan.nextLine();

		// create stack with nodes for each token of the given string
		Stack<String> stack = new Stack<>();
		int current = string_for_parsing.length() - 1;
		while ( current >= 0 )
		{
			if ( ( string_for_parsing.charAt(current) != ' ') && ( string_for_parsing.charAt(current) != '\t') ) stack.push(String.valueOf( string_for_parsing.charAt(current) ));

			current--;
		}

		// check if the given tokens belong to the grammar and if they o, print the result, otherwise print an error message
		try
		{
			goal( stack);
		}
		catch(ParseError p)
		{
			p.showInfo();
		}
	}
}
