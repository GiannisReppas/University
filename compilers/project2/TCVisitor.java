import syntaxtree.*;
import visitor.GJDepthFirst;

import java.io.*;
import java.util.*;

public class TCVisitor extends GJDepthFirst<String, String>
{
	// the type checking visitor has a variable for the symbol table visitor
	// this way we can have access to the symbol table
	STPVisitor stpv = new STPVisitor();

	/* at the constructor we also check for variables or method return types that have class names, because we can also have forward declarations */
	public TCVisitor(STPVisitor stpv)
	{
		this.stpv = stpv;

		this.inTC = true;

		this.searchMethod = false;

		this.realExpressionList = "";

		int i,j,k;
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			for ( j=0; j < this.stpv.classes.get(i).attributes.size(); j++)
			{
				if ( !this.stpv.classes.get(i).attributes.get(j).type.equals("int") && !this.stpv.classes.get(i).attributes.get(j).type.equals("int[]") &&
					!this.stpv.classes.get(i).attributes.get(j).type.equals("boolean") && !this.stpv.classes.get(i).attributes.get(j).type.equals("boolean[]") &&
					!isClass(this.stpv.classes.get(i).attributes.get(j).type) )
				{
					throw new MyException("Error - Object creation of non existing class (in class attributes)");
				}
			}

			for ( j=0; j < this.stpv.classes.get(i).methods.size(); j++)
			{
				if ( !this.stpv.classes.get(i).methods.get(j).type.equals("int") && !this.stpv.classes.get(i).methods.get(j).type.equals("int[]") &&
					!this.stpv.classes.get(i).methods.get(j).type.equals("boolean") && !this.stpv.classes.get(i).methods.get(j).type.equals("boolean[]") &&
					!isClass(this.stpv.classes.get(i).methods.get(j).type) && (i!=0) )
				{
					throw new MyException("Error - Function returns type of non exisitng class -> "+this.stpv.classes.get(i).methods.get(j).type + " " + i);
				}

				for ( k=0; k < this.stpv.classes.get(i).methods.get(j).parameters.size(); k++)
				{
					if ( !this.stpv.classes.get(i).methods.get(j).parameters.get(k).type.equals("int") && !this.stpv.classes.get(i).methods.get(j).parameters.get(k).type.equals("int[]") &&
						!this.stpv.classes.get(i).methods.get(j).parameters.get(k).type.equals("boolean") && !this.stpv.classes.get(i).methods.get(j).parameters.get(k).type.equals("boolean[]") &&
						!isClass(this.stpv.classes.get(i).methods.get(j).parameters.get(k).type) && (i!=0) )
					{
						throw new MyException("Error - Object creation of non existing class (in method parameters)");
					}
				}

				for ( k=0; k < this.stpv.classes.get(i).methods.get(j).variables.size(); k++)
				{
					if ( !this.stpv.classes.get(i).methods.get(j).variables.get(k).type.equals("int") && !this.stpv.classes.get(i).methods.get(j).variables.get(k).type.equals("int[]") &&
						!this.stpv.classes.get(i).methods.get(j).variables.get(k).type.equals("boolean") && !this.stpv.classes.get(i).methods.get(j).variables.get(k).type.equals("boolean[]") &&
						!isClass(this.stpv.classes.get(i).methods.get(j).variables.get(k).type) )
					{
						throw new MyException("Error - Object creation of non existing class (in method variables)");
					}
				}
			}
		}
	}

	// these variables hold information for the class and method we are visitting while traversing the tree
	String currentClass;
	String currentMethod;

	// inTC flag is basically used for the identifier visit function
	// when inTC is true, then we get the type of the identifier
	// when inTC is false, then we get the name of the identifier
	boolean inTC;
	// search method flag is also used for the identifier visit function
	// when it is true, we call a function that searches for the type of a function and not the type of a variable
	// this method is basically the identifier
	// see below for further explanation
	boolean searchMethod;

	// this variable is used to hold info for the parameters of a function at a message send expression (explained more below)
	String realExpressionList;

	///////////////////////////////////////

	/* returns the type of method givenMethodName, which is inside the class givenClassName*/
	public String getMethodType(String givenClassName, String givenMethodName)
	{
		String className; String methodName;
		if ( givenClassName.equals("this")) { className = this.currentClass; methodName = givenMethodName; }
		else { className = givenClassName; methodName = givenMethodName; }

		String returnValue;
		int i,j;
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			if ( this.stpv.classes.get(i).name.equals(className))
			{
				for ( j=0; j < this.stpv.classes.get(i).methods.size(); j++)
				{
					if ( this.stpv.classes.get(i).methods.get(j).name.equals(methodName))
					{
						returnValue = this.stpv.classes.get(i).methods.get(j).type;

						return returnValue;
					}
				}

				break;
			}
		}

		throw new MyException("Error - method given at message send expression does not exist");
	}

	/* returns the type of variable varName, which is inside method methodName, which is inside class className*/
	public String getVarType(String className, String methodName, String varName)
	{
		int i,j,k;
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			if ( this.stpv.classes.get(i).name.equals(className))
			{
				for ( j=0; j < this.stpv.classes.get(i).methods.size(); j++)
				{
					if( this.stpv.classes.get(i).methods.get(j).name.equals(methodName))
					{
						for ( k=0; k < this.stpv.classes.get(i).methods.get(j).parameters.size(); k++)
						{
							if ( this.stpv.classes.get(i).methods.get(j).parameters.get(k).name.equals(varName))
								return this.stpv.classes.get(i).methods.get(j).parameters.get(k).type;
						}

						for ( k=0; k < this.stpv.classes.get(i).methods.get(j).variables.size(); k++)
						{
							if ( this.stpv.classes.get(i).methods.get(j).variables.get(k).name.equals(varName))
								return this.stpv.classes.get(i).methods.get(j).variables.get(k).type;
						}

						break;
					}
				}

				for ( j=0; j < this.stpv.classes.get(i).attributes.size(); j++)
				{
					if ( this.stpv.classes.get(i).attributes.get(j).name.equals(varName))
						return this.stpv.classes.get(i).attributes.get(j).type;
				}

				break;
			}
		}

		throw new MyException("Error - variable given in expression does not exisist in class as an attribute or a method variable/parameter");
	}

	/* returns true if the name given is a name of a class and false if it is not a name of a class*/
	public boolean isClass(String className)
	{
		int i;
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			if ( this.stpv.classes.get(i).name.equals(className))
				return true;
		}

		return false;
	}

	/* takes as parameters a class name and a method name inside that class and returns all the types of the parameters of that method in a string */
	/* for example, if the parameters of that method are method(int a, boolean b), the string would be "int boolean" */
	public String getParametersOfMethod(String givenClassName, String givenMethodName)
	{
		String className; String methodName;
		if ( givenClassName.equals("this")) { className = this.currentClass; methodName = givenMethodName; }
		else { className = givenClassName; methodName = givenMethodName; }

		int i,j,k;
		String returnString = "";
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			if ( this.stpv.classes.get(i).name.equals(className))
			{
				for ( j=0; j < this.stpv.classes.get(i).methods.size(); j++)
				{
					if ( this.stpv.classes.get(i).methods.get(j).name.equals(methodName))
					{
						for ( k=0; k < this.stpv.classes.get(i).methods.get(j).parameters.size(); k++)
							returnString += this.stpv.classes.get(i).methods.get(j).parameters.get(k).type + " ";

						break;
					}
				}

				break;
			}
		}

		return returnString;
	}

	/* returns true if class type1 is a child of class type2, which means type1 extends type2 */
	public boolean isChild( String type1, String type2)
	{
		int i;
		for ( i=0; i < this.stpv.classes.size(); i++)
		{
			if ( this.stpv.classes.get(i).name.equals(type2))
			{
				if ( this.stpv.classes.get(i).children.contains(type1))
					return true;

				break;
			}
		}

		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	public String visit(MainClass n, String argu)
	{
		// update this.currentClass and this.currentMethod
		this.inTC = false;
		this.currentClass = n.f1.accept( this, null);
		this.inTC = true;
		this.currentMethod = "main";

		// visit the statements of the main method
		if ( n.f15.present())
		{
			n.f15.accept( this, null);
		}

		return null;
	}

	public String visit(ClassDeclaration n, String argu)
	{
		// update this.currentClass
		this.inTC = false;
		this.currentClass = n.f1.accept( this, null);
		this.inTC = true;

		// visit the methods of the current class
		if ( n.f4.present())
		{
			n.f4.accept( this, null);
		}

		return null;
	}

	public String visit(ClassExtendsDeclaration n, String argu)
	{
		// update this.currentClass
		this.inTC = false;
		this.currentClass = n.f1.accept( this, null);
		this.inTC = true;

		// visit the methods of the current class
		if ( n.f6.present())
		{
			n.f6.accept( this, null);
		}

		return null;
	}

	public String visit(MethodDeclaration n, String argu)
	{
		// update this.currentMethod
		this.inTC = false;
		this.currentMethod = n.f2.accept( this, null);
		this.inTC = true;

		// get the return type (type) of the current method in the definition part
		String methodType = getMethodType( this.currentClass, this.currentMethod);

		// visit the method's statements
		if ( n.f8.present())
		{
			n.f8.accept( this, null);
		}

		// get the type of the return expression of the method
		String returnType = n.f10.accept( this, null);
		if ( returnType.equals("this"))
			returnType = this.currentClass;

		// if the types dont match, throw error
		if ( !returnType.equals(methodType) && !isChild(returnType,methodType))
			throw new MyException("Error - different type between method type and return value");

		return null;
	}

	public String visit(AssignmentStatement n, String argu)
	{
		// get the type of the identifier of the assignment statement (left part)
		String idType = n.f0.accept( this, null);

		// get the type of the expression of the assignment statement (right part)
		String expressionType = n.f2.accept( this, null);
		if ( expressionType.equals("this"))
			expressionType = this.currentClass;

		// if the types dont match, throw error
		if ( !idType.equals(expressionType) && !isChild(expressionType,idType) )
			throw new MyException("Error - Assignment statement must have the same types of identifier and expression");

		return null;
	}

	public String visit(ArrayAssignmentStatement n, String argu)
	{
		// get the type of the identifier of the array assignment statement (left part)
		// it can inly be an array
		String idType = n.f0.accept( this, null);
		if ( idType.equals("int[]"))
			idType = "int";
		else if ( idType.equals("boolean[]"))
			idType = "boolean";
		else
			throw new MyException("Error - There can only be boolean and integer arrays");

		// get the expression that should be the index of the array
		// it can only be an integer
		String indexType = n.f2.accept( this, null);
		if ( !indexType.equals("int"))
			throw new MyException("Error - Index at array assignment expression must be an integer");

		// get the type of the expression of the array assignment statement (right part)
		String expressionType = n.f5.accept( this, null);

		// if the types dont match, throw error
		if ( !idType.equals(expressionType) )
			throw new MyException("Error - Array assignment statement must have the same types of identifier and expression");

		return null;
	}

	public String visit(IfStatement n, String argu)
	{
		// get the type of the if expression
		// it can only be boolean
		String expressionType = n.f2.accept( this, null);
		if ( !expressionType.equals("boolean"))
			throw new MyException("Error - Expression in if statement must be boolean");

		// visit the statements of the if expression
		n.f4.accept( this, null);

		n.f6.accept( this, null);

		return null;
	}

	public String visit(WhileStatement n, String argu)
	{
		// get the type of the while expression
		// it can only be boolean
		String expressionType = n.f2.accept( this, null);
		if ( !expressionType.equals("boolean"))
			throw new MyException("Error - Expression in while statement must be boolean");

		// visit the statements of the while expression
		n.f4.accept( this, null);

		return null;
	}

	public String visit(PrintStatement n, String argu)
	{
		// check if the expression at the print statement is an integer
		if ( !n.f2.accept( this, null).equals("int"))
			throw new MyException("Error - Print statement can only accept integers");

		return null;
	}

	public String visit(Expression n, String argu)
	{
		return n.f0.accept( this, null);
	}

	public String visit(AndExpression n, String argu)
	{
		// the 2 parts of an and expression must be booleans only
		String part1 = n.f0.accept( this, null);
		String part2 = n.f2.accept( this, null);

		if ( !part1.equals("boolean") || !part2.equals("boolean"))
			throw new MyException("Error - cant have an AndExpression with non-boolean parts");

		return "boolean";
	}

	public String visit(CompareExpression n, String argu)
	{
		// the 2 parts of a compare expression must be integers only
		String part1 = n.f0.accept( this, null);
		String part2 = n.f2.accept( this, null);

		if ( !part1.equals("int") || !part2.equals("int"))
			throw new MyException("Error - cant have a compare expression with non-integer parts");

		return "boolean";
	}

	public String visit(PlusExpression n, String argu)
	{
		// the 2 parts of a plus expression must be integers only
		String part1 = n.f0.accept( this, null);
		String part2 = n.f2.accept( this, null);

		if ( !part1.equals("int") || !part2.equals("int"))
			throw new MyException("Error - cant have a plus expression with non-integer parts");

		return "int";
	}

	public String visit(MinusExpression n, String argu)
	{
		// the 2 pats of a minus expression must be integers only
		String part1 = n.f0.accept( this, null);
		String part2 = n.f2.accept( this, null);

		if ( !part1.equals("int") || !part2.equals("int"))
			throw new MyException("Error - cant have a minus expression with non-integer parts");

		return "int";
	}

	public String visit(TimesExpression n, String argu)
	{
		// the 2 parts of a times expression must be integers only
		String part1 = n.f0.accept( this, null);
		String part2 = n.f2.accept( this, null);

		if ( !part1.equals("int") || !part2.equals("int"))
			throw new MyException("Error - cant have a times expression with non-integer parts");

		return "int";
	}

	public String visit(ArrayLookup n, String argu)
	{
		// get the first expression of an array lookup
		// it must be an array
		String part1 = n.f0.accept( this, null);
		if ( !part1.equals("boolean[]") && !part1.equals("int[]") )
			throw new MyException("Error - identifier on an array lookup must be boolean OR integer array");

		// get the second expression of an array look, the index
		// it must be an integer
		String part2 = n.f2.accept( this, null);
		if ( !part2.equals("int"))
			throw new MyException("Error - index on an array lookup must be an integer");

		// if we have a boolean array, return boolean
		// if we have an integer array, return integer
		// it is because we actually return an element from an integer/boolean array
		if ( part1.equals("boolean[]"))
			return "boolean";
		else
			return "int";

	}

	public String visit(ArrayLength n, String argu)
	{
		// get the array type (first expression of array length)
		// obviously, it must be an arrray
		String part1 = n.f0.accept( this, null);
		if ( !part1.equals("boolean[]") && !part1.equals("int[]") )
			throw new MyException("Error - cant compute array length for a non-array identifier");

		return "int";
	}

	public String visit(MessageSend n, String argu)
	{
		// messageSend -> class.method( expressionList )

		// get class
		String part1 = n.f0.accept( this, null);
		if ( !part1.equals("this") && !isClass(part1) )
			throw new MyException("Error - cant have a message send expression from a non class variable");

		// get type of method
		this.searchMethod = true;
		String part2 = n.f2.accept( this, part1);
		this.searchMethod = false;

		// get name of method
		this.inTC = false;
		String methodName = n.f2.accept( this, null);
		this.inTC = true;

		// get all the types of the parameters of the method from the symbol table
		// store all of them in a large string (this.realExpressionList)
		// compare them with the ones at the message send expression
		// they must be of the same number and of the same type each
		// we store the old this.realExpressionList in a local variable in case we have recursive message send expressions
		String realExpressionListHolder = this.realExpressionList;
		this.realExpressionList = getParametersOfMethod(part1, methodName);

		if ( n.f4.present())
		{
			n.f4.accept( this, null);
		}

		if ( this.realExpressionList.length() > 0)
			throw new MyException("Error - wrong number of arguements at message send expression");

		this.realExpressionList = realExpressionListHolder;

		// return the type of the method
		return part2;
	}

	public String visit(ExpressionList n, String argu)
	{
		// this is the code for the first parameter in the expression list

		// if this.realExpressionList is empty, this means the method should take no arguements but the message send expression had arguements - error
		if ( this.realExpressionList.length() == 0)
			throw new MyException("Error - wrong number of arguements at message send expression");

		// get the first type from this.realExpressionList
		int index = 0;
		while ( this.realExpressionList.charAt(index) != ' ')
			index++;

		String wantedType = this.realExpressionList.substring(0,index);
		this.realExpressionList = this.realExpressionList.substring(index+1,this.realExpressionList.length());

		// get the first type from the message send expression arguement list
		// if the type dont match, throw an error
		String currentType = n.f0.accept(this,null);
		if (currentType.equals("this"))
			currentType = this.currentClass;
		if ( !currentType.equals(wantedType) && !isChild(currentType,wantedType) )
			throw new MyException("Error - The type of an arguement in a message send expression must be the same type of the arguement defined in the method");

		// visit the rest parameters from the message send expression
		n.f1.accept( this, null);

		return null;
	}

	public String visit(ExpressionTail n, String argu)
	{
		if ( n.f0.present())
		{
			n.f0.accept( this, null);
		}

		return null;
	}

	public String visit(ExpressionTerm n, String argu)
	{
		// this code is for each and every one of the rest parameters of the message send arguement list
		// we follow exactly the same logic with the ExpressionList visit function

		if ( this.realExpressionList.length() == 0)
			throw new MyException("Error - wrong number of arguements at message send expression");

		int index = 0;
		while (this.realExpressionList.charAt(index) != ' ')
			index++;

		String wantedType = this.realExpressionList.substring(0,index);
		this.realExpressionList = this.realExpressionList.substring(index+1,this.realExpressionList.length());

		String currentType = n.f1.accept(this,null);
		if ( currentType.equals("this"))
			currentType = this.currentClass;
		if ( !currentType.equals(wantedType) && !isChild(currentType,wantedType) )
			throw new MyException("Error - The type of an arguement in a message send expression must be the same type of the arguement defined in the method");

		return null;
	}

	public String visit(Clause n, String argu)
	{
		return n.f0.accept( this, null);
	}

	public String visit(PrimaryExpression n, String argu)
	{
		return n.f0.accept( this, null);
	}

	public String visit(IntegerLiteral n, String argu)
	{
		return "int";
	}

	public String visit(TrueLiteral n, String argu)
	{
		return "boolean";
	}

	public String visit(FalseLiteral n, String argu)
	{
		return "boolean";
	}

	public String visit(Identifier n, String argu)
	{
		// if inTC is true, return type of identifier
		// if inTC is false, return name of identifier
		if ( this.inTC)
		{
			// is searchMethod is true, search for method with name of the identifier given and return its type
			// argu in this case will be the name of the class in which the method should exist
			// basically, this.searchMethod is used only for the message send expressions
			if ( this.searchMethod)
				return getMethodType( argu, n.f0.toString());
			// if search Method is false, search for variable with name of the identifier given and return its type
			else
				return getVarType( this.currentClass, this.currentMethod, n.f0.toString());
		}
		else
			return n.f0.toString();
	}

	public String visit(ThisExpression n, String argu)
	{
		return "this";
	}

	public String visit(ArrayAllocationExpression n, String argu)
	{
		return n.f0.accept( this, null);
	}

	public String visit(IntegerArrayAllocationExpression n, String argu)
	{
		// check is the type of the size of the array to be allocated is integer
		if ( !n.f3.accept( this, null).equals("int"))
			throw new MyException("Error - cant have an array definition with length of non integer type");

		return "int[]";
	}

	public String visit(BooleanArrayAllocationExpression n, String argu)
	{
		// check is the type of the size of the array to be allocated is integer
		if ( !n.f3.accept( this, null).equals("int"))
			throw new MyException("Error - Cant have an array definition with length of non integer type");

		return "boolean[]";
	}

	public String visit(AllocationExpression n, String argu)
	{
		// check if the class to be allocated exists
		this.inTC = false;
		String className = n.f1.accept( this, null);
		this.inTC = true;

		if ( !isClass( className))
			throw new MyException("Error - Identifier at class objext allocation expression does not exist");

		return className;
	}

	public String visit(NotExpression n, String argu)
	{
		// not expression must be boolean and not operator must also be applied to boolean expressions
		if ( !n.f1.accept(this, null).equals("boolean"))
			throw new MyException("Error - Cant have a not expression of non boolean type");

		return "boolean";
	}

	public String visit(BracketExpression n, String argu)
	{
		return n.f1.accept( this, null);
	}
}
