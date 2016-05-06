import java.io.*;

public class SLOG_String
{
    String str;

    public SLOG_String()
    {
        str = new String();
    }

    public SLOG_String( byte [] bytes )
    {
        str = new String( bytes );
    }

    public SLOG_String( char [] values )
    {
        str = new String( values );
    }

    public int ReadFromRandomFileAt( RandomAccessFile file_stm, long fileloc )
    throws IOException
    {
        file_stm.seek( fileloc );
        return( ReadFromRandomFile( file_stm ) );
    }

    public int ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        int Nchar    = file_stm.readInt();
        byte bytes[] = new byte[ Nchar ];
        file_stm.readFully( bytes );
        str = new String( bytes );

        return Nchar;
    }

    public int WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        int Nchar = str.length();
        file_stm.writeInt( Nchar );
        file_stm.writeBytes( str );

        return Nchar;
    }

    public String toString()
    {
        return str;
    }
}
