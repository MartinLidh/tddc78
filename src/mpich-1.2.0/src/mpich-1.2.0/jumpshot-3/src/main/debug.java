import java.io.*;

public class debug
{
    private static boolean msg_on = false;
    private static String filename = null;

    public static void SetMessageOn() { msg_on = true; }
    public static void SetMessageOff() { msg_on = false; }

    public static void SetFilename( String in_name )
    throws IOException
    {
        filename = new String( in_name );
        System.setOut( new PrintStream( new FileOutputStream(in_name) ) );
    }

    public static void print( String x )
    {
        if ( msg_on ) System.out.print( x );
    }

    public static void println( String x )
    {
        if ( msg_on ) System.out.println( x );
    }
}
