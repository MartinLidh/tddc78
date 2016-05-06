import java.io.*;

public class SLOG_Preview extends SLOG_String
{
    public SLOG_Preview( RandomAccessFile file_stm )
    throws IOException
    {
        super.ReadFromRandomFile( file_stm );
    }

    public SLOG_Preview( RandomAccessFile file_stm, long fileloc )
    throws IOException
    {
        super.ReadFromRandomFileAt( file_stm, fileloc );
    }
}
