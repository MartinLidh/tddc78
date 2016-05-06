import java.util.Vector;



class Info
{
    double  begT;
    double  endT;
    double  lenT;
    boolean blink = false;

    public  Info() {}
}


//This class represents Rectangle( State ) Info
class StateInfo extends Info
{ 
    StateGroupLabel  groupID;        // Label used in Y axis in timelines window
    int              level = 0;      // For nested states
    StateInfo        higher = null;  // Pointer to the next higher state
                                     // enveloping this state.
    RecDef           stateDef;
  
    //Constructor 
    public StateInfo() 
    { 
        super();
    }
  
    public StateInfo( final SLOG_Irec_min     irec_min,
                      final SLOG_ThreadInfos  thread_infos,
                      final int               view_idx )
    {
        begT    = irec_min.starttime;
        lenT    = irec_min.duration;
        endT    = begT + lenT;
        blink   = false;
        groupID = new StateGroupLabel( irec_min.origID, 
                                       thread_infos, view_idx );
    }

    public void SetStateDefLink( RecDef in_statedef )
    {
        stateDef = in_statedef;
    }

    public String toString()
    {
        return ( "StateInfo=[ level=" + level + ", groupID=" + groupID + 
                 ", begT=" + begT + ", endT=" + endT + ", lenT=" + lenT +
                 ", name=" + stateDef.description + " ]" );
    }
}



//This class stores information about a message
class ArrowInfo extends Info
{
    StateGroupLabel begGroupID;
    StateGroupLabel endGroupID;

    RecDef          arrowDef;
  
    //Constructor for the object representing a message.
    //origID  = Task Id of the StateGroup where the message originates
    //destID  = Task Id of the StateGroup where the message ends.
    //begtime = The time at which the message originates.
    public ArrowInfo( StateGroupLabel origID,  StateGroupLabel destID, 
                      double          begtime, double          endtime )
    {
        super ();
        begGroupID = new StateGroupLabel( origID );
        endGroupID = new StateGroupLabel( destID );
        begT       = begtime;
        endT       = endtime;
        lenT       = endT - begT;
    }
  
    public ArrowInfo( final SLOG_Irec_min     irec_min,
                      final SLOG_ThreadInfos  thread_infos,
                      final int               view_idx,
                      final boolean           forward )
    {
        if ( forward ) {
            begT       = irec_min.starttime;
            lenT       = irec_min.duration;
            endT       = begT + lenT;
            begGroupID = new StateGroupLabel( irec_min.origID,
                                              thread_infos, view_idx );
            endGroupID = new StateGroupLabel( irec_min.destID,
                                              thread_infos, view_idx );
        }
        else {
            endT       = irec_min.starttime;
            lenT       = irec_min.duration;
            begT       = endT + lenT;
            begGroupID = new StateGroupLabel( irec_min.destID,
                                              thread_infos, view_idx );
            endGroupID = new StateGroupLabel( irec_min.origID,
                                              thread_infos, view_idx );
        }

        blink      = false;
    }

    public void SetArrowDefLink( RecDef in_arrowdef )
    {
        arrowDef = in_arrowdef;
    }

    public String toString () {
        return ("ArrowInfo=[begGroupID=" + begGroupID
                          + ", endGroupID=" + endGroupID
	                  + ", begT=" + begT + ", endT=" + endT + "]");
    }
} 



class MtnInfo
{ 
    double begT;
    double endT;
    int []s;
  
    //Constructor 
    public MtnInfo (int n) { s = new int [n]; }
  
    public String toString() 
    {
        return ( "MtnInfo=[ begT=" + begT + ", endT=" + endT
               + ", number = " + s.length + " ]");
    }
}
