import java.io.*;

public class SLOG_tasklabel
{
    public short       node;
    public byte        cpu;
    public int         thread;

    public SLOG_tasklabel()
    {
        node    = SLOG_Const.INVALID_short;
        cpu     = SLOG_Const.INVALID_byte;
        thread  = SLOG_Const.INVALID_int;
    }

    public SLOG_tasklabel( short   in_node_id,
                           byte    in_cpu_id,
                           int     in_thread_id )
    {
        node   = in_node_id;
        cpu    = in_cpu_id;
        thread = in_thread_id;
    }

    public SLOG_tasklabel( DataInputStream data_istm )
    throws IOException
    {
        this.ReadFromDataStream( data_istm );
    }

    public void ReadFromDataStream( DataInputStream data_istm )
    throws IOException
    {
        node       = data_istm.readShort();
        cpu        = data_istm.readByte();
        thread     = data_istm.readInt();
    }

    public final int NbytesInFile()
    {
//         return( 2 + 1 + 4 );
        return( 7 );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer();
        representation.append( "( " + node + " " + cpu + " " + thread + " )" );
        return( representation.toString() );
    }
}
