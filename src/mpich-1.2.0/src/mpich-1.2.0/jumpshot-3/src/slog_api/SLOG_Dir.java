import java.util.*;
import java.io.*;


public class SLOG_Dir
{
    private  SLOG_DirHdr        hdr;
    private  Vector             entries;  // Vector of SLOG_DirEntry

    public SLOG_Dir()
    {
        hdr        = new SLOG_DirHdr();
        entries    = new Vector();
    }

    public SLOG_Dir( int in_Nframe )
    {
        hdr        = new SLOG_DirHdr();
        hdr.Nframe = in_Nframe;
        entries    = new Vector( hdr.Nframe );
    }

    public SLOG_Dir( RandomAccessFile file_stm )
    throws IOException
    {
        hdr        = new SLOG_DirHdr();
        entries    = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public int NumOfFrames()
    {
        return hdr.Nframe;
    }

    public long FptrToPrevDir()
    {
        return hdr.prevdir;
    }

    public long FptrToNextDir()
    {
        return hdr.nextdir;
    }

    public double StartTime()
    {
        return ( (SLOG_DirEntry) entries.firstElement() ).starttime;
    }

    public double EndTime()
    {
        return ( (SLOG_DirEntry) entries.lastElement() ).endtime;
    }

    public SLOG_DirEntry EntryAt( int idx )
    {
        try {
            return( (SLOG_DirEntry) entries.elementAt( idx ) );
        }
        catch ( ArrayIndexOutOfBoundsException e ) {
            return null;
        }
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        hdr.ReadFromRandomFile( file_stm );
        for ( int ii = 0; ii < hdr.Nframe; ii++ ) {
            SLOG_DirEntry entry = new SLOG_DirEntry( file_stm );
            entries.addElement( entry );
        }
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        hdr.WriteToRandomFile( file_stm );

        Enumeration enum = entries.elements();
        while( enum.hasMoreElements() )
            ( (SLOG_DirEntry) enum.nextElement() )
            .WriteToRandomFile( file_stm );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer(
                                          "\t Directory :\n" );
        representation.append( hdr + "\n" );

        Enumeration enum = entries.elements();
        while( enum.hasMoreElements() )
            representation.append( (SLOG_DirEntry) enum.nextElement()
                                 + "\n" );

        return( representation.toString() );
    }
}
