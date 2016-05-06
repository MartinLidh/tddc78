import java.io.*;

public class SLOG_FrameHdr
{
    public int Nbytes_cur;
    public int Nrec_cur;
    public int Nbytes_inc;
    public int Nrec_inc;
    public int Nbytes_pas;
    public int Nrec_pas;
    public int Nbytes_out;
    public int Nrec_out;

    public SLOG_FrameHdr()
    {
        this.Init();
    }

    public SLOG_FrameHdr( DataInputStream data_istm )
    throws IOException
    {
        this.ReadFromDataStream( data_istm );
    }

    public void Init()
    {
        Nbytes_cur = 0;
        Nrec_cur   = 0;
        Nbytes_inc = 0;
        Nrec_inc   = 0;
        Nbytes_pas = 0;
        Nrec_pas   = 0;
        Nbytes_out = 0;
        Nrec_out   = 0;
    }

    public void ReadFromDataStream( DataInputStream data_istm )
    throws IOException
    {
        Nbytes_cur = data_istm.readInt();
        Nrec_cur   = data_istm.readInt();
        Nbytes_inc = data_istm.readInt();
        Nrec_inc   = data_istm.readInt();
        Nbytes_pas = data_istm.readInt();
        Nrec_pas   = data_istm.readInt();
        Nbytes_out = data_istm.readInt();
        Nrec_out   = data_istm.readInt();
    }

    public String toString()
    {
        StringBuffer rep = new StringBuffer();
        rep.append( "CUR = ( " + Nbytes_cur + ", " + Nrec_cur + " ),  " );
        rep.append( "INC = ( " + Nbytes_inc + ", " + Nrec_inc + " ),  " );
        rep.append( "PAS = ( " + Nbytes_pas + ", " + Nrec_pas + " ),  " );
        rep.append( "OUT = ( " + Nbytes_out + ", " + Nrec_out + " ) ");
        return rep.toString();
    }
}
