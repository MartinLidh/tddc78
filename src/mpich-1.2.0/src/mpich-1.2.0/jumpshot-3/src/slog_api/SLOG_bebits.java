import java.io.*;

    class SLOG_bebits
    {
        private byte[]        bits;
       
        public SLOG_bebits()
        {
            bits      = new byte[ 2 ];
        }

        public SLOG_bebits( byte bit_0, byte bit_1 )
        {
            bits      = new byte[ 2 ];
            bits[ 0 ] = bit_0;
            bits[ 1 ] = bit_1;
        }

        public SLOG_bebits( DataInputStream data_istm )
        throws IOException
        {   
            bits      = new byte[ 2 ];
            this.ReadFromDataStream( data_istm );
        }

        public SLOG_bebits( RandomAccessFile file_stm )
        throws IOException
        {  
            bits      = new byte[ 2 ];
            this.ReadFromRandomFile( file_stm );
        }
        
        private byte conv( byte bit )
        {
            return  (byte)( bit > 00 ? 01 : 00 );
        }

        public byte encode()
        {
            return (byte)( 02 * conv( bits[0] ) + conv( bits[1] ) );
        }

        public  void decode( byte in_bebits )
        throws IllegalArgumentException
        {
            if ( in_bebits >= 0 && in_bebits <= 3 ) {
                bits[ 0 ] = (byte) ( in_bebits % 2 );
                bits[ 1 ] = (byte) ( in_bebits / 2 );
            }
            else
                throw new IllegalArgumentException( "input bebits = " 
                                                  + in_bebits );
        }

        public boolean equal( final SLOG_bebits the_bebits )
        {
            if (    bits[0] == the_bebits.bits[0]
                 && bits[1] == the_bebits.bits[1] )
                return true;
            else
                return false;
        }

        public void ReadFromDataStream( DataInputStream data_istm )
        throws IOException, IllegalArgumentException
        {   
            byte tmp = data_istm.readByte();
            this.decode( tmp );
        }  

        public void ReadFromRandomFile( RandomAccessFile file_stm )
        throws IOException, IllegalArgumentException
        {   
            byte tmp = file_stm.readByte();
            this.decode( tmp );
        }  

        public void WriteToDataStream( DataOutputStream data_ostm )
        throws IOException
        {
            byte tmp = this.encode();
            data_ostm.writeByte( tmp );
        }
    
        public void WriteToRandomFile( RandomAccessFile file_stm )
        throws IOException
        {
            byte tmp = this.encode();
            file_stm.writeByte( tmp );
        }
    
        public String toString()
        {
            StringBuffer representation = new StringBuffer( "( " );
            representation.append( bits[ 0 ] + ", " + bits[ 1 ] );
            representation.append( " )" );
            return ( representation.toString() );
        }
    }
