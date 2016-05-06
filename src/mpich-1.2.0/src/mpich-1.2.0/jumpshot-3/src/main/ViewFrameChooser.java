import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import javax.swing.text.*;

public class ViewFrameChooser extends JFrame
                              implements DocumentListener, ActionListener,
                                         TimeInterface
{
    private Graph_Pane       stat_preview = null;
    private int              Npreview;
    private int              Nset;

    private int              Nconnect = 2;
    private String[]         connect_names = { "Disconnected States",
                                               "Connected States" };
    private int              connect_idx = 0;
    private JRadioButton[]   connect_btns;

    private int              Nview;
    private String[]         view_names;
    private int              view_idx;
    private JRadioButton[]   view_btns;

    private int              Nframe;
    private int              frame_idx;
    private JLabel           frame_idx_tag;
    private JTextField       frame_idx_fld;
    private JButton          prev_btn;
    private JButton          display_btn;
    private JButton          next_btn;

    private JButton          close_btn;

            Mainwin          init_win;
            SLOG_InputStream slog = null;
            SLOG_Frame       slog_frame = null;
            PlotData         slog_plotdata = null;

    private FrameDisplay     frame_display = null;
    // private ProgressMonitor  progress_mntr = null;
    private ProgressDialog   progress_mntr = null;
    private FrameReadingTask task = null;
    private Timer            timer = null;


    public ViewFrameChooser( final SLOG_InputStream  in_slog,
                             final String[]          in_view_names )
    {
        super();

        slog = in_slog;
        int  in_Nframe = slog.dir.NumOfFrames();

        if ( in_Nframe > 0 ) {
            Nframe    = in_Nframe;
            frame_idx = 0;
        }
        else
            return;

        if ( in_view_names.length > 0 ) {
            Nview           = in_view_names.length;
            view_idx        = 1;
            view_names      = new String[ Nview ];
            for ( int ii = 0; ii < Nview; ii++ )
                view_names[ ii ] = new String( in_view_names[ ii ] );
        }
        else
            return;

        Npreview = slog.statistics.Nbin;
        Nset     = slog.statistics.Nset;
        String pview_legends[] = new String[ Nset ];
        double pview_vals[][]  = new double[ Nset ][ Npreview ];

        Enumeration enum = slog.statistics.sets.elements();
        SLOG_StatSet statset;
        int idx = 0;
        while ( enum.hasMoreElements() ) {
            statset = ( SLOG_StatSet ) enum.nextElement();
            pview_legends[ idx ] = new String( statset.label );
            for ( int ii = 0; ii < Npreview; ii++ )
                pview_vals[ idx ][ ii ] = statset.bins[ ii ]; 
            idx++;
        }

        // top_panel consists 3 subpanels: 
        // view_panel, idx_panel( text field ), btn_panel( buttons )
        setTitle( "View & Frame Selector" );
        //  setSize( 300, 200 );
        setBackground( Color.gray );
        getContentPane().setLayout( new GridBagLayout() );
        GridBagConstraints constraint = new GridBagConstraints();

        //  Specify stat_preview panel
        Border bdr1 = BorderFactory.createEmptyBorder (4, 4, 2, 4);
        Border bdr2 = BorderFactory.createLoweredBevelBorder();
        Border bdr3 = BorderFactory.createCompoundBorder( bdr1, bdr2 );
        JPanel top_panel = new JPanel();
        top_panel.setBorder( bdr3 );
        top_panel.setLayout( new GridBagLayout() );

            stat_preview = new Graph_Pane( this, "Event Count vs Time",
                                           pview_vals, pview_legends,
                                           slog.dir.StartTime(),
                                           slog.dir.EndTime() );
        
            constraint.fill  = GridBagConstraints.NONE;
            constraint.gridwidth = 1; constraint.gridheight = 1;
            constraint.gridx = 0; constraint.gridy = 0;
            top_panel.add( stat_preview, constraint );
            stat_preview.setTimeListener( this );
        
        constraint.fill  = GridBagConstraints.BOTH;
        constraint.gridwidth = 2; constraint.gridheight = 1;
        constraint.gridx = 0; constraint.gridy = 0;
        getContentPane().add( top_panel, constraint );
        UpdateGraphTimeMarker();

        //  Specify connect_panel
        JPanel connect_panel = new JPanel();
        connect_panel.setBorder( new TitledBorder( new EtchedBorder(),
                                                   "Connectivity Options" ) );
            connect_btns = new JRadioButton[ Nconnect ];
            ButtonGroup connect_group = new ButtonGroup();
            for ( int ii = 0; ii < Nconnect; ii++ ) {
                connect_btns[ ii ] = new JRadioButton( connect_names[ ii ] );
                // connect_btns[ ii ].addActionListener( this );
                connect_group.add( connect_btns[ ii ] );
                connect_panel.add( connect_btns[ ii ] );
            }
            connect_btns[ connect_idx ].doClick();
            //  Temporary disable the Connected States Button
            connect_btns[ Nconnect - 1 ].setEnabled( false );
            //
        constraint.fill  = GridBagConstraints.HORIZONTAL;
        constraint.gridwidth = 1; constraint.gridheight = 1;
        constraint.gridx = 0; constraint.gridy = 1;
        getContentPane().add( connect_panel, constraint );

        //  Specify view_panel
        JPanel view_panel = new JPanel();
        view_panel.setBorder( new TitledBorder( new EtchedBorder(),
                                                "View Options" ) );
            view_btns = new JRadioButton[ Nview ];
            ButtonGroup view_group = new ButtonGroup();
            for ( int ii = 0; ii < Nview; ii++ ) {
                view_btns[ ii ] = new JRadioButton( view_names[ ii ] );
                // view_btns[ ii ].addActionListener( this );
                view_group.add( view_btns[ ii ] );
                view_panel.add( view_btns[ ii ] );
            }
            view_btns[ view_idx ].doClick();
            constraint.fill  = GridBagConstraints.HORIZONTAL;
            constraint.gridwidth = 1; constraint.gridheight = 1;
        constraint.gridx = 1; constraint.gridy = 1;
        getContentPane().add( view_panel, constraint );
        
        //  Specify idx_panel( text field )
        JPanel idx_panel = new JPanel();
        idx_panel.setBorder( new TitledBorder( new EtchedBorder(),
                                               "Frame Information" ) );
        idx_panel.setLayout( new GridBagLayout() );
        constraint.fill  = GridBagConstraints.HORIZONTAL;

            JLabel info_tag = new JLabel( " Number of Frames = " );
            constraint.gridx = 0; constraint.gridy = 0;
            idx_panel.add( info_tag, constraint );

            JLabel info_value = new JLabel( Integer.toString( Nframe ) );
            constraint.gridx = 1; constraint.gridy = 0;
            idx_panel.add( info_value, constraint );

            frame_idx_fld = new JTextField( 10 );
            frame_idx_fld.setHorizontalAlignment( JTextField.CENTER );
            frame_idx_fld.setText( Integer.toString( frame_idx ) );

            frame_idx_tag = new JLabel( " Current Frame        = " );
            frame_idx_tag.setLabelFor( frame_idx_fld );

            constraint.gridx = 0; constraint.gridy = 1;
            idx_panel.add( frame_idx_tag, constraint );
            constraint.gridx = 1; constraint.gridy = 1;
            idx_panel.add( frame_idx_fld, constraint );

        constraint.fill  = GridBagConstraints.BOTH;
        constraint.gridwidth = 1; constraint.gridheight = 1;
        constraint.gridx = 0; constraint.gridy = 2;
        getContentPane().add( idx_panel, constraint );

        //  Specify btn_panel( buttons )
        JPanel btn_panel = new JPanel();
        btn_panel.setBorder( new TitledBorder( new EtchedBorder(),
                                               "Frame Operations" ) );
        btn_panel.setLayout( new FlowLayout() );
        Dimension btn_dim = new Dimension( 90, 50 );

            ImageIcon back_arrow_icon
                      = new ImageIcon( "images/prev_arrow.gif" );
            prev_btn = new JButton( "Previous", back_arrow_icon );
            prev_btn.setVerticalTextPosition( AbstractButton.BOTTOM );
            prev_btn.setHorizontalTextPosition( AbstractButton.CENTER );
            // prev_btn.setPreferredSize( btn_dim );
            prev_btn.setToolTipText( "Decrement the frame index counter" );
            prev_btn.setEnabled( false );
            // prev_btn.addActionListener( this );
            btn_panel.add( prev_btn ); 

            ImageIcon load_arrow_icon
                      = new ImageIcon( "images/load_arrow.gif" );
            display_btn = new JButton( "Display", load_arrow_icon );
            display_btn.setVerticalTextPosition( AbstractButton.BOTTOM );
            display_btn.setHorizontalTextPosition( AbstractButton.CENTER );
            // display_btn.setPreferredSize( btn_dim );
            display_btn.setPreferredSize( prev_btn.getPreferredSize() );
            display_btn.setToolTipText( "Read the requested frame "
                                      + "and Display it" );
            display_btn.setEnabled( false );
            // display_btn.addActionListener( this );
            btn_panel.add( display_btn );

            ImageIcon next_arrow_icon
                      = new ImageIcon( "images/next_arrow.gif" );
            next_btn = new JButton( "Next", next_arrow_icon );
            next_btn.setVerticalTextPosition( AbstractButton.BOTTOM );
            next_btn.setHorizontalTextPosition( AbstractButton.CENTER );
            // next_btn.setPreferredSize( btn_dim );
            next_btn.setPreferredSize( prev_btn.getPreferredSize() );
            next_btn.setToolTipText( "Increment the frame index counter" );
            next_btn.setEnabled( false );
            // next_btn.addActionListener( this );
            btn_panel.add( next_btn ); 

        constraint.fill  = GridBagConstraints.BOTH;
        constraint.gridwidth = 1; constraint.gridheight = 1;
        constraint.gridx = 1; constraint.gridy = 2;
        getContentPane().add( btn_panel, constraint );

        //  Specify the various components for this Action Listener
            for ( int ii = 0; ii < Nconnect; ii++ ) 
                connect_btns[ ii ].addActionListener( this );
            for ( int ii = 0; ii < Nview; ii++ )
                view_btns[ ii ].addActionListener( this );
            prev_btn.addActionListener( this );
            display_btn.addActionListener( this );
            next_btn.addActionListener( this );

        //  Specify the "close" button
/*
            close_btn = new JButton( "Close" );
            close_btn.setToolTipText( "Close this window" );
            close_btn.addActionListener( this );

        constraint.fill = GridBagConstraints.EAST;
        constraint.gridwidth = 2; constraint.gridheight = 1;
        constraint.gridx = 0; constraint.gridy = 3;
        getContentPane().add( close_btn, constraint );
*/

        frame_idx_fld.getDocument().addDocumentListener( this );

        FrameButtonsIdxFldEnabled( frame_idx_fld.getText() );

        addWindowListener( new WindowAdapter()
                               {
                                   public void windowClosing( WindowEvent e )
                                   { 
                                       init_win.enableRead();
                                       init_win.slog = null;
                                       init_win.frame_chooser = null;
                                       free();
                                   }
                               }
                         );
    }

    //  Set Frame Buttons according to the frame_idx and Nframe
    public void EnableFrameButtonsIdxFld()
    {
        frame_idx_fld.setEnabled( true );

        if ( frame_idx >= 0 && frame_idx < Nframe ) {
            if ( frame_idx > 0 )
                prev_btn.setEnabled( true );
            else
                prev_btn.setEnabled( false );

            display_btn.setEnabled( true );

            if ( frame_idx < Nframe-1 )
                next_btn.setEnabled( true );
            else
                next_btn.setEnabled( false );
        }
        else {
            prev_btn.setEnabled( false );
            display_btn.setEnabled( false );
            next_btn.setEnabled( true );
        }
    }

    public void DisableFrameButtonsIdxFld()
    {
        frame_idx_fld.setEnabled( false );

        prev_btn.setEnabled( false );
        display_btn.setEnabled( false );
        next_btn.setEnabled( false );
    }

    private void FrameButtonsIdxFldEnabled( String str )
    {
        if ( str.length() > 0 ) {
            try { 
                frame_idx = Integer.parseInt( str );
                EnableFrameButtonsIdxFld();
                return; 
            }
            catch ( NumberFormatException nerr ) { 
                System.err.println( "Invalid number format" );
            }
        }

        DisableFrameButtonsIdxFld();
    }

    public void changedUpdate( DocumentEvent event )
    {
        FrameButtonsIdxFldEnabled( frame_idx_fld.getText() );
        UpdateGraphTimeMarker();
    }

    public void insertUpdate( DocumentEvent event )
    {
        changedUpdate( event );
    }

    public void removeUpdate( DocumentEvent event )
    {
        changedUpdate( event );
    }

    public void actionPerformed( ActionEvent event )
    {
        for ( int ii = 0; ii < Nconnect; ii++ )
            if ( event.getSource() == connect_btns[ ii ] ) {
                connect_idx = ii;
                EnableFrameButtonsIdxFld();
                return;
            }

        for ( int jj = 0; jj < Nview; jj++ )
            if ( event.getSource() == view_btns[ jj ] ) {
                view_idx = jj;
                EnableFrameButtonsIdxFld();
                return;
            }

        if ( event.getSource() == display_btn ) {
            debug.println( "Frame Index = " + frame_idx + ",  "
                         + "View Action Index = " + view_idx + ",  "
                         + "Connect Action Index = " + connect_idx );
            UpdateGraphTimeMarker();
            ReadLogFrame();
            // DisplayFrame();
            return;
        }
        if ( event.getSource() == prev_btn ) {
            frame_idx--;
            frame_idx_fld.setText( Integer.toString( frame_idx ) );
            UpdateGraphTimeMarker();
            return;
        }
        if ( event.getSource() == next_btn ) {
            frame_idx++;
            frame_idx_fld.setText( Integer.toString( frame_idx ) );
            UpdateGraphTimeMarker();
            return;
        }

/*
        if ( event.getSource() == close_btn ) {
            init_win.enableRead();
            init_win.slog = null;
            init_win.frame_chooser = null;
            free();
            setVisible( false );
            dispose();
            // else
            //    new ErrorDiag( this, "The child Frame Display still exists" );
        }
*/
    }

/*
    public void ReadLogFrame()
    {
        SLOG_DirEntry    dir_entry = null;

        if ( frame_idx >= 0 ) {
            display_btn.setEnabled( false );
            SLOG_DirEntry dir_entry = slog.dir.EntryAt( frame_idx );
            try { slog_frame = slog.GetFrame( dir_entry.fptr2framehdr ); }
            catch ( IOException ioerr )
                { System.err.println( "ReadLogFrame() has IO error" ); }
            debug.println( "slog_dir_entry = " + dir_entry );
            slog_plotdata = new PlotData( slog, slog_frame,
                                          connect_idx, view_idx );
            display_btn.setEnabled( true );
        }
    }
*/
    private void ReadLogFrame()
    {
        final ViewFrameChooser frame_chooser = this;

        if ( frame_idx >= 0 ) {
            task = new FrameReadingTask( this,
                                         slog, frame_idx,
                                         connect_idx, view_idx );
            timer = new Timer( 100, new RefreshTimerListener() );
            // timer.setRepeats( true );
            timer.setInitialDelay( 0 );
            timer.setCoalesce( true );
            // progress_mntr = new ProgressMonitor( this,
            progress_mntr =  new ProgressDialog( this,
                                                 "Reading from the logfile",
                                                 "Reading Frame ",
                                                 task.GetMinProgIdx(),
                                                 task.GetMaxProgIdx() );
            progress_mntr.setMillisToDecideToPopup( 0 );
            progress_mntr.setMillisToPopup( 0 );
            progress_mntr.setNote( task.GetMessage() );
            progress_mntr.setProgress( task.GetMinProgIdx() );

            waitCursor();
            display_btn.setEnabled( false );
            task.Start();
            timer.start();
            if ( timer.isRunning() )
                System.out.println( "Timer is running" );
            else
                System.out.println( "Timer is NOT running" );
        }
        else
            new ErrorDiag( this, "ViewFrameChooser.ReadLogFrame(): "
                                 + "Invalid Frame Index " + frame_idx );
    }

    class RefreshTimerListener implements ActionListener
    {
        private int   itick              = 0;
        private int   min_prog_idx       = task.GetMinProgIdx();
        private int   max_prog_idx       = task.GetMaxProgIdx();
        private int   duration_prog      = max_prog_idx - min_prog_idx;
        private int   ten_percent_prog   = duration_prog / 10;
        private int   cur_prog_idx       = min_prog_idx;
        private int   task_prog_idx      = min_prog_idx;
        private long  ten_percent_msec   = 0;
        private long  unit_msec          = 0;
        private long  start_msec         = ( new Date() ).getTime();
        private long  elapsed_msec;

        public void actionPerformed( ActionEvent evt )
        {
            itick++;

            if ( progress_mntr.isCanceled() || task.Finished() ) {
                progress_mntr.close();
                task.Stop();
                Toolkit.getDefaultToolkit().beep();
                timer.stop();
                display_btn.setEnabled( true );
            }
            else {
                progress_mntr.setNote( task.GetMessage() );
                //  A complication algorithm to estimate the progress
                //  Assume 1 itick corresponds to 10% progress,
                //  Then use this to set a 10% real time progress benchmark.
                //  Or when the task.GetCurProgIdx() return non-zero value
                //  which will be used to benchmark the progress.
                elapsed_msec = ( new Date() ).getTime() - start_msec;
                task_prog_idx = task.GetCurProgIdx();
                if ( itick > 0 ) {
                    if ( task_prog_idx == min_prog_idx ) {
                        if ( ten_percent_msec == 0 )
                            ten_percent_msec = elapsed_msec / itick;
                        cur_prog_idx = (int) ( elapsed_msec
                                               / ten_percent_msec )
                                               * ten_percent_prog;
                    }
                    else {
                        if ( unit_msec == 0 )
                            unit_msec = elapsed_msec / task_prog_idx ;
                        cur_prog_idx = (int) ( elapsed_msec / unit_msec );
                    }
                }
                else 
                    cur_prog_idx = task_prog_idx;

                progress_mntr.setProgress( cur_prog_idx % duration_prog );
            }

            System.out.println( "RefreshTimerListener: task.GetCurProgIdx() = "
                              + task_prog_idx + ", " + itick + ", "
                              + cur_prog_idx  + ", " + elapsed_msec );
        }
    }

    public void DisplayFrame()
    {
        slog_plotdata = task.GetPlotData();

        if ( slog_plotdata != null ) {
            display_btn.setEnabled( false );
            waitCursor();

            StringBuffer title_str = new StringBuffer();
                title_str.append( connect_names[ connect_idx ] + " in " );
                title_str.append( view_names[ view_idx ] + " view " );
                title_str.append( "at frame index = " + frame_idx );

            SLOG_DirEntry slog_dir_entry = slog.dir.EntryAt( frame_idx );

            frame_display = new FrameDisplay( init_win, this, slog_plotdata,
                                              slog_dir_entry.starttime,
                                              slog_dir_entry.endtime,
                                              title_str.toString() );
            normalCursor();
            /*
            frame_display.addWindowListener( new WindowAdapter()
                {
                    public void windowClosing( WindowEvent e )
                    { 
                        display_btn.setEnabled( true );
                        SetFrameButtonsEnabled( frame_idx_fld.getText() );
                    };
                }
                                           );
            */
        }
        else
            new ErrorDiag( this, "ViewFrameChooser.DisplayFrame(): "
                                 + "slog_plotdata == null" );
    }

    private void free()
    {
        for ( int ii = 0; ii < Nconnect; ii++ )
            connect_btns[ ii ] = null;
        for ( int ii = 0; ii < Nview; ii++ ) {
            view_btns[ ii ] = null;
            view_names[ ii ] = null;
        }
        frame_idx_tag  = null;
        frame_idx_fld  = null;
        prev_btn       = null;
        display_btn    = null;
        next_btn       = null;

        slog           = null;
        slog_frame     = null;
        slog_plotdata  = null;

        frame_display  = null;
    }

    public void SetInitWindow( Mainwin in_initwin )
    {
        init_win = in_initwin;
    }

    public void SetLogFileHdr( SLOG_InputStream in_slog )
    {
        slog = in_slog;
    }

    public int GetFrameIdx()
    {
        return frame_idx;
    }

    public int GetViewActionIdx()
    {
        return view_idx;
    }

    public int GetConnectActionIdx()
    {
        return connect_idx;
    }

    //  To implemet the required TimeInterface needed
    public void showTime( double time )
    {
        SLOG_DirEntry   dir_entry;

        for ( int idx = 0; idx < Nframe; idx++ ) {
            dir_entry = slog.dir.EntryAt( idx );
            if ( time >= dir_entry.starttime && time < dir_entry.endtime ) {
                frame_idx = idx;
                frame_idx_fld.setText( Integer.toString( frame_idx ) );
                // display_btn.doClick();
                break;
            }
        }
    }

    public void closePreview()
    {
        init_win.enableRead();
        init_win.slog = null;
        init_win.frame_chooser = null;
        free();
        super.setVisible( false );
        dispose();
    }

    private void UpdateGraphTimeMarker()
    {
        if ( frame_idx < Nframe ) {
            SLOG_DirEntry dir_entry = slog.dir.EntryAt( frame_idx );
            double frame_time_marker = ( dir_entry.starttime
                                       + dir_entry.endtime ) / 2.0;
            stat_preview.moveMarkerTo( frame_time_marker );
        }
    }

    /**
     * sets the current cursor to DEFAULT_CURSOR
     */
    void normalCursor()
    {
        ROUTINES.makeCursor( this, new Cursor( Cursor.DEFAULT_CURSOR ) );
    }

    /**
     * sets the current cursor to WAIT_CURSOR
     */
    void waitCursor()
    {
        ROUTINES.makeCursor( this, new Cursor( Cursor.WAIT_CURSOR ) );
    }
}
