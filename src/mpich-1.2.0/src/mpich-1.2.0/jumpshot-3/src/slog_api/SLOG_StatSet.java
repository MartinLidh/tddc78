import java.io.*;

public class SLOG_StatSet
{
    public short    rectype;
    public short    intvltype;
    public String   label;
    public int      Nbin;
    public double   bins[];

    private int     SLOG_STRING_LEN = 21;

    public SLOG_StatSet()
    {
        rectype   = SLOG_Const.INVALID_short;
        intvltype = SLOG_Const.INVALID_short;
        label     = null;
        Nbin      = 0;
        bins      = null;
    }

    public SLOG_StatSet( DataInputStream data_istm )
    throws IOException
    {
        this.ReadFromDataStream( data_istm );
    }

    public void ReadFromDataStream( DataInputStream data_istm )
    throws IOException
    {
        BufferedReader cstm;
        short          label_size;
        byte           buffer[];

        rectype     = data_istm.readShort();
        intvltype   = data_istm.readShort();
        label_size  = data_istm.readShort();
        buffer      = new byte[ label_size ];
        data_istm.readFully( buffer );
        label       = ( new String( buffer ) ).trim();
        Nbin        = data_istm.readInt();
        bins        = new double[ Nbin ];
        for ( int ii = 0; ii < Nbin; ii++ )
            bins[ ii ] = data_istm.readDouble();
    }

    public static int ByteSizeInFile( int in_Nbin )
    {
        return ( 2 + 2 + 2 + 21 + 4 + in_Nbin * 8 );
    }

    public String toString()
    {
        final int count_per_line = 5;

        StringBuffer rep = new StringBuffer( "\t" );

        rep.append( "rectype = "   + rectype   + ",  " );
        rep.append( "intvltype = " + intvltype + ",  " );
        rep.append( "label = "     + label     + "\n"  );
        for ( int ii = 0; ii < Nbin; ii++ ) {
            rep.append( bins[ ii ] + ", " );
            if ( ii % count_per_line == count_per_line - 1 )
                rep.append( "\n" );
        }
        return rep.toString();
    }
}
