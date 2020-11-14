public class ParseError extends Exception
{
	private String info;

	public ParseError()
	{
		this.info = "\nParse Error";
	}

	public void showInfo()
	{
		System.out.println(this.info);
	}
}