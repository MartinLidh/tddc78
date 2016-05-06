import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

import java.util.*;

public class ProgressDialog extends JDialog
{
    private Component         parent;

    private JLabel            header_msg;
    private String            message;

    private JTextArea         status_area;
    private int               status_area_height = 4;
    private int               status_area_width  = 25;
    private String            note;

    private JProgressBar      progress_bar;
    private int               min_bar_idx = 0;
    private int               max_bar_idx = 100;

    private JButton           cancel_btn;
    private boolean           canceled = false;
    private String            newline;

    public ProgressDialog( Component  in_parent,
                           String     in_message, String     in_note,
                           int        in_min,     int        in_max )
    {
        super( (Frame) in_parent );
        parent        = in_parent;
        message       = new String( in_message );
        note          = new String( in_note );
        min_bar_idx   = in_min;
        max_bar_idx   = in_max;

        SetupPanel();

        pack();
        setVisible( true );
    }

    private void SetupPanel()
    {
        newline = System.getProperty("line.separator");

        setTitle( "Progress Status Dialog" );
        JPanel top_pane = new JPanel();
        top_pane.setLayout( new GridBagLayout() );
        top_pane.setBorder( BorderFactory.createEmptyBorder(10,5,10,5) );
        GridBagConstraints con = new GridBagConstraints();

            header_msg = new JLabel( message );
            header_msg.setBorder( BorderFactory.createEmptyBorder(5,5,10,5) );

        con.gridx = 0; con.gridy = 0;
        top_pane.add( header_msg, con );

            JPanel progress_pane = new JPanel();
            progress_pane.setBorder( new TitledBorder( new EtchedBorder(),
                                         " Progress Indicator " ) );
                progress_bar = new JProgressBar( min_bar_idx, max_bar_idx );
                progress_bar.setValue( min_bar_idx );
                progress_bar.setStringPainted( true );
                Dimension pbar_size = new Dimension( status_area_width * 12,
                                                     20 );
                progress_bar.setPreferredSize( pbar_size ); 
            progress_pane.add( progress_bar );

        con.gridx = 0; con.gridy = 2;
        con.anchor = GridBagConstraints.CENTER;
        top_pane.add( progress_pane, con );

            JPanel status_pane = new JPanel();
            status_pane.setBorder( new TitledBorder( new EtchedBorder(),
                                                     " Status " ) );
                status_area = new JTextArea( status_area_height,
                                             status_area_width );
                status_area.setMargin( new Insets(5,5,5,5) );
                status_area.setEditable( false );
                status_area.append( note + newline );
            status_pane.add( new JScrollPane( status_area ) );

        con.gridx = 0; con.gridy = 3;
        con.anchor = GridBagConstraints.WEST;
        top_pane.add( status_pane, con );

            JPanel cancel_pane = new JPanel();
            cancel_pane.setBorder( BorderFactory.createEmptyBorder(10,5,5,5) );
                cancel_btn = new JButton( "Cancel" );
                canceled   = false;
                cancel_btn.addActionListener( new ButtonListener() );
            cancel_pane.add( cancel_btn );

        con.gridx = 0; con.gridy = 4;
        con.anchor = GridBagConstraints.CENTER;
        top_pane.add( cancel_pane, con );

        setContentPane( top_pane );
    }

    public void setNote( String new_note )
    {
        status_area.append( new_note + newline );
        status_area.setCaretPosition( status_area.getDocument().getLength() );
    }

    public void setProgress( int cur_bar_idx )
    {
        progress_bar.setValue( cur_bar_idx );
    }
        
    public boolean isCanceled()
    {
        return ( canceled );
    }

    // the actionPerformed method in this class
    // is called when the user presses the Cancel button
    class ButtonListener implements ActionListener
    {
        public void actionPerformed( ActionEvent evt )
        {
            canceled = true;
            cancel_btn.setEnabled( false );
            close();
        }
    }

    public void close()
    {
        setVisible( false );
        this.dispose();
    }

    public void setMillisToDecideToPopup( int itime )
    {}

    public void setMillisToPopup( int itime )
    {}

    public static void main( String [] args )
    {
        ProgressDialog prog_dlg;
        long           start_time;
        long           elapsed_time;
        int index;

        prog_dlg = new ProgressDialog( null, "Test Progress", "Testing",
                                       0, 100 );

        start_time = ( new Date() ).getTime();
        for ( int ii = 0; ii < 100000; ii++ ) {
            try { 
                index = ii % 100;
                prog_dlg.setProgress( index );
                elapsed_time = ( new Date() ).getTime() - start_time;
                prog_dlg.setNote( "At elapsed_time = " + elapsed_time
                                + " msec,    index = " + index );
                Thread.sleep( 10 );
            }
            catch ( InterruptedException e ) {}
        }

        prog_dlg.addWindowListener( new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        });
    }
}
