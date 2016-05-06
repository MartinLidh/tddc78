import java.io.*;

public class SLOG_Frame
{
    public SLOG_FrameHdr      hdr;     // Header of the frame
    public SLOG_FrameSector   cur;     // Current  Interval records
    public SLOG_FrameSector   inc;     // Incoming pseudo-records
    public SLOG_FrameSector   pas;     // Passing Throgh pseudo-records
    public SLOG_FrameSector   out;     // Outgoing pseudo-records

    public SLOG_Frame()
    {
        hdr = new SLOG_FrameHdr();
        cur = new SLOG_FrameSector();
        inc = new SLOG_FrameSector();
        pas = new SLOG_FrameSector();
        out = new SLOG_FrameSector();
    }

    public void Init()
    {
        hdr.Init();
        cur.Init();
        inc.Init();
        pas.Init();
        out.Init();
    }

    public void ReadFromDataStream(       DataInputStream  data_istm,
                                    final SLOG_RecDefs     recdefs ) 
    throws IOException, IllegalArgumentException
    {
        hdr.ReadFromDataStream( data_istm );

        cur.SetByteSize( hdr.Nbytes_cur );
        cur.SetNumOfRecs( hdr.Nrec_cur );
        cur.ReadFromDataStream( data_istm, recdefs );

        inc.SetByteSize( hdr.Nbytes_inc );
        inc.SetNumOfRecs( hdr.Nrec_inc );
        inc.ReadFromDataStream( data_istm, recdefs );

        pas.SetByteSize( hdr.Nbytes_pas );
        pas.SetNumOfRecs( hdr.Nrec_pas );
        pas.ReadFromDataStream( data_istm, recdefs );

        out.SetByteSize( hdr.Nbytes_out );
        out.SetNumOfRecs( hdr.Nrec_out );
        out.ReadFromDataStream( data_istm, recdefs );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();

        representation.append( "**** Frame Header : \n" );
        representation.append( hdr.toString() + "\n" );
        representation.append( "---- Incoming Records : \n" );
        representation.append( inc.toString() );
        representation.append( "---- Passing Through Records : \n" );
        representation.append( pas.toString() );
        representation.append( "---- Current Records : \n" );
        representation.append( cur.toString() );
        representation.append( "---- Outgoing Records : \n" );
        representation.append( out.toString() );

        return representation.toString();
    }
}
