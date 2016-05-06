import java.io.*;


public class SLOG_InputStream
{
    private String            filename;
    private int               frame_bsize;
    private RandomAccessFile  file_stm;
    private DataInputStream   data_istm;

    public  SLOG_Header       header;
    public  SLOG_Statistics   statistics;
    public  SLOG_Preview      preview;
    public  SLOG_Profile      profile;

    public  SLOG_ThreadInfos  threadinfos;
    public  SLOG_RecDefs      recdefs;

    //  For now there is only 1 directory.  Eventually there will be collection
    //  of directories, like "Vector of SLOG_Dir", i.e. dirs
    public  SLOG_Dir          dir;

    private SLOG_Buffer       buffer;

    public SLOG_InputStream( String file_name )
    throws IOException
    {
        filename = new String( file_name );
        file_stm = new RandomAccessFile( filename, "r" );

        header      = new SLOG_Header( file_stm );
        statistics  = new SLOG_Statistics( file_stm,
                                           header.GetFptrToStatistics() );
        preview     = new SLOG_Preview( file_stm, header.GetFptrToPreview() );
        profile     = new SLOG_Profile( file_stm, header.GetFptrToProfile() );

        threadinfos = new SLOG_ThreadInfos( file_stm,
                                            header.GetFptrToThreadTable() );
        recdefs     = new SLOG_RecDefs( file_stm, header.GetFptrToRecDefs() );

        file_stm.seek( header.GetFptrToFrameData() );
        dir         = new SLOG_Dir( file_stm );

        frame_bsize = header.GetFrameByteSize();
        buffer      = new SLOG_Buffer( file_stm, frame_bsize );
    }

    public SLOG_Frame GetFrame()
    throws IOException, IllegalArgumentException
    {
        SLOG_Frame frame = new SLOG_Frame();

        buffer.ReadFrame();
        frame.Init();
        frame.ReadFromDataStream( buffer.GetDataInputStream(), recdefs );

        return( frame );
    }

    public SLOG_Frame GetFrame( long file_ptr )
    throws IOException, IllegalArgumentException
    {
        SLOG_Frame frame = new SLOG_Frame();

        buffer.ReadFrame( file_ptr );
        frame.Init();
        data_istm = buffer.GetDataInputStream();
        frame.ReadFromDataStream( data_istm, recdefs );

        return( frame );
    }

    public String toString()
    {
        StringBuffer rep = new StringBuffer();
        rep.append( "\t" + header + "\n" );
        rep.append( "\t" + statistics + "\n" );
        rep.append( "\t" + preview + "\n");
        rep.append( "\t" + profile + "\n" );
        rep.append( "\t" + threadinfos + "\n" );
        rep.append( "\t" + recdefs + "\n" );
        rep.append( "\t" + dir + "\n" );

        return( rep.toString() );
    }
}
