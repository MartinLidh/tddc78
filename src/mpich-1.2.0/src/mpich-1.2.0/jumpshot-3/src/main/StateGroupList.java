import java.io.*;
import java.util.Vector;
import java.util.Enumeration;

public class StateGroupList extends Vector
{
    // private int Ngroups;
    private int max_nesting_level = 0;

    public StateGroupList()
    { 
        super();
        // Ngroups = 0;
    }

    /*
        Create the StateGroupList based on the Thread Table
        Set up time lines based on the number of entries in Thread Table
        i.e. set up the linked lists' heads
    */
    public void Init( final SLOG_ThreadInfos threadinfos,
                      final int              connect_idx,
                      final int              view_idx )
    {
        SLOG_ThreadInfo  thread;
        StateGroup       cur_group;
        StateGroupLabel  groupID;
        boolean          HasGroupBeenAdded;
        int              th_idx, seq_idx;

        groupID = new StateGroupLabel();

        int Nthreads = threadinfos.NumOfThreads();
        for ( th_idx = 0; th_idx < Nthreads; th_idx++ ) {

            thread = threadinfos.EntryAt( th_idx );
            groupID.SetIndexes( thread, view_idx );

            HasGroupBeenAdded = false;
            seq_idx           = -1;
            cur_group         = null;
            Enumeration groups = super.elements();
            while ( groups.hasMoreElements() ) {
                seq_idx++;
                cur_group = (StateGroup) groups.nextElement();
                if ( cur_group.groupID.IsGreaterThan( groupID ) ) {
                    StateGroup group = new StateGroup( groupID );
                    super.insertElementAt( group, seq_idx );
                    // Ngroups ++;
                    HasGroupBeenAdded = true;
                    break;  // exit the while loop
                }
                else if ( cur_group.groupID.IsEqualTo( groupID ) ) { 
                    HasGroupBeenAdded = true;
                    break;  // exit the while loop
                } 
                else  // if ( cur_group.groupID.IsLessThan( groupID ) )
                    {  /*  Continue  */  }
            }   //  Endof  while ( groups.hasMoreElements() )
            if ( ! HasGroupBeenAdded ) {
                StateGroup group = new StateGroup( groupID );
                super.addElement( group );
                HasGroupBeenAdded = true;
            }

        }   //  for ( th_idx = 0; th_idx < Nthreads; th_idx++ )
    }

    /**
     *   Add the input StateInfo to a StateGroupList whose elements,
     *   StateGroups, are assumed to have their groupIDs arranged in the
     *   accending order of their values
     */
    public void AddStateInfo( StateInfo in_stateinfo )
    {
        StateGroup       cur_group         = null;
        boolean          HasStateBeenAdded = false;
        int              idx               = -1;
        StateGroupLabel  in_groupID        = in_stateinfo.groupID;

        Enumeration groups = super.elements();
        while ( groups.hasMoreElements() ) {
            idx ++;
            cur_group = (StateGroup) groups.nextElement();
            //  Assume the groups is sorted in accending order of groupID
            if ( cur_group.groupID.IsGreaterThan( in_groupID ) ) {
                // Create the corresponding StateGroup with the given groupID
                StateGroup group = new StateGroup( in_groupID );
                super.insertElementAt( group, idx );   //  <--- Insert Elem --
                // Ngroups ++;

                // Add the in_stateinfo to the appropriate StateGroup
                StateInterval state = new StateInterval( in_stateinfo,
                                                         group );
                group.addElement( state );
                HasStateBeenAdded = true;
                break;
            }
            else if ( cur_group.groupID.IsEqualTo( in_groupID ) ) {
                // Add the in_stateinfo to the appropriate StateGroup
                StateInterval state = new StateInterval( in_stateinfo,
                                                         cur_group );
                cur_group.addElement( state );
                HasStateBeenAdded = true;
                break;
            }
            else  // if ( cur_group.groupID.IsLessThan( in_groupID ) )
                {  /*  Continue  */  }
        }   //  Endof  while ( enum.hasMoreElements() )
         
        if ( ! HasStateBeenAdded ) {
            // Create the corresponding StateGroup with the given groupID
            StateGroup group = new StateGroup( in_groupID );
            super.addElement( group );                 //  <--- Add Elem -----
            // Ngroups ++;

            // Add the in_stateinfo to the appropriate StateGroup
            StateInterval state = new StateInterval( in_stateinfo, group );
            group.addElement( state );
            HasStateBeenAdded = true;
        }
    }

