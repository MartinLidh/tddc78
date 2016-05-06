import java.io.*;

public class ts_slog_thread2
{
    String filename = "slogfile.data";
    SLOG_Thread slogth;

    public ts_slog_thread2()
    throws IOException
    {
        slogth = new SLOG_Thread( filename );

        Runnable doReadHdr = new Runnable()
        {
            public void run()
            {
                try { slogth.ReadIstm(); }
                catch ( IOException err ) 
                    { System.out.println( "slogth.ReadIstm() has IO err" ); }
            }
        };

        Runnable doPrint = new Runnable() 
        {
            public void run() 
            {
                System.out.println( slogth.toString() );
            }
        };

        Runnable doReadFrames = new Runnable() 
        {
            public void run() 
            {
                for ( int idx = 0; idx < slogth.NumOfFrames(); idx++ ) {
                    if ( idx % 2 == 0 ) {
                        try {
                            slogth.ReadFrame( idx );
                            System.out.println( "SLOG_Frame No. " 
                                              + idx + ": \n" );
                            System.out.println( slogth.toString() );
                        } catch ( IOException err ) {
                            System.out.println(
                            "slogth.ReadFrame() has IO err" );
                        }
                    }
                }
            }
        };

        Thread slog_readframes = new Thread( doReadFrames );
        slog_readframes.start();
        Thread slog_init = new Thread( doReadHdr );
        slog_init.start();
        Thread slog_print = new Thread( doPrint );
        slog_print.start();
    }

    public static void main( String argv[] ) throws IOException
    {
         ts_slog_thread2 pgm = new ts_slog_thread2();
    }
}
