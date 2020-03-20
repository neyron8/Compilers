public class MyClass
{
	public static void main(String args[])
	{
		int position_of_full_str = 0;
		int position_of_substr = 0;
		String substr = "java";
		int size_substr = 4;
		String full_str = "ai111java";
		int size_full_str = 9;
		while (position_of_substr <= size_full_str - size_substr)
		{
			while(position_of_full_str < size_substr &&
			substr.charAt(position_of_full_str) ==
			full_str.charAt(position_of_full_str + position_of_substr))
				position_of_full_str++;
			if (position_of_full_str >= size_substr)
				System.out.println(position_of_substr);
			position_of_substr++;
		}
	}
}




