import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class RecordDialog extends JDialog
implements ActionListener
{
    public RecordDialog( Frame f, StateInterval state )
    {
        super( f, "Rectangle Info" );

        StateInfo info      = state.info;
        RecDef    statedef  = info.stateDef;
   
        Setup( statedef.classtype + " : \'" + statedef.description + "\'",
               info.begT, info.groupID, info.endT, info.groupID );
    }

    public RecordDialog( Frame f, ArrowInfo arrow )
    {
        super( f, "Arrow Info" );

        RecDef    arrowdef  = arrow.arrowDef;

        Setup( arrowdef.classtype + " : \'" + arrowdef.description + "\'",
               arrow.begT, arrow.begGroupID,
               arrow.endT, arrow.endGroupID );
    }

    private void Setup( String description_str,
                        double beg_time, final StateGroupLabel beg_groupID,
                        double end_time, final StateGroupLabel end_groupID )
    {
        GridBagConstraints con = new GridBagConstraints ();
        con.anchor = GridBagConstraints.SOUTHWEST;
        
        JButton b = new JButton();
        b.addActionListener( this );
        b.setLayout( new GridBagLayout() );
        b.setToolTipText( "Click once to close this dialog" );
        b.setCursor( new Cursor( Cursor.HAND_CURSOR ) );

        con.gridy = 0;
        b.add( new JLabel( description_str ), con );
        con.gridy++;
        b.add( new JLabel( "Starts at : task ID = " + beg_groupID + " &  "
                         + "time = " + beg_time + " sec." ), con );
        con.gridy++;
        b.add( new JLabel( "Ends   at : task ID = " + end_groupID + " &  "
                         + "time = " + end_time + " sec." ), con );

        getContentPane().setLayout( new BorderLayout() );
        getContentPane().add( b, BorderLayout.CENTER );

        addWindowListener (new WindowAdapter () {
            public void windowClosing( WindowEvent e )
            {
                dispose();
            }
        });

        pack();
        setSize( getMinimumSize() );
        setResizable( false );
    }

    public void actionPerformed (ActionEvent e)
    { 
        dispose();
    }
}
