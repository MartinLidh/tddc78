import java.io.*;
import java.util.*;

public class SLOG_Profile
{
    Vector entries;  // Vector of SLOG_ProfileEntry

    public SLOG_Profile()
    {
        entries  = new Vector( 0 );
    }

    public SLOG_Profile( RandomAccessFile file_stm )
    throws IOException, NoSuchElementException
    {
        entries  = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_Profile( RandomAccessFile file_stm, long fileloc )
    throws IOException, NoSuchElementException
    {
        file_stm.seek( fileloc );
        entries  = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException, NoSuchElementException
    {
        int Nbytes_read = 0;
        int Nentries    = file_stm.readInt(); // No. of entries in SLOG_Profile

        for ( int ii = 0; ii < Nentries; ii++ ) {
            String line = file_stm.readLine();
            Nbytes_read += line.length() + 1;
            if ( line.charAt( 0 ) != '#' ) {
                SLOG_ProfileEntry entry = new SLOG_ProfileEntry( line );
                entries.addElement( entry );
                // System.out.println( entry );
                entry = null;
            }
            line = null;
        }
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer( "SLOG_Profile :\n" );
        Enumeration enum = entries.elements();
        while ( enum.hasMoreElements() ) 
            representation.append( enum.nextElement() + "\n" );
        return representation.toString();
    }
}
