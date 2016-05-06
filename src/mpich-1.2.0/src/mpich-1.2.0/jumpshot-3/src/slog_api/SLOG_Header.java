import java.io.*;


public class SLOG_Header
{
    private int[]  version;
    private int    frame_bytesize;
    private int    frame_reserved_size;
    private int    max_Ndirframe;
    private short  IsIncreasingStarttime;
    private short  IsIncreasingEndtime;
    private short  HasReserveSpaceBeenUsed;
    private long   fptr2statistics;
    private long   fptr2preview;
    private long   fptr2profile;
    private long   fptr2threadtable;
    private long   fptr2recdefs;
    private long   fptr2framedata;

    public SLOG_Header()
    {
        version                  = new int[ 2 ];
        frame_bytesize           = SLOG_Const.INVALID_int;
        frame_reserved_size      = SLOG_Const.INVALID_int;
        max_Ndirframe            = SLOG_Const.INVALID_int;
        IsIncreasingStarttime    = SLOG_Const.INVALID_short;
        IsIncreasingEndtime      = SLOG_Const.INVALID_short;
        HasReserveSpaceBeenUsed  = SLOG_Const.INVALID_short;
        fptr2statistics          = SLOG_Const.NULL_fptr;
        fptr2preview             = SLOG_Const.NULL_fptr;
        fptr2profile             = SLOG_Const.NULL_fptr;
        fptr2threadtable         = SLOG_Const.NULL_fptr;
        fptr2recdefs             = SLOG_Const.NULL_fptr;
        fptr2framedata           = SLOG_Const.NULL_fptr;
    }

    public SLOG_Header( RandomAccessFile file_stm )
    throws IOException
    {
        version          = new int[ 2 ];
        ReadFromRandomFile( file_stm );
    }

    public void ReadFromRandomFile( RandomAccessFile file_stm )
    throws IOException
    {
        version[ 0 ]            = file_stm.readInt();
        version[ 1 ]            = file_stm.readInt();
        frame_bytesize          = file_stm.readInt();
        frame_reserved_size     = file_stm.readInt();
        max_Ndirframe           = file_stm.readInt();
        IsIncreasingStarttime   = file_stm.readShort();
        IsIncreasingEndtime     = file_stm.readShort();
        HasReserveSpaceBeenUsed = file_stm.readShort();
        fptr2statistics         = file_stm.readLong();
        fptr2preview            = file_stm.readLong();
        fptr2profile            = file_stm.readLong();
        fptr2threadtable        = file_stm.readLong();
        fptr2recdefs            = file_stm.readLong();
        fptr2framedata          = file_stm.readLong();
    }

    public int GetFrameByteSize()
    {
        return( frame_bytesize );
    }

    public long GetFptrToStatistics()
    {
        return( fptr2statistics );
    }

    public long GetFptrToPreview()
    {
        return( fptr2preview );
    }

    public long GetFptrToProfile()
    {
        return( fptr2profile );
    }

    public long GetFptrToThreadTable()
    {
        return( fptr2threadtable );
    }

    public long GetFptrToRecDefs()
    {
        return( fptr2recdefs );
    }

    public long GetFptrToFrameData()
    {
        return( fptr2framedata );
    }

    public String toString()
    {
        StringBuffer rep = new StringBuffer();

        rep.append( "version = " + version[0] + "." + version[1] + "\n" );
        rep.append( "frame byte size                         = "
                    + frame_bytesize + "\n" );
        rep.append( "byte size of the frame reserved space   = "
                    + frame_reserved_size + "\n" );
        rep.append( "max. number of frames per directory     = "
                    + max_Ndirframe + "\n" );

        if ( IsIncreasingStarttime == SLOG_Const.TRUE )
            rep.append( "Is_Increasing_Starttime_Order           = TRUE\n" );
        else
            rep.append( "Is_Increasing_Starttime_Order           = FALSE\n" );

        if ( IsIncreasingEndtime == SLOG_Const.TRUE )
            rep.append( "Is_Increasing_Endtime_Order             = TRUE\n" );
        else
            rep.append( "Is_Increasing_Endtime_Order             = FALSE\n" );

        if ( HasReserveSpaceBeenUsed == SLOG_Const.TRUE )
            rep.append( "Has_ReserveSpace()_Been_Used            = TRUE\n" );
        else
            rep.append( "Has_ReserveSpace()_Been_Used            = FALSE\n" );

        rep.append( "location of Preview Statistics          = "
                    + fptr2statistics + "\n" );
        rep.append( "location of Preview                     = "
                    + fptr2preview + "\n" );
        rep.append( "location of Threads Table               = "
                    + fptr2threadtable + "\n" );
        rep.append( "location of Profile                     = "
                    + fptr2profile + "\n" );
        rep.append( "location of Record Definition Table     = "
                    + fptr2recdefs + "\n" );
        rep.append( "location of FRAMES Data                 = "
                    + fptr2framedata + "\n" );

        return rep.toString();
    }
}
