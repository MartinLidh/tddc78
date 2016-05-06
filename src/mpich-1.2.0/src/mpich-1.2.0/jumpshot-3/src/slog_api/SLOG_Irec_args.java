import java.io.*;

public class SLOG_Irec_args
{
    public iarray  elems;

    public SLOG_Irec_args()
    {
        elems = new iarray();
    }

    public SLOG_Irec_args( int in_Nelem )
    {
        elems = new iarray( in_Nelem );
    }

    public SLOG_Irec_args( DataInputStream data_istm,
                           int              Nargs )
    throws IOException
    {
        elems = new iarray( data_istm, Nargs );
    }

    public void ReadFromDataStream( DataInputStream  data_istm,
                                    int              Nargs )
    throws IOException
    {
        elems.ReadFromDataStream( data_istm, Nargs );
    } 

    public void WriteToDataStream( DataOutputStream  data_ostm )
    throws IOException
    {
        elems.WriteToDataStream( data_ostm );
    }

    public int NbytesInFile()
    {
        return( elems.NbytesInFile() );
    }

    public String toString()
    {
        return( elems.toString() );
    }
}
