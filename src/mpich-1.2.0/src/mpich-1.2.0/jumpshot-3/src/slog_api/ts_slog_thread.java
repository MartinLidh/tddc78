import java.io.*;

public class ts_slog_thread
{
    public static void main( String argv[] ) throws IOException
    {
        String filename = "../logfiles/slogfile.data";
        slog_thread slogth;
        
        slogth = new slog_thread( filename );
        slogth.ReadLogFile();
        System.out.println( "\t __SLOG_InputStream__ \n" );
        System.out.println( slogth.toString() );

        for ( int idx = 0; idx < slogth.NumOfFrames(); idx++ ) {
            if ( idx % 2 == 0 ) {
                slogth.ReadLogFile( idx );
                System.out.println( "SLOG_Frame No. " + idx + ": \n" );
                System.out.println( slogth.toString() );
            }
        }
    }
}
