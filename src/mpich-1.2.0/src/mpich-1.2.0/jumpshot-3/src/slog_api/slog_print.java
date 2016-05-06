import java.io.*;

public class slog_print
{
    public static void main( String argv[] ) throws IOException
    {
        String filename = "slogfile.data";
        long fptr2frame = 0;

        if ( argv.length >= 1 ) filename = argv[ 0 ];

        SLOG_InputStream slog = new SLOG_InputStream( filename );
        System.out.println( "\t __SLOG_InputStream__ \n" );
        System.out.println( slog );

        for ( int idx = 0; idx < slog.dir.NumOfFrames(); idx++ ) {
            if ( idx % 1 == 0 ) {
                fptr2frame = slog.dir.EntryAt( idx ).fptr2framehdr;
                System.out.println( "Main : fptr2frame = " + fptr2frame );
                SLOG_Frame slog_frame = slog.GetFrame( fptr2frame );
                System.out.println( "SLOG_Frame No. " + idx + ": \n" );
                System.out.println( slog_frame );
            }
        }

    }
}
