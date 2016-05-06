import java.io.*;

public class SLOG_RecDef extends Object implements Cloneable
{
    public  short       intvltype;
    public  SLOG_bebits bebits;
    public  short       Nassocs;
    public  short       Nargs;

    public SLOG_RecDef()
    {
        intvltype = SLOG_Const.INVALID_short;
        bebits    = null;
        Nassocs   = SLOG_Const.INVALID_short;
	Nargs     = SLOG_Const.INVALID_short;
    }

    public SLOG_RecDef( short in_intvltype, 
                        byte  in_bits_0,
                        byte  in_bits_1,
                        short in_Nassocs,
                        short in_Nargs )
    {
        intvltype = in_intvltype;
        bebits    = new SLOG_bebits( in_bits_0, in_bits_1 );
        Nassocs   = in_Nassocs;
        Nargs     = in_Nargs;
    }

    public Object clone() throws CloneNotSupportedException
    {
        SLOG_RecDef dest = (SLOG_RecDef)super.clone();
        return dest;
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        this.intvltype = file_stm.readShort();
        this.bebits    = new SLOG_bebits( file_stm );
        this.Nassocs   = file_stm.readShort();
        this.Nargs     = file_stm.readShort();
    }

    public void WriteToRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        file_stm.writeShort( this.intvltype );
        bebits.WriteToRandomFile( file_stm );
        file_stm.writeShort( this.Nassocs );
        file_stm.writeShort( this.Nargs );
    }

    public boolean non_negative() throws NegativeIntegerException
    {
        try { 
            if ( intvltype < 0 || Nassocs < 0 || Nargs < 0 )
                throw new NegativeIntegerException( this.toString()
                                                  + " has negative component" );
        }
        catch ( NegativeIntegerException err ) {
            System.out.println( "SLOG_RecDef: NegativeIntegerException "
                              + "caught" );
            System.out.println( err.toString() );
        }

        return true;
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer( "{ " );
        representation.append( intvltype + ", " );
        representation.append( bebits    + " : " );
        representation.append( Nassocs   + ", " );
        representation.append( Nargs     + " }" );
        return( representation.toString() );
    }
}
