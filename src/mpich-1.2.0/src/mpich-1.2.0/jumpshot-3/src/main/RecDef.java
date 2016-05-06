import java.util.Vector;
import java.awt.Color;
import javax.swing.*;

//This class represents a Record definition description
class RecDef
{
    int       intvltype;
    String    classtype;
    String    description;  // string describing state
    Color     color;        // Color given to this state

    Vector    stateVector;  // Vector of completed states (paired up events)

    // This checkbox determines whether states beloging to this state def.,
    // should be displayed or not.
    JCheckBox checkbox;

    public RecDef()
    {
        description = new String();
        color       = null;

        // stateVector.size() determines if JCheckbox = true 
        // in RecDefButtons.setupPanels()
        stateVector = new Vector( 0 );
    }

    public RecDef( SLOG_ProfileEntry entry )
    {
        intvltype   = entry.intvltype;
        classtype   = new String( entry.classtype );
        description = new String( entry.label );
        color       = ColorUtil.getColor( entry.color );

        // stateVector.size() determines if JCheckbox = true 
        // in RecDefButtons.setupPanels()
        stateVector = new Vector( 0 );
    }

    public String toString()
    {
        return ("RecDef=[ intvltype=" + intvltype
                       + ", classtype=" + classtype
                       + ", color=" + color.toString ()
                       + ", description=" + description
                       + " ]");
    }
}
