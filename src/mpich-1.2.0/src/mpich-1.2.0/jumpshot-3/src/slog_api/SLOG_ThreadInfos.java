import java.util.*;
import java.io.*;


public class SLOG_ThreadInfos
{
    private Vector threads;

    public SLOG_ThreadInfos()
    {
        threads = new Vector();
    }

    public SLOG_ThreadInfos( int Nelem )
    {
        threads = new Vector( Nelem );
    }

    public SLOG_ThreadInfos( RandomAccessFile file_stm )
    throws IOException
    {
        threads = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_ThreadInfos( RandomAccessFile file_stm, long fileloc )
    throws IOException
    {
        file_stm.seek( fileloc );
        threads = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_ThreadInfo EntryAt( int idx )
    throws ArrayIndexOutOfBoundsException
    {
        try {
            return( (SLOG_ThreadInfo) threads.elementAt( idx ) );
        }
        catch ( ArrayIndexOutOfBoundsException e ) {
            return null;
        }
    }

    public int NumOfThreads()
    {
        return threads.size();
    }

    public int GetOSprocessID( short in_node_id, int in_thread_id )
    throws IndexOutOfBoundsException
    {
        SLOG_ThreadInfo  thread;

        Enumeration enum = threads.elements();
        while( enum.hasMoreElements() ) {
            thread = (SLOG_ThreadInfo) enum.nextElement();
            if (    thread.node_id   == in_node_id
                 && thread.thread_id == in_thread_id )
                return thread.OSprocess_id;
        }

        throw new IndexOutOfBoundsException( in_node_id + ", " + in_thread_id );
    }

    public int GetOSthreadID( short in_node_id, int in_thread_id )
    throws IndexOutOfBoundsException
    {
        SLOG_ThreadInfo  thread;

        Enumeration enum = threads.elements();
        while( enum.hasMoreElements() ) {
            thread = (SLOG_ThreadInfo) enum.nextElement();
            if (    thread.node_id   == in_node_id
                 && thread.thread_id == in_thread_id )
                return thread.OSthread_id;
        }

        throw new IndexOutOfBoundsException( in_node_id + ", " + in_thread_id );
    }

    public int GetAppID( short in_node_id, int in_thread_id )
    throws IndexOutOfBoundsException
    {
        SLOG_ThreadInfo  thread;

        Enumeration enum = threads.elements();
        while( enum.hasMoreElements() ) {
            thread = (SLOG_ThreadInfo) enum.nextElement();
            if (    thread.node_id   == in_node_id
                 && thread.thread_id == in_thread_id )
                return thread.app_id;
        }

        throw new IndexOutOfBoundsException( in_node_id + ", " + in_thread_id );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        int Nelem = file_stm.readInt();
        for ( int ii = 0; ii < Nelem; ii++ ) {
            SLOG_ThreadInfo thread = new SLOG_ThreadInfo();
            thread.ReadFromRandomFile( file_stm );
            threads.addElement( thread );
        }
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        Enumeration enum = threads.elements();
        while( enum.hasMoreElements() )
            ( (SLOG_ThreadInfo) enum.nextElement() )
            .WriteToRandomFile( file_stm );
    }

    public String toString()
    {
        StringBuffer rep = new StringBuffer( "ThreadInfos[] = \n" );

        Enumeration enum = threads.elements();
        while( enum.hasMoreElements() )
            rep.append( (SLOG_ThreadInfo) enum.nextElement() + "\n" );

        return( rep.toString() );
    }
}
