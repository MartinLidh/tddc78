import java.io.*;
import java.util.*;

public class iarray extends Vector
{
    private int bytesize;

    public iarray()
    {
        super();
        bytesize = 0;
    }

    public iarray( int Nelem )
    {
        super( Nelem );
        bytesize = Nelem * 4;
    }

    public iarray( DataInputStream data_istm, int Nelem )
    throws IOException
    {
        super( Nelem );
        this.ReadFromDataStream( data_istm, Nelem );
    }

    public void ReadFromDataStream( DataInputStream  data_istm,
                                    int              Nelem )
    throws IOException
    {
        super.ensureCapacity( Nelem );
        for ( int ii = Nelem; ii > 0; ii-- )
            super.addElement( new Integer( data_istm.readInt() ) );
        bytesize = Nelem * 4;
    } 

    public void WriteToDataStream( DataOutputStream  data_ostm )
    throws IOException
    {
        Enumeration enum = super.elements();
        while( enum.hasMoreElements() )
            data_ostm.writeInt( ( (Integer) enum.nextElement() ).intValue() );
    }

    public int NbytesInFile()
    {
        return bytesize;
    }

    //  Method toString() is inherited from Vector 
}
