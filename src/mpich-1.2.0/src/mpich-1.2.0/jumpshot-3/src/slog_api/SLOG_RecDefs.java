import java.util.*;
import java.io.*;


public class SLOG_RecDefs
{
    private Vector defs;            //  Vector of SLOG_RecDef

    public SLOG_RecDefs()
    {
	defs = new Vector();
    }

    public SLOG_RecDefs( int Nelem )
    {
	defs = new Vector( Nelem );
    }

    public SLOG_RecDefs( RandomAccessFile file_stm )
    throws IOException
    {
        defs = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_RecDefs( RandomAccessFile file_stm, long fileloc )
    throws IOException
    {
        file_stm.seek( fileloc );
        defs = new Vector();
        this.ReadFromRandomFile( file_stm );
    }

    public SLOG_RecDef EntryAt( int idx )
    throws ArrayIndexOutOfBoundsException
    {
        try {
            return( (SLOG_RecDef) defs.elementAt( idx ) );
        }
        catch ( ArrayIndexOutOfBoundsException e ) {
            return null;
        }
    }

    public int NumOfAssocs( short in_intvltype, final SLOG_bebits in_bebits )
    throws IndexOutOfBoundsException
    {
        SLOG_RecDef the_def;

        Enumeration enum = defs.elements();
        while( enum.hasMoreElements() ) {
            the_def = (SLOG_RecDef) enum.nextElement();
            if (    the_def.intvltype == in_intvltype
                 && the_def.bebits.equal( in_bebits ) )
                return the_def.Nassocs;
        }

        throw new IndexOutOfBoundsException( in_intvltype + ", " + in_bebits );
    }

    public int NumOfArgs( short in_intvltype, final SLOG_bebits in_bebits )
    throws IndexOutOfBoundsException
    {
        SLOG_RecDef the_def;

        Enumeration enum = defs.elements();
        while( enum.hasMoreElements() ) {
            the_def = (SLOG_RecDef) enum.nextElement(); 
            if (    the_def.intvltype == in_intvltype
                 && the_def.bebits.equal( in_bebits ) )
                return the_def.Nargs;
        }

        throw new IndexOutOfBoundsException( in_intvltype + ", " + in_bebits );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        SLOG_RecDef recdef;

        int Nelem = file_stm.readInt();
        for ( int ii = 0; ii < Nelem; ii++ ) {
            recdef = new SLOG_RecDef();
            recdef.ReadFromRandomFile( file_stm );
            defs.addElement( recdef );
        }
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        Enumeration enum = defs.elements();
        while( enum.hasMoreElements() )
            ( (SLOG_RecDef) enum.nextElement() ).WriteToRandomFile( file_stm );
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer( "RecDefs[] = \n" );

        Enumeration enum = defs.elements();
        while( enum.hasMoreElements() )
            representation.append( (SLOG_RecDef) enum.nextElement() + "\n" );

        return( representation.toString() );
    }
}
