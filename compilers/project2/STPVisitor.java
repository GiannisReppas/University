import syntaxtree.*;
import visitor.GJDepthFirst;

import java.io.*;
import java.util.*;

public class STPVisitor extends GJDepthFirst<String, String>
{
	// class used to save class fields, method parameters and variables
	public class AttributeNode
	{
		String type;
		String name;
		boolean offsetInclude;
	}

	// class used to save class methods
	public class MethodNode
	{
		String type;
		String name;
		boolean offsetInclude;
		LinkedList<AttributeNode> parameters = new LinkedList<AttributeNode>();
		LinkedList<AttributeNode> variables = new LinkedList<AttributeNode>();
	}

	// class used to save classes
	public class ClassNode
	{
		String name;
		boolean extendsClass;
		LinkedList<String> children = new LinkedList<String>();
		LinkedList<AttributeNode> attributes = new LinkedList<AttributeNode>();
		LinkedList<MethodNode> methods = new LinkedList<MethodNode>();
	}

	// linked list used to save all classNodes of the symbol table
	// this is basically the symbol table, a linked list of classes
	LinkedList<ClassNode> classes = new LinkedList<ClassNode>();

	// flag variable used to know when we are inside a function declaration
	boolean inFunction = false;

	// linked list used for the calculation of offsets
	LinkedList<String> offsetPrintList = new LinkedList<String>();

	///////////////////////////////////////////////////////////////////////////////////////

	// inserts a new class node at the end of the symbol table
	public void insertClass( String name, boolean extendsClass)
	{
		int i;
		for ( i=0; i < this.classes.size() ; i++)
			if ( this.classes.get(i).name.equals(name) )
				throw new MyException("Error - Class "+name+" has been already declared");

		ClassNode temp = new ClassNode();
		temp.name = name;
		temp.extendsClass = extendsClass;

		this.classes.add(temp);
	}

