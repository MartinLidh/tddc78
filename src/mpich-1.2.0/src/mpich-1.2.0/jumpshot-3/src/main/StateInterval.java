/**
 * This class specifies each state
 */
public class StateInterval
{
    StateGroup parent;    // StateGroup that this state belongs to
    StateInfo  info;      // object storing all relevent info. on a state
    int y;                // y coordinate of this state instance's rectangle
    int h;                // height of this state instance's rectangle

    public StateInterval( StateInfo in_info, StateGroup p )
    { 
        info   = in_info;
        parent = p;
    }
}
