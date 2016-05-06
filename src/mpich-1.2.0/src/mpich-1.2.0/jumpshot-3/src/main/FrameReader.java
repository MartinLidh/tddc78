import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class FrameReader extends JDialog
//                         implements ActionListener
{
    private ViewFrameChooser  frame_chooser = null;
    private SLOG_InputStream  slog = null;
    private int               frame_idx = 0;
    private int               connect_idx = 0;
    private int               view_idx = 0;

    private SLOG_Frame        slog_frame = null;
    private PlotData          slog_plotdata = null;

    private JLabel            status_fld;
    private int               max_char_ary_size = 40;

    private JProgressBar      progress_bar;
    private static int        min_bar_idx = 0;
    private static int        max_bar_idx = 100;

    private JButton           cancel_btn;

    private Timer             timer;
    public final static int   refresh_intvl = 1000;   // in 1/1000 sec


    public FrameReader(       ViewFrameChooser in_chooser,
                        final SLOG_InputStream in_slog,
                              int              in_frame_idx,
                              int              in_connect_idx,
                              int              in_view_idx )
    {
        super( in_chooser );
        frame_chooser = in_chooser;
        slog          = in_slog;
        frame_idx     = in_frame_idx;
        connect_idx   = in_connect_idx;
        view_idx      = in_view_idx;

        SetupPanel();
        // timer = new Timer( refresh_intvl, new TimerListener() );
        // timer.start();

        ReadFrameAndConvert();
        
        this.dispose();
    }

    private void SetupPanel()
    {
        setTitle( "Reading the SLOG frame" );
        // setBackground( Color.gray );
        getContentPane().setLayout( new GridBagLayout() );
        GridBagConstraints con = new GridBagConstraints();

        JPanel top_pane = new JPanel();
        // top_pane.setBorder( BorderFactory.createEmptyBorder(20, 20, 20, 20) );
        top_pane.setLayout( new GridBagLayout() );

            char [] char_ary = new char[ max_char_ary_size ];
            for ( int ii = 0; ii < max_char_ary_size; ii++ )
                char_ary[ ii ] = ' ';
            status_fld = new JLabel( new String( char_ary ) );

        con.gridx = 0; con.gridy = 0;
        top_pane.add( status_fld, con );

            progress_bar = new JProgressBar( min_bar_idx, max_bar_idx );
            progress_bar.setValue( min_bar_idx );

        con.gridx = 0; con.gridy = 1;
        top_pane.add( progress_bar, con );

        /*
            cancel_btn = new JButton( "Cancel" );

        con.gridx = 1; con.gridy = 2;
        top_pane.add( cancel_btn, con );
        */
        
        con.gridx = 0; con.gridy = 0;
        getContentPane().add( top_pane, con );

        pack();
        setVisible( true );
        // show();
    }

    private void ReadFrameAndConvert()
    {
        SLOG_DirEntry dir_entry = slog.dir.EntryAt( frame_idx );
        try {
            SetStatusFieldText( "Reading Frame from Logfile" );
            slog_frame = slog.GetFrame( dir_entry.fptr2framehdr );
        } catch ( IOException ioerr ) {
            System.err.println( "slog.GetFrame() ) has IO error" );
        }
        if ( slog_frame != null ) {
            SetStatusFieldText( "Converting Selected Frame to Ploting data" );
            slog_plotdata = new PlotData( slog, slog_frame,
                                          connect_idx, view_idx );
        }
        else
            System.err.println( "FrameReader(): slog.GetFrame() return null" );
        // frame_chooser.EnableFrameButtonsIdxFld();
        // frame_chooser.DisplayFrame();
    }

    class TimerListener implements ActionListener
    {
        private int index = 0;
        public void actionPerformed( ActionEvent evt )
        {
            index ++;
            SetProgressBarValue( index % max_bar_idx );
        }
    }

    public void actionPerformed( ActionEvent event )
    {
        if ( event.getSource() == cancel_btn ) {
            this.dispose();
        }
    }

    private void SetProgressBarValue( final int idx )
    {
        Runnable DoSetProgressBarValue = new Runnable()
        {
            public void run()
            {   progress_bar.setValue( idx );   }
        };
        SwingUtilities.invokeLater( DoSetProgressBarValue );
    }


    private void SetStatusFieldText( final String str )
    {
        Runnable DoSetStatusFieldText = new Runnable()
        {
            public void run()
            {   status_fld.setText( str );   }
        };
        SwingUtilities.invokeLater( DoSetStatusFieldText );
    }
    
    public PlotData GetPlotData()
    {
        return( slog_plotdata );   
    }

    /*
    public boolean IsAlive()
    {
        return( reader_alive );
    }
    */
}
