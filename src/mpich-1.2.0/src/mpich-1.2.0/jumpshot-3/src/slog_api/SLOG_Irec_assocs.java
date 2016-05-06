import java.io.*;

public class SLOG_Irec_assocs
{
    public iarray  elems;

    public SLOG_Irec_assocs()
    {
        elems = new iarray();
    }

    public SLOG_Irec_assocs( int in_Nelem )
    {
        elems = new iarray( in_Nelem );
    }

    public SLOG_Irec_assocs( DataInputStream data_istm,
                             int              Nassocs )
    throws IOException
    {
        elems = new iarray( data_istm, Nassocs );
    }

    public void ReadFromDataStream( DataInputStream  data_istm,
                                    int              Nassocs )
    throws IOException
    {
        elems.ReadFromDataStream( data_istm, Nassocs );
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