    public void SetBeginTime()
    {
        StateGroup     cur_group         = null;

        Enumeration    groups            = super.elements();
        while ( groups.hasMoreElements() ) {
            cur_group = (StateGroup) groups.nextElement();
            cur_group.SetBeginTime();
        }
    }

    /**
     * This method goes trough the stateVector for each process and assigns 
     * an appropriate level number to each state. If no nesting is present,
     * a level of 0 will be assigned
     */
    public void SetupNestedStates()
    {
        StateInfo       prev_state_info;
        StateInfo       tmp_state_info;
        StateGroup      cur_grp;
        StateInterval   state;
        int             idx;

        max_nesting_level = 0;

        //  Scan each StateGroup 
        Enumeration groups = this.elements ();
        while ( groups.hasMoreElements() ) {
            prev_state_info = null;
            cur_grp         = (StateGroup) groups.nextElement();

            // For each StateGroup assign levels to all states 
            for ( idx = cur_grp.size() - 1; idx >= 0; idx-- ) {
                state = (StateInterval) cur_grp.elementAt(idx);
                if ( state.info.stateDef.checkbox.isSelected() ) {
                    if ( prev_state_info != null ) {
                        //
                        //  if ( prev_state_info.begT <= currState.info.begT ) {
                        //
                        if ( prev_state_info.begT < state.info.begT ) {
                            // Entering here should mean that 
                            // this state is nested inside the 
                            // previous state and hence shoud be smaller.
                            state.info.level  = prev_state_info.level + 1;
                            state.info.higher = prev_state_info;
                        }
                        else {
                            // Entering here means that 
                            // this state is independent of the previous state.
                            // however there may be higher states which
                            // contain both this and the previous state.
                            // An EFFICIENT WAY FOR THIS HAS TO BE DETERMINED.
                            tmp_state_info   = prev_state_info.higher;
                            state.info.level = prev_state_info.level;
  
                            while (  tmp_state_info      != null
                                  && tmp_state_info.begT >  state.info.endT ) {
                                // Entering here means that 
                                // the current state is definitely
                                // not containted in tmp_state_info..
                                state.info.level --;
                                tmp_state_info = tmp_state_info.higher;
                            }
                            state.info.higher = tmp_state_info;
                        }
                    }
                    prev_state_info = state.info;
                }

                if ( state.info.level > max_nesting_level )
                    max_nesting_level = state.info.level;
            }   //  Endof for()
        }   //  Endof While()
    }

    public int GetMaxNestingLevel()
    {
         return max_nesting_level;
    }

    public void ResetTimeLines()
    {
        Enumeration groups = super.elements();
        while ( groups.hasMoreElements() ) {
            ( (StateGroup) groups.nextElement() ).ResetTimeLine();
        }
    }

    public boolean DoesStateGroupExist( final StateGroupLabel in_groupID )
    {
        StateGroupLabel groupID;

        Enumeration groups = this.elements();
        while ( groups.hasMoreElements() ) {
            groupID = ( (StateGroup) groups.nextElement() ).groupID;
            if ( groupID.IsEqualTo( in_groupID ) )
                return true;
        }
        return false;
    }

    public StateGroup GetStateGroupAt( final StateGroupLabel in_groupID )
    {
        StateGroup cur_group;

        Enumeration groups = this.elements();
        while ( groups.hasMoreElements() ) {
            cur_group = (StateGroup) groups.nextElement();
            if ( cur_group.groupID.IsEqualTo( in_groupID ) )
                return cur_group;
        }
        return null;
    }

    /*  SeqIndex = Sequence Index  */
    public int GetSeqIndexAt( final StateGroupLabel in_groupID )
    throws IndexOutOfBoundsException
    {
        StateGroupLabel groupID;
        int idx = -1;

        Enumeration groups = super.elements();
        while ( groups.hasMoreElements() ) {
            idx ++;
            groupID = ( (StateGroup) groups.nextElement() ).groupID;
            if ( groupID.IsEqualTo( in_groupID ) )
                return idx;
        }

        throw new IndexOutOfBoundsException( "Error : groupID = "
                                           + in_groupID
                                           + " cannot be found!" );
    }
}
