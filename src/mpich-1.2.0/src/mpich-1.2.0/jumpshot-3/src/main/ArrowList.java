import java.io.*;
import java.util.*;

public class ArrowList extends Vector
{
    private final short invalid_itype             = -1;
    private       short itype4_forward_arrow;
    private       short itype4_backward_arrow;

    public ArrowList()
    {
        super();
        itype4_forward_arrow   = invalid_itype;
        itype4_backward_arrow  = invalid_itype;
    }

    public void SetIntvltypeForForwardArrow( short in_intvltype )
    {
        itype4_forward_arrow   = in_intvltype;
    }

    public void SetIntvltypeForBackwardArrow( short in_intvltype )
    {
        itype4_backward_arrow  = in_intvltype;
    }

    public boolean SetIntvltypeForDiffArrows( final SLOG_Profile slog_profile )
    {
        SLOG_ProfileEntry entry;
        String            entry_classtype;
        String            entry_description;

        Enumeration profile = slog_profile.entries.elements();
        while (    profile.hasMoreElements()
                && (    itype4_forward_arrow  == invalid_itype
                     || itype4_backward_arrow == invalid_itype  ) ) {
            entry = ( SLOG_ProfileEntry ) profile.nextElement();
            entry_classtype   = ( new String( entry.classtype ) ).toLowerCase();
            if ( entry_classtype.indexOf( "message" ) >= 0 ) {
                entry_description = ( new String( entry.label ) ).toLowerCase();
                if ( entry_description.indexOf( "forward" ) >= 0 )
                    itype4_forward_arrow = entry.intvltype;
                if ( entry_description.indexOf( "backward" ) >= 0 )
                    itype4_backward_arrow = entry.intvltype;
            }
        }

        debug.println("itype4_forward_arrow = " + itype4_forward_arrow);
        debug.println("itype4_backward_arrow = " + itype4_backward_arrow);

        if (    itype4_forward_arrow  != invalid_itype
             || itype4_backward_arrow != invalid_itype  )
            return true;
        else
            return false;
    }

    public boolean IsForwardArrow( short in_intvltype )
    {
        /*
        return ( in_intvltype == itype4_forward_arrow ? true : false );
        */
        if ( in_intvltype == itype4_forward_arrow )
            return true;
        else
            return false;
    }

    public boolean IsBackwardArrow( short in_intvltype )
    {
        return ( in_intvltype == itype4_backward_arrow ? true : false );
    }
}
