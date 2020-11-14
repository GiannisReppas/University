import java.io.*;
import syntaxtree.*;
import java.io.*;

public class Main
{
	public static void main(String args[])
	{
		if (args.length == 0)
		{
			System.err.println("Usage: java Main <inputFile1> <inputFile2> ...");
			System.exit(1);
		}

		FileInputStream fis = null;

		// for every file
		int j;
		for ( j=0; j < args.length; j++)
		{
			try
			{
				fis = new FileInputStream(args[j]);

				// parser
				MiniJavaParser parser = new MiniJavaParser(fis);
				Goal root = parser.Goal();

				// symbol table visitor
				STPVisitor stpv = new STPVisitor();
				root.accept(stpv, null);

				// type checking visitor
				TCVisitor tcv = new TCVisitor( stpv);
				root.accept(tcv, null);

				// print offsets
				stpv.printOffsets();

				// end
				System.out.println("");
			}
			catch (ParseException ex)
			{
				System.out.println(ex.getMessage());
			}
			catch (FileNotFoundException ex)
			{
				System.out.println(ex.getMessage());
			}
			catch (MyException ex)
			{
				System.out.println(ex.getMessage());
			}
			finally
			{
				try
				{
					if ( fis != null)
						fis.close();
				}
				catch(IOException ex)
				{
					System.err.println(ex.getMessage());
				}
			}
		}
	}
}
