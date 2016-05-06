import java.io.*;

public class SLOG_Buffer
{
    private int                   buffer_bsize;
    private byte[]                buffer;
    private RandomAccessFile      file_stm;
    private DataInputStream       data_istm;
    private long                  file_ptr;

    public SLOG_Buffer()
    {
        buffer_bsize = 0;
        buffer       = new byte[ buffer_bsize ];
        file_stm     = null;
        data_istm    = null;
        file_ptr     = 0;
    }

    public SLOG_Buffer( int bytesize )
    {
        buffer_bsize = bytesize;
        buffer       = new byte[ buffer_bsize ];
        file_stm     = null;
        data_istm    = null;
        file_ptr     = 0;
    }

    public SLOG_Buffer( RandomAccessFile in_file_stm, int bytesize )
    throws IOException
    {
        buffer_bsize = bytesize;
        buffer       = new byte[ buffer_bsize ];
        file_stm     = in_file_stm;
        data_istm    = null;
        file_ptr     = file_stm.getFilePointer();
    }

    public void SetFileStream( RandomAccessFile in_file_stm )
    {
        file_stm = in_file_stm;
    }

    public void Seek( long pos )
    {
        file_ptr = pos;
    }

    public long GetFilePointer()
    {
        return( file_ptr );
    }

    public DataInputStream GetDataInputStream()
    {
        return( data_istm );
    }

    public void ReadFrame()
    throws IOException
    {
        ByteArrayInputStream ary_stream;

        if ( file_stm != null ) {
            if ( file_ptr > 0 ) {
                file_stm.seek( file_ptr );
                // Reuse the existing byte buffer
                file_stm.readFully( buffer );
                ary_stream = new ByteArrayInputStream( buffer );
                data_istm = new DataInputStream( ary_stream );
            }
            else
                throw new IOException( "file_ptr = " + file_ptr );
        }
        else
            throw new IOException( "file_stm == null" );
    }

    public void ReadFrame( long pos )
    throws IOException
    {
        this.Seek( pos );
        this.ReadFrame();
    }
}
