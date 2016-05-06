import java.io.*;

public class SLOG_Thread implements Runnable
{
    private String           filename;
    private SLOG_InputStream slog;
    private SLOG_Frame       slog_frame;
    private long             fptr2frame;
    private int              frame_idx;

    public SLOG_Thread( String in_filename )
    {
        filename     = new String( in_filename );
        fptr2frame   = 0;
        frame_idx    = -1;
        slog         = null;
        slog_frame   = null;
    }

    public SLOG_Thread( int in_frame_idx )
    {
        SeekFrame( in_frame_idx );
    }

    public SLOG_Thread( SLOG_InputStream in_slog, int in_frame_idx )
    {
        SetIstm( in_slog );
        SeekFrame( in_frame_idx );
    }

    public synchronized int NumOfFrames()
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) { return 0; }
        return slog.dir.NumOfFrames();
    }

    public synchronized SLOG_InputStream GetSlogIstm()
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) { return null; }
        return slog;
    }

    public synchronized void SeekFrame( int in_frame_idx )
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) { 
            throw new NullPointerException( "slog == null" ); 
        }
        if ( in_frame_idx >= 0 && in_frame_idx < slog.dir.NumOfFrames() ) {
            frame_idx = in_frame_idx;
            fptr2frame = slog.dir.EntryAt( frame_idx ).fptr2framehdr;
        }
        else {
            frame_idx = -1;
            fptr2frame = -1;
        }
    }

    public synchronized void SetIstm( SLOG_InputStream in_slog )
    {
        slog = in_slog;
        notifyAll();
    }

    public synchronized void ReadIstm()
    throws IOException
    {
        slog = new SLOG_InputStream( filename );
        notifyAll();
    }
 
    public synchronized void ReadFrame()
    throws IOException
    {
        try {
            while ( slog == null ) wait();
        } catch ( InterruptedException ie ) {
            throw new NullPointerException( "slog == null" );
        }
        if ( fptr2frame > 0 ) 
            slog_frame = slog.GetFrame( fptr2frame );
        else
            throw new IOException( "fptr2frame is Invalid" );
    }

    public void ReadFrame( int in_frame_idx )
    throws IOException
    {
        SeekFrame( in_frame_idx );
        ReadFrame();
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

    public void run() 
    {
        if ( fptr2frame == 0 ) {
            try { ReadIstm(); }
            catch ( IOException err )
                { System.out.println( "ReadIstm() has IO error" ); }
            System.out.println( this.toString() );
        }
          
        
        if ( fptr2frame > 0 ) {
            try { ReadFrame( frame_idx ); }
            catch ( IOException err )
                { System.out.println( "ReadFrame() has IO error" ); }
            System.out.println( "Frame Index = " + frame_idx + "\n"
                              + this.toString() );
        }
    }
}
