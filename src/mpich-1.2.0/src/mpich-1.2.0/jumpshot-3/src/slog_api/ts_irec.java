import java.io.*;

public class ts_irec
{
    public static void main( String[] argv ) throws IOException
    {
        SLOG_Irec_min irec = new SLOG_Irec_min( (short)30, (short)100, 
                                                (short)101, (byte)1, (byte)1, 
                                                0.5, 1.0, (short)99, (byte)0, 
                                                1000, 0 );
        System.out.println( irec.toString() );
    }
}
