import java.io.*;
import java.util.*;

public class SLOG_Irec_vtrargs
{
    public Vector   vtr;

    public SLOG_Irec_vtrargs()
    {
        vtr     = new Vector();
    }

    public SLOG_Irec_vtrargs( int in_Nvtr )
    {
        vtr     = new Vector( in_Nvtr );
    }

    public SLOG_Irec_vtrargs(       DataInputStream data_istm,
                              final SLOG_RecDefs    recdefs,
                              final SLOG_Irec_min   irec_min,
                                    int             in_Nbytes_read )
    throws IOException, IllegalArgumentException
    {
        vtr     = new Vector();
        this.ReadFromDataStream( data_istm, recdefs,
                                 irec_min, in_Nbytes_read );
    }

    public void ReadFromDataStream(       DataInputStream  data_istm,
                                    final SLOG_RecDefs     recdefs,
                                    final SLOG_Irec_min    irec_min,
                                          int              in_Nbytes_read )
    throws IOException, IllegalArgumentException
    {
        int count;
        int Nbytes_read = in_Nbytes_read;
        int Nargs;

        if ( SLOG_global.IsVarRec( irec_min.rectype ) ) {

            if ( Nbytes_read > irec_min.bytesize )
                throw new IllegalArgumentException( "in_Nbytes_read ( "
                                                  + in_Nbytes_read + " ) > "
                                                  + "irec_min.bytesize ( "
                                                  + irec_min.bytesize 
                                                  + " ) " );

            for ( count = 0; Nbytes_read < irec_min.bytesize; count++ ) {
                Nargs = data_istm.readShort();
                Nbytes_read += 2;
                SLOG_Irec_args args  = new SLOG_Irec_args( data_istm, Nargs );
                vtr.addElement( args );
                Nbytes_read += args.NbytesInFile();
            }
        }    //  end of if SLOG_global.IsVarRec( irec_min.rectype ) = true
        else {   //  if SLOG_global.IsVarRec( irec_min.rectype ) = false 
            Nargs = recdefs.NumOfArgs( irec_min.intvltype, irec_min.bebits );
            SLOG_Irec_args args  = new SLOG_Irec_args( data_istm, Nargs );
            vtr.addElement( args );
            Nbytes_read += args.NbytesInFile();
        }    //  end of if SLOG_global.IsVarRec( irec_min.rectype ) = false

        if ( Nbytes_read != irec_min.bytesize )
            throw new IOException( "Inconsistency detected : "
                                 + "Nbytes_read ( " + Nbytes_read + " ) != "
                                 + "Irec.bytesize ( " + irec_min.bytesize
                                 + " ) " );
    } 

    public void WriteToDataStream( DataOutputStream  data_ostm )
    throws IOException
    {
    }

    public String toString()
    {
        return( vtr.toString() );
    }
}
