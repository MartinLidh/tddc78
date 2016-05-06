import java.io.*;

public class ts_profile
{
    public static void main( String argv[] ) throws IOException
    {
        RandomAccessFile  file_stm
        = new RandomAccessFile( "../test/SLOG_Profile.txt", "r" );
        SLOG_Profile      profile = new SLOG_Profile( file_stm );
    }
}
