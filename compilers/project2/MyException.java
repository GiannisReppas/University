public class MyException extends Error
{
	String msg;

	public MyException(String temp) throws MyException
	{
		this.msg = temp;
	}

	public String getMessage()
	{
		return "\n\n"+this.msg+"\n\n";
	}
}
