public class MainClass
{
	public static void main(String[] args)
	{
		int[] array = { 5,5,4,1,7 };
    int value_of_array;
		value_of_array = 0;
		int min_element_of_array;
		min_element_of_array = array[0];
		int curr_pos_of_array;
    curr_pos_of_array = 0;
		while (curr_pos_of_array < 5)
		{
			if (min_element_of_array < array[curr_pos_of_array]){
				min_element_of_array = array[curr_pos_of_array];
			}
			curr_pos_of_array++;
		}
		System.out.println(min_element_of_array);
	}
}
