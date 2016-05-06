import java.io.*;
import java.util.*;

public class SLOG_ProfileEntry
{
    public short   intvltype;
    public String  classtype;
    public String  label;
    public String  color;

    // public boolean is_var_irec;
    // public short   Nassocs;
    // public short   Nargs;

    public SLOG_ProfileEntry( String line )
    throws NoSuchElementException, IndexOutOfBoundsException
    {
        StringTokenizer token_strs = new StringTokenizer( line, "\t" );
        int count = token_strs.countTokens();
        if ( count == 4 ) {
            intvltype   = Short.parseShort( token_strs.nextToken().trim() );
            classtype   = token_strs.nextToken().trim();
            label       = token_strs.nextToken().trim();
            color       = token_strs.nextToken().trim();
            // is_var_irec = token_strs.nextToken().equals( "true" );
            // Nassocs     = Short.parseShort( token_strs.nextToken() );
            // Nargs       = Short.parseShort( token_strs.nextToken() );
        }
        else {
            throw new IndexOutOfBoundsException(
                      "Incorrect number of tokens = "
                      + token_strs.countTokens() );
        }
    }

    public String toString()
    {
        StringBuffer representation = new StringBuffer( "[ " );
        representation.append( intvltype   + ": " );
        representation.append( classtype   + ", " );
        representation.append( label       + ", " );
        representation.append( color       + " " );
        // representation.append( is_var_irec + " " );
        // representation.append( Nassocs     + " " );
        // representation.append( Nargs       + " " );
        representation.append( "]" );
        return representation.toString();
    }
}
