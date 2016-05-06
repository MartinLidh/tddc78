import java.io.*;
import java.util.*;

public class StateGroupLabel
{
    private static String IndexesLabel[] = { "AppID:0",
                                             "AppID:LocalThreadID",
                                             "NodeID:LocalCpuID" };

    private int idx1;
    private int idx2;

    public StateGroupLabel()
    {
        idx1 = 0;
        idx2 = 0;
    }

    public StateGroupLabel( int in_idx1, int in_idx2 )
    {
        idx1 = in_idx1;
        idx2 = in_idx2;
    }

    public StateGroupLabel( final StateGroupLabel label )
    {
        idx1 = label.idx1;
        idx2 = label.idx2;
    }

    public StateGroupLabel( String label_str )
    throws NoSuchElementException, IndexOutOfBoundsException
    {
        StringTokenizer token_strs = new StringTokenizer( label_str.trim(),
                                                          " :" );
        int count = token_strs.countTokens();
        if ( count == 2 ) {
            idx1 = Integer.parseInt( token_strs.nextToken().trim() );
            idx2 = Integer.parseInt( token_strs.nextToken().trim() );
        }
        else {
            throw new IndexOutOfBoundsException(
                      "Incorrect number of tokens = "
                      + token_strs.countTokens() );
        }
    }

    public StateGroupLabel( final SLOG_ThreadInfo threadinfo, int view_idx )
    {
        this.SetIndexes( threadinfo, view_idx );
    }

    public StateGroupLabel( final SLOG_tasklabel    task_id,
                            final SLOG_ThreadInfos  thread_infos,
                            final int               view_idx )
    {
        this.SetIndexes( task_id, thread_infos, view_idx );
    }

    public static String GetIndexesLabel( int view_idx )
    {
        return IndexesLabel[ view_idx ];
    }

    public void SetIndexes( final SLOG_ThreadInfo threadinfo, int view_idx )
    {
        switch ( view_idx ) {
            case 0:
                idx1 = threadinfo.app_id;
                idx2 = 0;
                break;
            case 1:
                idx1 = threadinfo.app_id;
                idx2 = threadinfo.thread_id;
                break;
            case 2:
                idx1 = threadinfo.node_id;
                idx2 = 0;
                break;
            default:
                idx1 = 0;
                idx2 = 0;
        }
    }

    public void SetIndexes( final SLOG_tasklabel    task_id,
                            final SLOG_ThreadInfos  thread_infos,
                            final int               view_idx )
    {
        switch ( view_idx ) {
            case 0:
                idx1 = thread_infos.GetAppID( task_id.node,
                                              task_id.thread );
                idx2 = 0;
                break;
            case 1:
                idx1 = thread_infos.GetAppID( task_id.node,
                                              task_id.thread );
                idx2 = task_id.thread;
                break;
            case 2:
                idx1 = task_id.node;
                idx2 = task_id.cpu;
                break;
            default:
                idx1 = 0;
                idx2 = 0;
        }
    }

    public boolean IsEqualTo( StateGroupLabel label )
    {
        if ( idx1 == label.idx1 && idx2 == label.idx2 )
            return true;
        else
            return false;
    }

    public boolean IsGreaterThan( StateGroupLabel label )
    {
        if ( idx1 > label.idx1 )
            return true;
        else if ( idx1 == label.idx1 && idx2 > label.idx2 )
            return true;
        else
            return false;
    }

    public boolean IsLessThan( StateGroupLabel label )
    {
        if ( idx1 < label.idx1 )
            return true;
        else if ( idx1 == label.idx1 && idx2 < label.idx2 )
            return true;
        else
            return false;
    }

    public String toString()
    {
        return ( Integer.toString( idx1 ) + ":" + Integer.toString( idx2 ) );
    }
}
