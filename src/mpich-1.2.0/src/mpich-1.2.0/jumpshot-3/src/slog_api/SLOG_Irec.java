import java.io.*;

public class SLOG_Irec
{
    public SLOG_Irec_min      fix;
    public SLOG_Irec_assocs   assocs;
    public SLOG_Irec_vtrargs  vtrargs;

    public SLOG_Irec()
    {
        fix      = new SLOG_Irec_min();
        assocs   = new SLOG_Irec_assocs();
        vtrargs  = new SLOG_Irec_vtrargs();
    }

    public SLOG_Irec( short   in_bytesize,
                      short   in_rectype,
                      short   in_intvltype,
                      byte    in_bits_0,
                      byte    in_bits_1,
                      double  in_starttime,
                      double  in_endtime,
                      short   in_node_id,
                      byte    in_cpu_id,
                      int     in_thread_id,
                      long    in_where )
    {
        fix      = new SLOG_Irec_min( in_bytesize, in_rectype, in_intvltype,
                                      in_bits_0, in_bits_1,
                                      in_starttime, in_endtime,
                                      in_node_id, in_cpu_id, in_thread_id,
                                      in_where );
        assocs   = new SLOG_Irec_assocs(); 
        vtrargs  = new SLOG_Irec_vtrargs();
    }

    public SLOG_Irec(       DataInputStream  data_istm,
                      final SLOG_RecDefs     recdefs )
    throws IOException, IllegalArgumentException
    {
        int Nbytes_read = 0;

        fix      = new SLOG_Irec_min( data_istm );
        Nbytes_read += fix.NbytesInFile();
        assocs   = new SLOG_Irec_assocs( data_istm,
                                         recdefs.NumOfAssocs( fix.intvltype,
                                                              fix.bebits ) );
        Nbytes_read += assocs.NbytesInFile();
        vtrargs  = new SLOG_Irec_vtrargs( data_istm, recdefs,
                                          fix, Nbytes_read );
        
        
    }

    public void ReadFromDataStream(       DataInputStream  data_istm,
                                    final SLOG_RecDefs     recdefs )
    throws IOException, IllegalArgumentException
    {
        int Nbytes_read = 0;

        fix.ReadFromDataStream( data_istm );
        Nbytes_read += fix.NbytesInFile();
        assocs.ReadFromDataStream( data_istm,
                                   recdefs.NumOfAssocs( fix.intvltype,
                                                        fix.bebits ) );
        Nbytes_read += assocs.NbytesInFile();
        vtrargs.ReadFromDataStream( data_istm, recdefs, fix, Nbytes_read );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();
        representation.append(       fix.toString() );
        representation.append( " " + assocs.toString() );
        representation.append( " " + vtrargs.toString() );
        return( representation.toString() );
    }
}
