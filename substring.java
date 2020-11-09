public class MainClass
{
    public static void main(String[] args)
    {
        String full_str;
        full_str = "Welcome";
        String substr;
        substr = "om";
        int i;
        i = 0;
        char first;
        char second;
        while( 7 > i + 1){
            first = full_str.charAt(i);
            second = full_str.charAt(i+1);
            char test;
            if(first == substr.charAt(0)){
                if(second == substr.charAt(1)){
                    System.out.println(0);
                }
            }
            i++;
        }
    }
}
