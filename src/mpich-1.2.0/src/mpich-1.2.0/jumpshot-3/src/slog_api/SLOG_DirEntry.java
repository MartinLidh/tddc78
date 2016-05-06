import java.io.*;

public class SLOG_DirEntry
{
    public long    fptr2framehdr;
    public double  starttime;
    public double  endtime;

    public SLOG_DirEntry()
    {
        fptr2framehdr = SLOG_Const.NULL_fptr;
        starttime     = SLOG_Const.INVALID_double;
        endtime       = SLOG_Const.INVALID_double;
    }

    public SLOG_DirEntry( long   in_fptr, double in_stime, double in_etime )
    {
        fptr2framehdr = in_fptr;
        starttime     = in_stime;
        endtime       = in_etime;
    }

    public SLOG_DirEntry( RandomAccessFile file_stm )
    throws IOException
    {
        this.ReadFromRandomFile( file_stm );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        fptr2framehdr = file_stm.readLong();
        starttime     = file_stm.readDouble();
        endtime       = file_stm.readDouble();
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        file_stm.writeLong( this.fptr2framehdr );
        file_stm.writeDouble( this.starttime );
        file_stm.writeDouble( this.endtime );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();
        representation.append( fptr2framehdr + " | " );
        representation.append( starttime + " -> " + endtime );
        return( representation.toString() );
    }
}
