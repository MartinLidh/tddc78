import java.applet.Applet;

public class jumpshot extends Applet 
{
    public void init()
    {
        checkVersion ();
        String fullfilename = null;
        String filename     = getParameter( "logfile" );
        if ( filename != null)
            fullfilename = getParameter( "logfiledir" ) + "/" + filename;
        new Mainwin( this, true, false, fullfilename, 0 );
    }

    static public void main( String [] argv )
    {
        String filename;
        int    frame_idx;

        checkVersion ();
        switch ( argv.length ) {
            case 0 :
                filename  = null;
                frame_idx = 0;
                break;
            case 1 :
                filename  = argv[ 0 ];
                frame_idx = 0;
                break;
            case 2 :
                filename  = argv[ 0 ];
                frame_idx = Integer.parseInt( argv[ 1 ] );
                break;
            default :
                System.err.println( "MAIN : "
                                  + "Number of command line arguments is "
                                  + argv.length + " which is more than what "
                                  + " we expected.\n Assume 2 and continue" );
                filename  = argv[ 0 ];
                frame_idx = Integer.parseInt( argv[ 1 ] );
        }
        // debug.SetMessageOn();
        new Mainwin ( null, false, false, filename, frame_idx );
    }
  
    static void checkVersion()
    {
        String vers = System.getProperty ("java.version");
        if ( vers.compareTo ("1.1.7") < 0 )
        {
            System.err.println ( "WARNING: Jumpshot-3 should be run with "
                               + "Java Vitual Machine 1.1.7 or higher.\n"
                               + "\t In case there is problem to run "
                               + "jumpshot-3 with older version of JVM, \n"
                               + "\t Install a newer version of JVM, then do \n"
                               + "\t\t configure --with-java=NEW_JDK_PATH\n "
                               + "\t\t make" );
            System.err.println ("Questions: mpi-maint@mcs.anl.gov");
        }
    }
}
