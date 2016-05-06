import java.util.Vector;
import java.util.Random;
import java.util.Enumeration;
import java.awt.Color;
import java.io.*;

/**
 * This class is responsible for building data structures from records
 *  of the slog file. Each record read by SlogReader is passed here and
 * PlotData sifts out the relevent data from it.
 * 
 * Once the slog file is read, 4 data structures exist.
 * 
 * 1. (stateDefs) This is a vector of all the state definitions in the 
 *    order they were read. 
 * 2. Each of the state definition objects mentioned above contains a vector
 *    'stateVector' that is a collection of states of the same type
 *    in the ascending order of their end timestamps.
 * 3. (data) This is vector or all states of all types in ascending order 
 *    of their end timestamps.
 * 4. (a) This is an object that contains a vector 'arrows' which is a 
 *    list of ArrowInfo objects representing messages.
 */

public class PlotData
{
            SLOG_InputStream    slog                 = null;
            SLOG_Frame          slog_frame           = null;

            Vector              stateDefs;
            StateGroupListPair  all_states;
            Vector              arrowDefs;
            ArrowList           all_arrows;   /* Not used in drawing canvas */
            Vector              mtns;

            String              view_indexes_label;



    /**
     * Constructor
     */
    public PlotData( final SLOG_InputStream in_slog, 
                     final SLOG_Frame       in_frame,
                           int              connect_idx,
                           int              view_idx )
    {
        slog              = in_slog;
        slog_frame        = in_frame;

        view_indexes_label = StateGroupLabel.GetIndexesLabel( view_idx );

        stateDefs  = new Vector();
        all_states = new StateGroupListPair();
        arrowDefs  = new Vector();
        all_arrows = new ArrowList();
        mtns       = new Vector();

        all_states.visible.Init( slog.threadinfos, connect_idx, view_idx );
        CreateStateDefsAndArrowDefs();
        CreateStatesAndArrows( connect_idx, view_idx );
        all_states.visible.SetBeginTime();
    }

    /**
     *  Scan SLOG_InputStream's profile to create the StateDefs[]
     */
    private void CreateStateDefsAndArrowDefs()
    {
        SLOG_ProfileEntry entry;
        String            entry_classtype;
        String            entry_description;
        RecDef            statedef, arrowdef;

        Enumeration profile = slog.profile.entries.elements();
        while ( profile.hasMoreElements() ) {
            entry    = ( SLOG_ProfileEntry ) profile.nextElement();
            entry_classtype = ( new String( entry.classtype ) ).toLowerCase();
            if ( entry_classtype.indexOf( "message" ) >= 0 ) {
                arrowdef = new RecDef( entry );
                arrowDefs.addElement( arrowdef );
                entry_description = ( new String( entry.label ) ).toLowerCase();
                if ( entry_description.indexOf( "forward" ) >= 0 )
                    all_arrows.SetIntvltypeForForwardArrow( entry.intvltype );
                if ( entry_description.indexOf( "backward" ) >= 0 )
                    all_arrows.SetIntvltypeForBackwardArrow( entry.intvltype );
            }
            else {
                statedef = new RecDef( entry );
                stateDefs.addElement( statedef );
            }
        }
    }

    private void SplitSectorIntoStatesAndArrows( SLOG_FrameSector sector,
                                                 int              connect_idx,
                                                 int              view_idx )
    {
        SLOG_Irec           irec;
        RecDef              statedef;
        RecDef              arrowdef;
        Enumeration         irecs;
        boolean             isforward;

        SLOG_ThreadInfos    slog_threadinfos = slog.threadinfos;

        irecs = sector.irecs.elements();
        while ( irecs.hasMoreElements() ) {
            irec = ( SLOG_Irec ) irecs.nextElement();
            if ( irec.fix.rectype == CONST.RectypeForMessage ) {
                isforward = all_arrows.IsForwardArrow( irec.fix.intvltype );
                ArrowInfo arrow = new ArrowInfo( irec.fix,
                                                 slog_threadinfos,
                                                 view_idx,
                                                 isforward );

                arrowdef = GetLinkToArrowDefs( irec.fix.intvltype );
                arrow.SetArrowDefLink( arrowdef );

                arrowdef.stateVector.addElement( arrow );    // For histogram;
                all_arrows.addElement( arrow );
                debug.println( arrow + ",  " + isforward );
            }
            else {
                StateInfo state = new StateInfo( irec.fix,
                                                 slog_threadinfos,
                                                 view_idx );

                statedef = GetLinkToStateDefs( irec.fix.intvltype );
                state.SetStateDefLink( statedef );

                statedef.stateVector.addElement( state );    // For histogram
                all_states.visible.AddStateInfo( state );
            }
        }
    }

    private void CreateStatesAndArrows( int connect_idx, int view_idx )
    {
        //   For Current Sector
        SplitSectorIntoStatesAndArrows( slog_frame.cur,
                                        connect_idx, view_idx );

        //   For Incoming Sector
        SplitSectorIntoStatesAndArrows( slog_frame.inc,
                                        connect_idx, view_idx );

        //   For Passing Through Sector
        SplitSectorIntoStatesAndArrows( slog_frame.pas,
                                        connect_idx, view_idx );

        //   For Outgoing Sector
        SplitSectorIntoStatesAndArrows( slog_frame.out,
                                        connect_idx, view_idx );
    }

    /**
     * This function traverses through the vector containing state definitions
     * looking for the state definition whose starting or ending 
     * etype corresponds with the given etype. If no such state def. is found
     * a null is returned.
     */
    private RecDef GetLinkToStateDefs( int in_intvltype ) 
    {
        RecDef statedef;

        Enumeration enum = stateDefs.elements();
        while ( enum.hasMoreElements () ) {
            statedef = ( RecDef ) enum.nextElement();
            if ( in_intvltype == statedef.intvltype ) 
                return statedef;
        }
        System.err.println ( "Error: Unrecognized state interval type = "
                           + in_intvltype );
        return null;
    }

    private RecDef GetLinkToArrowDefs( int in_intvltype )
    {
        RecDef arrowdef;

        Enumeration enum = arrowDefs.elements();
        while ( enum.hasMoreElements () ) {
            arrowdef = ( RecDef ) enum.nextElement();
            if ( in_intvltype == arrowdef.intvltype )
                return arrowdef;
        }
        System.err.println ( "Error: Unrecognized arrow interval type = "
                           + in_intvltype );
        return null;
    }

    protected void finalize() throws Throwable
    { 
        stateDefs   = null;
        all_states  = null;
        arrowDefs   = null;
        all_arrows  = null;
        mtns        = null;

        super.finalize();
    }
}
