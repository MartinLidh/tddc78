import java.io.*;
import java.util.*;

public class SLOG_FrameSector
{
    private int     bytesize;          // Byte Size of the sector in file
    private int     Nrec;              // Number of Record in the sector
    public  Vector  irecs;             // Vector of SLOG_Irec

    public SLOG_FrameSector()
    {
        bytesize = 0;
        Nrec     = 0;
        irecs    = null;
    }

    public void Init()
    {
        bytesize = 0;
        Nrec     = 0;
        if ( irecs != null ) {
            irecs.removeAllElements();
            irecs = null;
        }
    }

    public void SetByteSize( int in_bytesize )
    {
        bytesize = in_bytesize;
    }

    public void SetNumOfRecs( int in_Nrec )
    {
        Nrec = in_Nrec;
    }

    public void ReadFromDataStream(       DataInputStream  data_istm,
                                    final SLOG_RecDefs     recdefs )
    throws IOException, IllegalArgumentException
    {
        int bytesize_expected = bytesize;
        int Nrec_expected     = Nrec;

        bytesize = 0;
        Nrec     = 0;
        
        irecs = new Vector( Nrec_expected );
        for ( Nrec = 0; Nrec < Nrec_expected; Nrec++ ) {
            SLOG_Irec irec = new SLOG_Irec( data_istm, recdefs );
            irecs.addElement( irec );
            bytesize += irec.fix.bytesize;
        }

        if ( bytesize != bytesize_expected )
            throw new IOException( "Inconsistency in bytes read : "
                                 + "Actual bytes read = " + bytesize + ", "
                                 + "bytes expected = " + bytesize_expected );
        if ( irecs.size() != Nrec_expected )
            throw new IOException( "Inconsistency in No. of Record read : "
                                 + "Actual No. of Records read = " 
                                 + irecs.size() + ", "
                                 + "No. of Records expected = "
                                 + Nrec_expected );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();

        Enumeration enum = irecs.elements();
        while( enum.hasMoreElements() ) {
            String str = enum.nextElement().toString();
            representation.append( str + "\n" );
        }

        return( representation.toString() );
    }
}

