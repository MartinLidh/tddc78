import java.io.*;
import java.util.*;

public class SLOG_Statistics
{
    double        starttime;
    double        endtime;
    int           Nset;
    int           Nbin;
    Vector        sets;       // Vector of SLOG_StatSet

    public SLOG_Statistics()
    {
        starttime  = SLOG_Const.INVALID_double;
        endtime    = SLOG_Const.INVALID_double;
        Nset       = SLOG_Const.INVALID_int;
        Nbin       = SLOG_Const.INVALID_int;
        sets       = null;
    }

    public SLOG_Statistics( RandomAccessFile file_stm )
    throws IOException, NegativeArraySizeException
    {
        Nset       = file_stm.readInt();
        Nbin       = file_stm.readInt();
        if ( Nset <= 0 || Nbin <= 0 )
            throw new NegativeArraySizeException( "Nset = " + Nset + ",  "
                                                + "Nbin = " + Nbin );
        sets       = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_Statistics( RandomAccessFile file_stm, long fileloc )
    throws IOException, NegativeArraySizeException
    {
        file_stm.seek( fileloc );

        Nset       = file_stm.readInt();
        Nbin       = file_stm.readInt();
        if ( Nset <= 0 || Nbin <= 0 )
            throw new NegativeArraySizeException( "Nset = " + Nset + ",  "
                                                + "Nbin = " + Nbin );
        sets       = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        SLOG_StatSet    statset;

        int expected_bsize = 8 * 2
                           + SLOG_StatSet.ByteSizeInFile( Nbin ) * Nset;

        byte buffer[] = new byte[ expected_bsize ];
        file_stm.readFully( buffer );

        ByteArrayInputStream ary_stream = new ByteArrayInputStream( buffer );
        DataInputStream data_istm = new DataInputStream( ary_stream );

        starttime = data_istm.readDouble();
        endtime   = data_istm.readDouble();
        for ( int ii = 0; ii < Nset; ii++ ) {
            statset = new SLOG_StatSet( data_istm );
            sets.addElement( statset );
        }
    }

    public String toString()
    {
        StringBuffer rep = new StringBuffer( "Statistics Preview[] = \n"
                                           + "\t" + "Collect At : " );

        rep.append( "Starttime = " + starttime + ", " );
        rep.append( "Endtime = "   + endtime   + "\n\n" );

        int idx = 0;
        Enumeration enum = sets.elements();
        while( enum.hasMoreElements() ) {
            rep.append( "\t" + "Preview Statistics Set " + idx + "\n" );
            rep.append( ( SLOG_StatSet ) enum.nextElement() + "\n" );
            idx++;
        }

        return( rep.toString() );
    }
}
