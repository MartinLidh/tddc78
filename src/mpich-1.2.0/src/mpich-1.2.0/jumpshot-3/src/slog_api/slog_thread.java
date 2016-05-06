import java.io.*;

public class slog_thread
{
    private String filename;
    private SLOG_InputStream slog;
    private SLOG_Frame       slog_frame;
    private long             fptr2frame;
    private SwingWorker      readworker;
    private boolean          reader_alive;

    public slog_thread( String in_filename )
    throws IOException
    {
        filename     = new String( in_filename );
        fptr2frame   = 0;
        slog         = null;
        slog_frame   = null;
        readworker   = null;
        reader_alive = false;
        // ReadLogFile();
    }

    public synchronized int NumOfFrames()
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) { return 0; }
        return slog.dir.NumOfFrames();
    }

    public synchronized void SeekFrame( int frame_idx )
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) { 
            throw new NullPointerException( "slog == null" ); 
        }
        if ( frame_idx >= 0 && frame_idx < slog.dir.NumOfFrames() )
            fptr2frame = slog.dir.EntryAt( frame_idx ).fptr2framehdr;
        else
            fptr2frame = -1;
    }
 
    public void ReadLogFile()
    {
        freeMem();

        readworker = new SwingWorker() 
        {
            public synchronized Object construct()
            {
                System.out.println( "Start construct()" );
                reader_alive = true;

                if ( fptr2frame == 0 ) {
                    try {
                        SLOG_InputStream tslog = new SLOG_InputStream(filename);
                        System.out.println( this.getClass().getName()
                                          + ": Initialize SLOG" );
                        // System.out.println( tslog );
                        return tslog;
                    } catch ( IOException ioerr ) {
                        System.out.println( "SLOG_InputStream() fails" );
                    }
                }

                if ( fptr2frame > 0 ) {
                    
                    try {
                        while ( slog == null ) wait();
                    } catch ( InterruptedException ie ) { 
                        throw new NullPointerException( "slog == null" ); 
                    }

                    try {
                           SLOG_Frame tslog_frame = slog.GetFrame( fptr2frame );
                           System.out.println( this.getClass().getName()
                                             + ": Retrieve Frame from SLOG" );
                           // System.out.println(  tslog_frame );
                           return tslog_frame;
                    } catch ( IOException ioerr ) {
                       System.out.println( "slog.GetFrame() fails" );
                    }
                }

                    System.out.println( this.getClass().getName()
                                  + ": invalid file pointer" );
                    return null;
            }

            public synchronized void finished()
            {
                System.out.println( "Start finished()" );
                if ( fptr2frame == 0 ) {
                    slog = ( SLOG_InputStream ) get();
                    notifyAll();
                }
                if ( fptr2frame > 0 )
                    slog_frame = ( SLOG_Frame ) get();
                reader_alive = false;
            }
        };
    }

    public void ReadLogFile( int frame_idx )
    {
        SeekFrame( frame_idx );
        ReadLogFile();
    }

    private synchronized void freeMem()
    {
        if ( reader_alive ) {
            readworker.interrupt(); 
            readworker = null;
            //    System.runFinalization (); System.gc ();
        }

        if ( fptr2frame > 0 )
            slog_frame = null;
    }

    public synchronized String toString()
    {
        if ( fptr2frame == 0 ) {
            try {
                while ( slog == null ) wait();
            } catch ( InterruptedException ie ) { 
                throw new NullPointerException( "slog == null" ); 
            }
            return slog.toString();
        }

        if ( fptr2frame > 0 ) {
            if ( slog_frame != null )
                return slog_frame.toString();
            else
               throw new NullPointerException( "slog_frame == null" );
        }

        return null;
    }
}
