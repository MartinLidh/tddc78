import java.io.*;

public class ts_bebits
{
    public static void main( String argv[] ) throws IOException
    {
        System.out.println( "Encoding" );
        for ( byte idx_0 = 0; idx_0 < 3; idx_0++ ) {
            for ( byte idx_1 = 0; idx_1 < 2; idx_1++ ) {
                SLOG_bebits bebits = new SLOG_bebits( idx_0, idx_1 );
                System.out.println( bebits + " = " + bebits.encode() );
            }
        }

        
        System.out.println( "Decoding" );
        for ( byte idx = 0; idx < 8; idx++ ) {
             SLOG_bebits bebits = new SLOG_bebits();
             bebits.decode( idx );
             System.out.println( idx + " = " + bebits );
        }
    }
}
