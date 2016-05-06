import java.io.*;

public class SLOG_DirHdr
{
    public long     prevdir;
    public long     nextdir;
    public int      Nframe;

    public SLOG_DirHdr()
    {
        prevdir = SLOG_Const.NULL_fptr;
        nextdir = SLOG_Const.NULL_fptr;
        Nframe  = 0;
    }

    public SLOG_DirHdr( long in_prevdir, long in_nextdir, int in_Nframe )
    {
        prevdir = in_prevdir;
        nextdir = in_nextdir;
        Nframe  = in_Nframe;
    }

    public SLOG_DirHdr( RandomAccessFile file_stm )
    throws IOException
    {
        this.ReadFromRandomFile( file_stm );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        prevdir = file_stm.readLong();
        nextdir = file_stm.readLong();
        Nframe  = file_stm.readInt();
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        file_stm.writeLong( this.prevdir );
        file_stm.writeLong( this.nextdir );
        file_stm.writeInt( this.Nframe );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();
        representation.append( "PrevDir at " + prevdir + ",   " );
        representation.append( "NextDir at " + nextdir + ",   " );
        representation.append( "Nframe = " + Nframe );
        return( representation.toString() );
    }
}
