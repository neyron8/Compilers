public class MainClass
{
	public static void main(String[] args)
	{
		int[] array = new int[5];
		int value_of_array = 0;
		while(value_of_array < 5){
			array[value_of_array] = value_of_array + 1;
			value_of_array++;   
		}
		int min_element_of_array = array[0];
		int curr_pos_of_array = 0;
		while(curr_pos_of_array < 5)
		{
			if (array[curr_pos_of_array] < min_element_of_array)
				min_element_of_array = array[curr_pos_of_array];
			curr_pos_of_array++;
		}
		System.out.println(min_element_of_array);
	}
}

