public class nod
{
    public static void main(String[] args) { 
	    int first_num = 11;
	    int second_num = 33;
	    while (first_num != second_num)
	        if(first_num > second_num)
	            first_num = first_num - second_num;
            else 
                second_num = second_num - first_num;
		System.out.println(first_num);
	}
}
