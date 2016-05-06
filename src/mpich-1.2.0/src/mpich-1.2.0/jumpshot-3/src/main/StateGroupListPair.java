import java.io.*;
import java.util.Vector;
import java.util.Enumeration;

public class StateGroupListPair
{
    StateGroupList  visible;
    StateGroupList  deleted;

    public StateGroupListPair()
    {
        visible          = new StateGroupList();
        deleted          = new StateGroupList();
    }

    /**
     * Move the state group with the group label, mv_groupID, from "visible"
     * and append it to the end of the "deleted"
     */
    public void Visible2Deleted( final StateGroupLabel mv_groupID )
    {
        int seq_idx = visible.GetSeqIndexAt( mv_groupID );
        StateGroup cur_group = (StateGroup) visible.elementAt( seq_idx );
        visible.removeElementAt( seq_idx );
        // visible.Ngroups --;
        deleted.addElement( cur_group );
        // deleted.Ngroups ++;
    }

    /**
     * Delete state group with the group label, mv_groupID, from "deleted" 
     * and __insert__ it on top of the state group with the group label,
     * vis_at_groupID, into "visible"
     */
    public void Deleted2Visible( final StateGroupLabel mv_groupID,
                                 final StateGroupLabel vis_at_groupID )
    {
        int del_seq_idx = deleted.GetSeqIndexAt( mv_groupID );
        StateGroup del_group = (StateGroup) deleted.elementAt( del_seq_idx );
        deleted.removeElementAt( del_seq_idx );
        // deleted.Ngroups --;
        int at_seq_idx = visible.GetSeqIndexAt( vis_at_groupID );
        visible.insertElementAt( del_group, at_seq_idx );
        // visible.Ngroups ++;
    }

    public void ResetTimeLines()
    {
        visible.ResetTimeLines();
        deleted.ResetTimeLines();
    }
}
