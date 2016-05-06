import java.io.*;

public class ts_slog_thread3
{
    public static void main( String argv[] ) throws IOException
    {
        String filename = "slogfile.data";
        SLOG_Thread slogth = new SLOG_Thread( filename );
        new Thread( slogth ).start();
        
        for ( int idx = 0; idx < slogth.NumOfFrames(); idx++ ) {
            new Thread( new SLOG_Thread( slogth.GetSlogIstm(), idx ) ).start();
        }
    }
}
