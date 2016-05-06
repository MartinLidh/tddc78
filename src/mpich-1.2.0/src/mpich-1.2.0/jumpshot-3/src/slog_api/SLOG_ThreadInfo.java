import java.io.*;

public class SLOG_ThreadInfo
{
    public short       node_id;
    public int         thread_id;
    public int         OSprocess_id;
    public int         OSthread_id;
    public int         app_id;

    public SLOG_ThreadInfo()
    {
        node_id       = SLOG_Const.INVALID_short;
        thread_id     = SLOG_Const.INVALID_int;
        OSprocess_id  = SLOG_Const.INVALID_int;
        OSthread_id   = SLOG_Const.INVALID_int;
        app_id        = SLOG_Const.INVALID_int;
    }

    public SLOG_ThreadInfo( short in_node_id,
                            int   in_thread_id,
                            int   in_OSprocess_id,
                            int   in_OSthread_id,
                            int   in_app_id )
    {
        node_id       = in_node_id;
        thread_id     = in_thread_id;
        OSprocess_id  = in_OSprocess_id;
        OSthread_id   = in_OSthread_id;
        app_id        = in_app_id;
    } 

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        this.node_id       = file_stm.readShort();
        this.thread_id     = file_stm.readInt();
        this.OSprocess_id  = file_stm.readInt();
        this.OSthread_id   = file_stm.readInt();
        this.app_id        = file_stm.readInt();
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        file_stm.writeShort( this.node_id );
        file_stm.writeInt( this.thread_id );
        file_stm.writeInt( this.OSprocess_id );
        file_stm.writeInt( this.OSthread_id );
        file_stm.writeInt( this.app_id );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer( "{ " );
        representation.append( node_id      + ", " );
        representation.append( thread_id    + " : " );
        representation.append( OSprocess_id + ", " );
        representation.append( OSthread_id  + ", " );
        representation.append( app_id       + " } " );
        return representation.toString();
    }
}

