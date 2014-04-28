import java.io.Console;
public class Try
{
	public static void main(String args[])
	{
        Console console = System.console();
        if (console == null) {
      System.err.println("sales: unable to obtain console");
      return;
    }

    String username = console.readLine("Enter username: ");
    System.out.println(username);
		System.out.println("wgg");
	}
};
