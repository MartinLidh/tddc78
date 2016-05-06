import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

/**
 * BUG 1: 
 * When delete button is pressed values associated with current process 
 * need to be updated as the current process may be deleted. This 
 * however, does not take place. For now a temporary fix is available. 
 * Each time the user presses delete the window closes so next time 
 * when it reopens hbar values are updated. If there is time this will 
 * be fixed
 */

/**
 * This is the dialog used to manipulate process time lines. It is displayed
 * when the user clicks upon a process number drawn on VertScale
 */
public class StateGroupDialog extends JFrame
implements AdjustmentListener, ActionListener, ItemListener {
  private int           NdigitsDisplayed = 8;
  private ProgramCanvas canvas;
  private Font dlgFont;

  private JComboBox visible_grp_choices, deleted_grp_choices;
  private JCheckBox displayChkBox;
  private MyTextField procField;
  private JScrollBar hbar;

  private StateGroup currProc, selectedProc;
  private StateGroupListPair all_states;

  private int tval;
  private boolean reflag;

  public StateGroupDialog( ProgramCanvas in_canvas, Font in_Font ) {
    super( "Time Lines Manipulations" );
    canvas = in_canvas;
    dlgFont = in_Font;
    setup();
  }

  /**
   * setup methods-------------------------------------------------------------
   */
  private void setup () {
    adjustDlgStuff ();
    setupData ();
    setupPanel ();
        
    pack();
    setSize( getPreferredSize() );
    // setResizable( true );
    
    // Define, instantiate and register a WindowListener object.
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {setVisible (false);}
    }); 
    
    /*    addComponentListener (new ComponentAdapter () {
      public void componentResized (ComponentEvent e) {
	setResizable (true); setSize (getMinimumSize ()); setResizable (false);
      }
    });*/
  }
  
  private void adjustDlgStuff () {
    setFont( dlgFont );
  }

  private void setupData()
  {
    Dimension  choices_size = new Dimension( 100, 20 );
    reflag = true;
    visible_grp_choices = new JComboBox();
    // visible_grp_choices.setFont( dlgFont );
    // visible_grp_choices.setSize( choices_size );
    visible_grp_choices.addItemListener( this );
    deleted_grp_choices = new JComboBox();
    // deleted_grp_choices.setFont( dlgFont );
    // deleted_grp_choices.setSize( choices_size );
    deleted_grp_choices.addItemListener( this );

    all_states = canvas.all_states;
    StateGroupLabel the_groupID;
    Enumeration groups = all_states.visible.elements();
    while ( groups.hasMoreElements() ) {
        the_groupID = ( (StateGroup) groups.nextElement() ).groupID;
        visible_grp_choices.addItem( the_groupID.toString() );
    }
    groups = all_states.deleted.elements();
    while ( groups.hasMoreElements() ) {
        the_groupID = ( (StateGroup) groups.nextElement() ).groupID;
        deleted_grp_choices.addItem( the_groupID.toString() );
    }
  }
  
  private void setupPanel ()
  {
    getContentPane().setLayout( new GridBagLayout() );
    
    GridBagConstraints con = new GridBagConstraints();
    con.fill = GridBagConstraints.BOTH;
    
    //Selected State Group
    MyJPanel p = new MyJPanel( new GridLayout(2,1), "Selected Time Line" );
    
        JPanel pl = new JPanel( new GridBagLayout() );
        pl.add( new JLabel("Time Line ID # "), con );
        
        con.gridx = 1; 
        procField = new MyTextField( NdigitsDisplayed, false );
        procField.setHorizontalAlignment( JTextField.CENTER );
        procField.setToolTipText( "Selected Time Line ID" );
        pl.add( procField, con );
    
    p.add( pl );
    
        MyJPanel pt = new MyJPanel( new GridBagLayout(), 
                                    "Time Line Adjustment" );
    
            hbar = new JScrollBar( JScrollBar.HORIZONTAL,
                                   0, canvas.widthCan/10,
                                   -canvas.widthCan, canvas.widthCan );
            hbar.addAdjustmentListener( this );
            hbar.setUnitIncrement( 1 );
            debug.println( "hbar.getMinimum() = " + hbar.getMinimum() );
            debug.println( "hbar.getMaximum() = " + hbar.getMaximum() );

        con.gridx = 0; con.gridwidth = 3; con.weightx = 1.0;
        pt.add( hbar, con );
    
        con.gridx = 3; con.gridwidth = 1; con.weightx = 0;
        pt.add( new MyButton( "Reset", "Restore selected state group to its "
                                     + "original position on the time line",
                               this ),
                con );

    p.add( pt );
    
    con.gridx = 0;
    getContentPane().add( p, con );
    
    //Visible State Groups
    p = new MyJPanel( new GridBagLayout(), "Visible Time Lines" );
    
        con.gridx = 0; con.gridy = 0; con.gridwidth = 1;
        p.add( new JLabel( "Available : " ), con );
    
        con.gridx = 1; con.gridy = 0; con.gridwidth = 2;
        p.add( visible_grp_choices, con );
    
        con.gridx = 3; con.gridwidth = 1;
        p.add( new JLabel( "             " ), con );
    
        con.gridx = 4; con.gridy = 0; con.gridwidth = 1;
        p.add( displayChkBox = new JCheckBox( "Visible" ), con );
        displayChkBox.addItemListener( this );
        displayChkBox.setToolTipText( "Enable or disable the state group " 
                                    + "at the top of the visible list" );
    
        con.gridx = 5; con.gridy = 0; con.gridwidth = 1;
        p.add( new MyButton( "Swap", "Swap the state group at the top of the "
                                   + "visible list with the selected state "
                                   + "group", this ),
              con );

        con.gridx = 6; con.gridy = 0; con.gridwidth = 1;
        p.add( new MyButton( "Delete", "Delete the state group at the top "
                                     + "of the visible list", this ),
               con );
    
    con.gridx = 0; con.gridy = 1;
    getContentPane().add( p, con );
    
    //Deleted State Group
    p = new MyJPanel( new GridBagLayout(), "Deleted Time Lines" );
    
        con.gridx = 0; con.gridy = 0; 
        p.add( new JLabel( "Deleted : " ), con );
    
        con.gridx = 1; con.gridwidth = 2; 
        p.add( deleted_grp_choices, con );

        con.gridx = 3; con.gridwidth = 1;
        p.add( new JLabel( "             " ), con );
    
        con.gridx = 4; con.gridwidth = 1;
        p.add( new MyButton( "Restore", "Restore the deleted state group back "
                                      + "to the visible list", this ),
               con );
    
    con.gridx = 0; con.gridy = 2; con.anchor = GridBagConstraints.CENTER;
    getContentPane().add( p, con );
    
    //Close
    con.gridy = 3; con.anchor = GridBagConstraints.EAST; 
    con.fill = GridBagConstraints.VERTICAL;
    getContentPane().add( new MyButton( "Close", "Close this dialog box",
                                                  this ),
                          con );
  }
  // end of setup methods-------------------------------------------------------

  // event handler methods------------------------------------------------------
  
  /**
   * Handles action events generated by buttons
   */
  public void actionPerformed( ActionEvent evt )
  {
    String command = evt.getActionCommand ();

    if ( command.equals( "Restore" ) ) {
      if ( all_states.deleted.size() > 0 ) {
        String selected = (String) deleted_grp_choices.getSelectedItem();
        StateGroupLabel selectedID = new StateGroupLabel( selected );
        all_states.Deleted2Visible( selectedID, getSelectedProc().groupID );
        canvas.UpdateYDensity();
        adjustChoices();
        canvas.Refresh();
      }
    }
    else if ( command.equals( "Swap" ) ) {
      if ( all_states.visible.size() > 1 ) {
        String selected = (String) visible_grp_choices.getSelectedItem();
        StateGroupLabel selectedID = new StateGroupLabel( selected );
        if ( ! currProc.groupID.IsEqualTo( selectedProc.groupID ) ) {
          StateGroupList visible_groups = all_states.visible;
          int selectedIndex
              = visible_groups.GetSeqIndexAt( selectedProc.groupID );
          int currIndex
              = visible_groups.GetSeqIndexAt( currProc.groupID );
          visible_groups.setElementAt( currProc, selectedIndex );
          visible_groups.setElementAt( selectedProc, currIndex );
	  canvas.Refresh();
	}
      }
    }
    else if ( command.equals( "Delete") ) {
      if ( all_states.visible.size() > 1 ) {
        String selected = (String) visible_grp_choices.getSelectedItem ();
        StateGroupLabel selectedID = new StateGroupLabel( selected );
        all_states.Visible2Deleted( selectedID );
        canvas.UpdateYDensity();
        adjustChoices();
        canvas.Refresh();
	setVisible( false );
      }
    }
    else if ( command.equals( "Close" ) )
        setVisible (false);
    else if ( command.equals( "Reset" ) ) {
      hbar.setValue (0); tval = 0;
      hbar.setMinimum( - canvas.widthCan );
      hbar.setMaximum(   canvas.widthCan );
    }
  }
  
  /**
   * Handles AdjustmentEvents generated by the scrollbar
   */
  public void adjustmentValueChanged( AdjustmentEvent e )
  {
    debug.println( "StateGroupDialog.adjustmentValueChanged : " );
    int val = hbar.getValue ();
    double adj_time = canvas.getTime( val - tval );

    debug.println( "\t" + "adj_time = " + adj_time );
    currProc.PushTimeLine( adj_time );
    canvas.pushArrows( currProc, adj_time );
    canvas.adjustPosition();
    canvas.Refresh();
    tval = val;
  }
  
  /**
   * Handles ItemEvents generated by the checkbox and the choice lists
   */
  public void itemStateChanged( ItemEvent e ) {
    Object o = e.getItemSelectable();
    if ( o == displayChkBox ) {
      getSelectedProc().dispStatus = displayChkBox.isSelected();
      if (reflag)
          canvas.Refresh ();
      else
          reflag = true;
    }
    else if ( o == visible_grp_choices ) {
      reflag = false; //since setSelected will fire off an ItemEvent
      try {
         displayChkBox.setSelected( getSelectedProc().dispStatus);
      } catch ( NullPointerException err ) {}
    }
  }
  //end of event handler methods---------------------------------------------------------
  
  /**
   * This method is used to reset values of this object for the given process
   * It must be noted that this object is reused for all processes
   */
  void reset( StateGroup p )
  {
    currProc = p;
    procField.setText( p.groupID.toString() );
    reflag = false; //since setSelected will fire off an ItemEvent
    displayChkBox.setSelected (getSelectedProc ().dispStatus);
    hbar.setValue(0); tval = 0;
    hbar.setMaximum(   canvas.widthCan );
    hbar.setMinimum( - canvas.widthCan );
  }
  
  /**
   * This method returns a pointer to the StateGroup selected in 
   * visible_grp_choices
   */
  private StateGroup getSelectedProc()
  {
    String selected = (String) visible_grp_choices.getSelectedItem();
    StateGroupLabel selectedID = new StateGroupLabel( selected );
    int selectedIndex = all_states.visible.GetSeqIndexAt( selectedID );
    selectedProc = (StateGroup) all_states.visible.elementAt( selectedIndex );
    return selectedProc;
  }

  private void adjustChoices () {
    StateGroupLabel groupID;
    Enumeration groups;

    visible_grp_choices.removeAllItems();
    deleted_grp_choices.removeAllItems();

    groups = all_states.visible.elements();
    while ( groups.hasMoreElements() ) {
        groupID = ( (StateGroup) groups.nextElement() ).groupID;
        visible_grp_choices.addItem( groupID.toString() );
    }
    groups = all_states.deleted.elements();
    while ( groups.hasMoreElements() ) {
        groupID = ( (StateGroup) groups.nextElement() ).groupID;
        deleted_grp_choices.addItem( groupID.toString() );
    }
    sort( visible_grp_choices );
    sort( deleted_grp_choices );
  }
  
  //this method sorts the contents of the given choice
    private void sort( JComboBox ch )
    {
        int total           = ch.getItemCount();
        StateGroupLabel x[] = new StateGroupLabel[ total ];
        for ( int i = 0; i < total; i++ )
            x [i] =  new StateGroupLabel( (String) ch.getItemAt(i) ) ;
    
        StateGroupLabel curr, next, temp;

        for ( int pass = 1; pass < total; pass++ )
            for ( int i = 0; i < total - 1; i++ ) {
                curr = x[ i ];
                next = x[ i + 1 ];
                if ( curr.IsGreaterThan( next ) ) {
                    temp       = x[ i ];
                    x[ i ]     = x[ i + 1 ];
                    x[ i + 1 ] = temp;
            }
        }

        ch.removeAllItems();
        for ( int i = 0; i < total; i++ ) 
            ch.addItem( x[i].toString() );
    }
  
  void makeUIChanges () {
    SwingUtilities.updateComponentTreeUI(this);
    setResizable (true); setSize (getMinimumSize ()); setResizable (false);
  }
}
