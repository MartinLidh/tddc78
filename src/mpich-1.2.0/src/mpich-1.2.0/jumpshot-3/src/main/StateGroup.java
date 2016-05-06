import java.util.*;

/*
   Collection of StateIntervals i.e. states belonging to the timeline
   in ascending order of their end timestamps
*/
public class StateGroup extends Vector
{
            StateGroupLabel   groupID;        // groupID of this StateGroup
            boolean           dispStatus;     // display status
    private double            begT;           // start time for the first state
            int               y;              // y-coordinate of this 
                                              // StateGroup's line in Canvas
  
    public StateGroup( int major_ID, int minor_ID )
    { 
        super();
        groupID    = new StateGroupLabel( major_ID, minor_ID );
        dispStatus = true;
        begT       = 0.0;
        y          = 0;
    }

    public StateGroup( StateGroupLabel label )
    { 
        super();
        groupID    = new StateGroupLabel( label );
        dispStatus = true;
        begT       = 0.0;
        y          = 0;
    }
    

    public void SetBeginTime()
    {
        if ( this.size() > 0 ) {
            StateInterval state = (StateInterval) this.firstElement();
            begT                = state.info.begT;
        }
    }

    public void SetBeginTime( double in_begtime )
    {
        if ( this.size() > 0 ) {
            StateInterval state = (StateInterval) this.firstElement();
            begT                = state.info.begT;
        }
        else
            begT = in_begtime;
    }

    /**
     * push the timeLine and all states of the given state group.
     * time = this specifies the time in seconds the time line is to be moved
     * (time > 0) => right, (time < 0) => left
     */
    public void PushTimeLine( double time )
    {
        debug.println( "StateGroup.PushTimeLine( " + time + " )" );
        if ( this.size() > 0 ) {
            Enumeration   states = super.elements();
            StateInterval state  = (StateInterval) super.firstElement();
            if ( state != null ) {
                state.info.begT += time;
                //  Should the if statement be deleted ?!!!, 7/22/99
                /*
                if ( state.info.begT < 0 ) {
                    time -= state.info.begT;
                    state.info.begT = 0;
                }
                */
                state.info.endT += time;
                states.nextElement();
            }

            while ( states.hasMoreElements() ) {
                state = (StateInterval) states.nextElement();
                state.info.begT += time;
                state.info.endT += time;
            }
        }

        
        //  The following 2 calls will leave for caller to calls
        //Push the arrows
        //   pushArrows (grp, time);
        //position the image appropriately
        //   adjustPosition ();
    }

    /**
     * push the time line and all states of the given states
     * p = process whose time line is to be moved
     * amount = displacement in pixels to move
     * dir = direction. true => right, false => left
     */
    public void PushTimeLine(double time, boolean dir)
    {
        if ( !dir ) time *= (- 1);
        PushTimeLine( time );
    }

    public void ResetTimeLine()
    {
        debug.println( "StateGroup.ResetTimeLine : " );
        if ( super.size() > 0 ) {
            StateInterval state = (StateInterval) super.firstElement();
            debug.println( "\t" + "StateGroup.PushTimeLine( "
                              + (this.begT - state.info.begT) + " )" );
            PushTimeLine( this.begT - state.info.begT );
        }
    }
}