	// inserts a new variable in the last class of the symbol table as a field
	public void insertAttributeInClass( String type, String name, boolean offsetInclude)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).attributes.size(); i++)
			if ( this.classes.get( this.classes.size()-1 ).attributes.get(i).name.equals(name) )
				throw new MyException("Error - Class "+this.classes.get( this.classes.size()-1 ).name+" cannot have 2 attributes with the same name" );

		AttributeNode temp = new AttributeNode();
		temp.type = type;
		temp.name = name;
		temp.offsetInclude = offsetInclude;

		this.classes.get( this.classes.size()-1 ).attributes.add(temp);
	}

	// inserts a new method in the last class of the symbol table
	public void insertMethodInClass( String type, String name, boolean offsetInclude)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).methods.size(); i++)
			if ( this.classes.get( this.classes.size()-1 ).methods.get(i).name.equals(name) )
				throw new MyException("Error - Class "+this.classes.get( this.classes.size()-1 ).name+" cannot have 2 methods with the same name" );

		MethodNode temp = new MethodNode();
		temp.type = type;
		temp.name = name;
		temp.offsetInclude = offsetInclude;

		this.classes.get( this.classes.size()-1 ).methods.add(temp);
	}

	// insers a new variable in the last method in the last class of the symbol table as a parameter
	public void insertParameterInMethod( String type, String name, boolean offsetInclude)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).parameters.size(); i++ )
			if ( this.classes.get( this.classes.size()-1).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).parameters.get(i).name.equals(name) )
				throw new MyException("Error - A method cant have 2 or more parameters with the same name");

		AttributeNode temp = new AttributeNode();
		temp.type = type;
		temp.name = name;

		this.classes.get( this.classes.size()-1 ).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).parameters.add(temp) ;
	}

	// inserts a new variable in the last method in the last class of the symbol table as a variable
	public void insertVariableInMethod( String type, String name, boolean offsetInclude)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).parameters.size(); i++ )
			if ( this.classes.get( this.classes.size()-1).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).parameters.get(i).name.equals(name) )
				throw new MyException("Error - A method cant have a variable with the same name as a parameter");

		for ( i=0; i < this.classes.get( this.classes.size()-1 ).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).variables.size(); i++ )
			if ( this.classes.get( this.classes.size()-1).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).variables.get(i).name.equals(name) )
				throw new MyException("Error - A method cant have 2 or more variables with the same name");

		AttributeNode temp = new AttributeNode();
		temp.type = type;
		temp.name = name;
		temp.offsetInclude = offsetInclude;

		this.classes.get( this.classes.size()-1 ).methods.get(  this.classes.get(this.classes.size()-1).methods.size()-1  ).variables.add(temp);
	}

	// returns the index of a class in the symbol table
	// has this name because it is used at the point where we need to get tha index of the extended class
	// in a "class B extends A declaration"
	public int getExtendedIndex( String className)
	{
		int i;
		for (i = 0; i < this.classes.size(); i++)
			if ( this.classes.get(i).name.equals(className) )
			{
				if ( i == (this.classes.size()-1))
					throw new MyException("Error - A class cant extend itsself");
				return i;
			}

		throw new MyException("Error - Extended class has not been declared");
	}

	// returns true is the string given is a name of a field in the last class of the symbol table
	public boolean attributeExistsInClass( String name)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).attributes.size(); i++)
			if ( this.classes.get( this.classes.size()-1 ).attributes.get(i).name.equals(name) )
				return true;

		return false;
	}

	// returns true if the string given is a name of a method in the last class of the symbol table
	public boolean methodExistsInClass( String name)
	{
		int i;
		for ( i=0; i < this.classes.get( this.classes.size()-1 ).methods.size(); i++)
			if ( this.classes.get( this.classes.size()-1 ).methods.get(i).name.equals(name) )
				return true;

		return false;
	}

	// gets the index of a class in the symbol table and copies all of the info of that class and copies it into the last class of the symbol table
	// this function is basically called at "each class B extends A" declaration in order to copy the info from class A that needs to be inherited into class B
	// in case class B has overloaded methods, these methods wont be copied/inherited from class A to class B
	public void copyInfo( int extendedIndex)
	{
		int i,j,k;

		// copy attributes
		for ( i=0; i < this.classes.get(extendedIndex).attributes.size(); i++)
			if ( !attributeExistsInClass( this.classes.get(extendedIndex).attributes.get(i).name) )
				insertAttributeInClass( this.classes.get(extendedIndex).attributes.get(i).type, this.classes.get(extendedIndex).attributes.get(i).name, false);

		// copy methods
		for ( i=0; i < this.classes.get(extendedIndex).methods.size(); i++)
		{
			// if the method does not exist in the final class, copy it with all its parameters and variables into the last class of the symbol table
			if ( !methodExistsInClass( this.classes.get(extendedIndex).methods.get(i).name) )
			{
				insertMethodInClass( this.classes.get(extendedIndex).methods.get(i).type, this.classes.get(extendedIndex).methods.get(i).name, false);

				for ( j=0; j < this.classes.get(extendedIndex).methods.get(i).parameters.size(); j++)
					insertParameterInMethod( this.classes.get(extendedIndex).methods.get(i).parameters.get(j).type, this.classes.get(extendedIndex).methods.get(i).parameters.get(j).name, false);

				for ( j=0; j < this.classes.get(extendedIndex).methods.get(i).variables.size(); j++)
					insertVariableInMethod( this.classes.get(extendedIndex).methods.get(i).variables.get(j).type, this.classes.get(extendedIndex).methods.get(i).variables.get(j).name, false);
			}
			// if method is overloaded from class A to class B (basically already exists in class B), check if is overloaded correctly, with the way miniJava requires
			// which means same return type and same arguements
			else
			{
				for ( j=0; j < this.classes.get( this.classes.size()-1 ).methods.size(); j++)
				{
					if ( this.classes.get( this.classes.size()-1 ).methods.get(j).name.equals( this.classes.get(extendedIndex).methods.get(i).name ) )
					{
						if ( this.classes.get( this.classes.size()-1 ).methods.get(j).type.equals( this.classes.get(extendedIndex).methods.get(i).type ) &&
							(this.classes.get( this.classes.size()-1 ).methods.get(j).parameters.size() == this.classes.get(extendedIndex).methods.get(i).parameters.size()) )
						{
							for ( k=0; k < this.classes.get( this.classes.size()-1 ).methods.get(j).parameters.size(); k++)
							{
								if ( !this.classes.get( this.classes.size()-1 ).methods.get(j).parameters.get(k).type.equals( this.classes.get(extendedIndex).methods.get(i).parameters.get(k).type ) )
									throw new MyException("Error - Inherently polymorphic functions must have same arguements and return type with the parent functions");
							}
						}
						else
							throw new MyException("Error - Inherently polymorphic functions must have same arguements and return type with the parent functions");

						this.classes.get( this.classes.size()-1 ).methods.get(j).offsetInclude = false;
						break;
					}
				}
			}
		}
	}

	// adds a child into the list of children of a class in the symbol table
	public void addChild( String className, String child)
	{
		// add child in the children list of className
		// add child in the children list of every class that has className as child
		int i;
		for ( i=0; i < this.classes.size(); i++)
		{
			if ( this.classes.get(i).name.equals(className) || this.classes.get(i).children.contains(className))
				this.classes.get(i).children.add( child);
		}
	}

	// this function is called recursively
	// first, using the current variable offset count (varOffsets) and the current method offset count (methodOffsets)
	// then, remove the class for which the offsets where printed from the offsetPrintList
	// and call the function again for every child the class has and it also belongs the offsetPrintList
	// in case the class has grandchildren, the grandchildren will have been removed from the offsetPrintList before the function has been called for them
	public void printInheritanceTree( int classIndex, int varOffsets, int methodOffsets)
	{
		int i,j;
		for ( i=0; i < this.classes.get(classIndex).attributes.size(); i++)
		{
			if ( this.classes.get(classIndex).attributes.get(i).offsetInclude)
			{
				System.out.println(this.classes.get(classIndex).name + "." + this.classes.get(classIndex).attributes.get(i).name +" : "+ varOffsets);
				if ( this.classes.get(classIndex).attributes.get(i).type.equals("boolean"))
					varOffsets += 1;
				else if ( this.classes.get(classIndex).attributes.get(i).type.equals("int"))
					varOffsets += 4;
				else
					varOffsets += 8;
			}
		}

		for ( i=0; i < this.classes.get(classIndex).methods.size(); i++)
		{
			if ( this.classes.get(classIndex).methods.get(i).offsetInclude)
			{
				System.out.println(this.classes.get(classIndex).name + "." + this.classes.get(classIndex).methods.get(i).name +" : "+ methodOffsets);
				methodOffsets += 8;
			}
		}

		this.offsetPrintList.remove( this.classes.get(classIndex).name );

		for ( i=0; i < this.classes.get(classIndex).children.size(); i++)
		{
			for ( j=0; j < this.classes.size(); j++)
				if ( this.classes.get(j).name.equals(this.classes.get(classIndex).children.get(i)) )
				{
					if ( this.offsetPrintList.contains( this.classes.get(j).name) )
						printInheritanceTree(j, varOffsets, methodOffsets);
					break;
				}
		}
	}

	// prints the offsets of the symbol table for every class
	public void printOffsets()
	{
		// first, initalize the print list
		// classes have in the children list class names that are not only their children, but also their grandchildren etc..
		// so in order to print the offsets only one time for each class we use this offset print list
		// each time offsets from a classs are printed, the class is removed from the print list
		int i;
		for( i=0; i < this.classes.size(); i++)
			this.offsetPrintList.add( this.classes.get(i).name );

		// for every class, if it is NOT a class that extends another one, call function printInheritanceTree
		for ( i=0; i < this.classes.size(); i++)
			if ( !this.classes.get(i).extendsClass)
				if ( this.offsetPrintList.contains( this.classes.get(i).name ))
					printInheritanceTree( i, 0, 0);
	}

	///////////////////////////////////////////////////////////////////////////////////

	public String visit( MainClass n, String argu)
	{
		// add the special class in the symbol table
		String mainClassName = n.f1.accept( this, null);
		insertClass( mainClassName, false);

		// add the main function the symbol table
		insertMethodInClass( "void", "main", false);

		// add String[] args in the symbol table
		String mainFunctionArgsName = n.f11.accept( this, null);
		insertParameterInMethod( "String[]", mainFunctionArgsName, false);

		// visit variable declarations in the main function
		if ( n.f14.present())
		{
			this.inFunction = true;
			n.f14.accept( this, "main");
			this.inFunction = false;
		}

		return null;
	}

	public String visit( ClassDeclaration n, String argu)
	{
		// add the class in the symbol table
		String className = n.f1.accept( this, null);
		insertClass( className, false);

		// visit fields of the class
		if ( n.f3.present())
		{
			n.f3.accept( this, null);
		}

		// visit methods of the class
		if ( n.f4.present())
		{
			n.f4.accept( this, null);
		}

		return null;
	}

	public String visit( ClassExtendsDeclaration n, String argu)
	{
		// add the class in the symbol table
		String className = n.f1.accept( this, null);
		String extendedClass = n.f3.accept( this, null);
		insertClass( className, true);

		// visit fields of the class
		if ( n.f5.present())
		{
			n.f5.accept( this, null);
		}

		// visit methods of the class
		if ( n.f6.present())
		{
			n.f6.accept( this, null);
		}

		// add the class name in the children list of the class that gets extended
		// if the extended class is NOT the special class, copy also its info in the new class in the symbol table
		int extendedIndex = getExtendedIndex( extendedClass);
		addChild( extendedClass, className);
		if ( extendedIndex != 0)
			copyInfo( extendedIndex);

		return null;
	}

	public String visit( MethodDeclaration n, String argu)
	{
		// insert method in the symbol table
		String type = n.f1.accept( this, null);
		String name = n.f2.accept( this, null);
		insertMethodInClass( type, name, true);

		// visit method's parameters
		if ( n.f4.present())
		{
			n.f4.accept( this, null);
		}

		// visit method's variable
		// set inFunction flag to true so the variable declarations will be handled differently
		if ( n.f7.present())
		{
			this.inFunction = true;
			n.f7.accept( this, null);
			this.inFunction = false;
		}

		return null;
	}

	public String visit( FormalParameter n, String argu)
	{
		// insert method parameter in symbol table
		String type = n.f0.accept( this, null);
		String name = n.f1.accept( this, null);
		insertParameterInMethod( type, name, false);

		return null;
	}

	public String visit( VarDeclaration n, String argu)
	{
		// insert variable in the symbol table
		// call different function to do it, based on the location we found the variable declaration
		String type = n.f0.accept( this, null);
		String name = n.f1.accept( this, null);

		if ( this.inFunction)
			insertVariableInMethod( type, name, false);
		else
			insertAttributeInClass( type, name, true);

		return null;
	}

	public String visit( Identifier n, String argu)
	{
		return n.f0.toString();
	}

	public String visit( Type n, String argu)
	{
		return n.f0.accept( this, null);
	}

	public String visit( ArrayType n, String argu)
	{
		 return n.f0.accept( this, null);
	}

	public String visit( IntegerArrayType n, String argu)
	{
		return n.f0.toString() + n.f1.toString() + n.f2.toString();
	}

	public String visit( BooleanArrayType n, String argu)
	{
		return n.f0.toString() + n.f1.toString() + n.f2.toString();
	}

	public String visit( BooleanType n, String argu)
	{
		return n.f0.toString();
	}

	public String visit( IntegerType n, String argu)
	{
		return n.f0.toString();
	}
}
