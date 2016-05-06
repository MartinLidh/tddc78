import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.Vector;
import java.util.Enumeration;
import javax.swing.border.Border;

/**
 * This class creates the frame with buttons which stand for each
 * state type. The buttons are pressed to produce histogram frames
 */
public class RecDefButtons extends JToolBar
implements ActionListener, ItemListener
{
  FrameDisplay parent;
  private Vector hists;
  private int numCols;
  private JCheckBox arrowsChkBox;
  
  public RecDefButtons( FrameDisplay p, int n )
  {
    super ();
    
    parent = p; numCols = n;
    hists = new Vector ();
    setupPanels ();
  }
  
  private void setupPanels()
  {
    Border   border1, border2;
    int      row, col;
    MyButton btn;


    border2 = BorderFactory.createLoweredBevelBorder();
    
    JPanel mainPanel = new JPanel( new GridBagLayout() );
    
    GridBagConstraints con = new GridBagConstraints();
    con.fill = GridBagConstraints.HORIZONTAL; 
    con.weightx = 1; con.anchor = GridBagConstraints.NORTHWEST;
    
    // Setup States Panels
    JPanel statesP = new JPanel( new GridBagLayout() );
    border1 = BorderFactory.createEmptyBorder (4, 4, 2, 4);
    statesP.setBorder( BorderFactory.createCompoundBorder(border1, border2) );
    
    JPanel statesP_in = new JPanel(new GridBagLayout() );
    
    row = 0; col = 0;
    Enumeration enum = parent.stateDefs.elements();
    
    while ( enum.hasMoreElements() ) {
        RecDef def = ( RecDef ) enum.nextElement();
      
        if ( def.stateVector.size() > 0 ) {
            con.gridx = col; con.gridy = row;

            btn = new MyButton( def.description,
                                "Press for " + def.description
                              + " histogram", this );
            statesP_in.add( btn, con);
            btn.setIcon( new ColoredRect( def.color ) );
            btn.setHorizontalAlignment( btn.LEFT );
		   
            con.gridx = col + 1;
            def.checkbox = new JCheckBox( "", true );
            statesP_in.add( def.checkbox, con ); 
	    def.checkbox.addItemListener( this ); 
            def.checkbox.setToolTipText( "Enable or disable "
                                       + def.description );
	
            if ( (col / 2) == numCols ) {
                col = 0; row ++; 
            }
            else
                col += 2;
        }
    }
    
    con.gridx = 0; con.gridy = 0;
    con.fill = GridBagConstraints.NONE;
    con.weightx = 0;
    statesP.add( statesP_in, con );
    
    // State Buttons Controls
    JPanel states_cntl = new JPanel( new GridLayout(2, 1) );
    states_cntl.add( new MyButton( "All States On",
                                   "Enable all states", this) ); 
    states_cntl.add( new MyButton( "All States Off",
                                   "Disable all states", this) );
    
    con.gridx = 1;
    statesP.add( states_cntl, con );
    
    con.gridx = 0; con.gridy = 0;
    mainPanel.add( statesP, con );
    
    // Setup Arrows Panels
    if ( parent.arrowDefs.size() > 0 ) {
        row = 0; col = 0;
        JPanel arrowsP = new JPanel( new GridBagLayout() );
        border1 = BorderFactory.createEmptyBorder (2, 4, 4, 4);
        arrowsP.setBorder( BorderFactory.createCompoundBorder( border1,
                                                               border2 ) );

        JPanel arrowsP_in = new JPanel( new GridBagLayout() );

        enum = parent.arrowDefs.elements();
        while ( enum.hasMoreElements() ) {
            RecDef def = ( RecDef ) enum.nextElement();
            if ( def.stateVector.size() > 0 ) {
                con.gridx = col; con.gridy = row;
                btn = new MyButton( def.description,
                                    "Press for " + def.description
                                  + " histogram", this );
                arrowsP_in.add( btn, con );
                btn.setIcon( new ColoredRect( def.color ) );
                btn.setHorizontalAlignment( btn.LEFT );

                con.gridx = col + 1;
                def.checkbox = new JCheckBox( "", true );
                arrowsP_in.add( def.checkbox, con ); 
                def.checkbox.addItemListener( this ); 
                def.checkbox.setToolTipText( "Enable or disable "
                                           + def.description );
                if ( (col / 2) == numCols ) {
                    col = 0; row ++; 
                }
                else
                    col += 2;
            }   //  Endof if ( def.stateVector.size() > 0 )
        }

        con.gridx = 0; con.gridy = 0;
        con.fill = GridBagConstraints.NONE;
        con.weightx = 0;
        arrowsP.add( arrowsP_in, con );
    
        // Arrow Buttons Controls
        JPanel arrows_cntl = new JPanel( new GridLayout(2, 1) );
        arrows_cntl.add( new MyButton( "All Arrows On",
                                       "Enable all arrows", this) ); 
        arrows_cntl.add( new MyButton( "All Arrows Off",
                                       "Disable all arrows", this) );
    
        con.gridx = 1;
        arrowsP.add( arrows_cntl, con );

        con.gridy = 2; con.gridx = 0;
        mainPanel.add( arrowsP, con );
    }   //  Endof  if ( parent.arrowDefs.size() > 0 )
      
    //Add mainPanel to JScrollPane
    JScrollPane sP = new JScrollPane (mainPanel);
    sP.setPreferredSize (new Dimension (100, 100));
    sP.setMinimumSize (new Dimension (100, 100));
    add (sP);
  }
  
  private void SetRecDefsEnabled( Vector rec_defs, boolean val )
  {
      Enumeration defs = rec_defs.elements();
      while ( defs.hasMoreElements() ) {
          RecDef def = (RecDef) defs.nextElement();
          if ( def.stateVector.size() > 0 )
              def.checkbox.setSelected( val );
      }
  }


  /**
   * causes the histogram frame to be displayed for the given state name
   */
  private void getStateHistogram( String name )
  {
      waitCursor();
      parent.waitCursor();
      parent.canvas.waitCursor();
      parent.init_win.waitCursor();
      RecDef def = getRecDef( name );
      if ( def != null ) 
          hists.addElement( new Histwin( def, parent ) );
      parent.init_win.normalCursor();
      parent.canvas.normalCursor();
      parent.normalCursor();
      normalCursor();
  }
  
  /**
   * This function returns the RecDef associated with the given name string.
   */
  private RecDef getRecDef( String name ) {
      RecDef      def;
      Enumeration defs;

      defs = parent.stateDefs.elements ();
      while ( defs.hasMoreElements() ) {
          def = (RecDef) defs.nextElement ();
          if ( def.description.compareTo( name ) == 0 )
              return def;
      }
    
      defs = parent.arrowDefs.elements ();
      while ( defs.hasMoreElements() ) {
          def = (RecDef) defs.nextElement ();
          if ( def.description.compareTo( name ) == 0 ) 
              return def;
      }
    
      new ErrorDiag (this, "Record Definition " + name + " does not exist"); 
      return null;
  }
  
  /**
   * The function prints out key to jumpshot data.
   */
  int print (Graphics g, int x, int y, int width, int height) {
    Font f = g.getFont ();
    FontMetrics fm = getToolkit ().getFontMetrics (f);
    
    int charW = fm.stringWidth (" "), charH = fm.getHeight ();
    int hgap1 = charW, hgap2 = 2 * charW, vgap = fm.getAscent ();
    int rectW = 30, rectH = charH; //Dimensions of state rectangles
    int xcord = x, ycord = y;
    
    Enumeration enum = parent.stateDefs.elements ();
    while (enum.hasMoreElements ()) {
      RecDef s = (RecDef)enum.nextElement ();
      
      if (s.stateVector.size () > 0) {
	int strW = fm.stringWidth (s.description);
	
	if ((xcord + rectW + hgap1 + strW) > (width + x)) {
          xcord = x; ycord += (charH + vgap);
        }
        
        g.setColor (s.color);
        g.fillRect (xcord, ycord, rectW, rectH);
        g.setColor (Color.black);
        g.drawRect (xcord, ycord, rectW - 1, rectH - 1);
        g.drawString( s.description,
                      xcord + rectW + hgap1,
                      ycord + rectH - fm.getDescent () - 1);
        xcord += (rectW + hgap1 + strW + hgap2);
      }
    }
    return (ycord - y + (2 * charH));
  }      

  /**
   * sets the cursor to the WAIT_CURSOR type
   */
  void waitCursor () {setCursor (new Cursor (Cursor.WAIT_CURSOR));} 
  
  /** 
   * sets the cursor to the cursor associated with this frame
   */
  void normalCursor () {setCursor (new Cursor (Cursor.DEFAULT_CURSOR));}
  
  public void actionPerformed( ActionEvent evt )
  {
      String command = evt.getActionCommand();
      if ( command.equals( "All States On" ) ) {
          SetRecDefsEnabled( parent.stateDefs, true );
          parent.canvas.Refresh();
      }
      else if ( command.equals( "All States Off" ) ) {
          SetRecDefsEnabled( parent.stateDefs, false );
          parent.canvas.Refresh();
      }
      else if ( command.equals( "All Arrows On" ) ) {
          SetRecDefsEnabled( parent.arrowDefs, true );
          parent.canvas.Refresh();
      }
      else if ( command.equals( "All Arrows Off" ) ) {
          SetRecDefsEnabled( parent.arrowDefs, false );
          parent.canvas.Refresh();
      }
      else
          getStateHistogram( command );
  }
  
  public void itemStateChanged( ItemEvent e )
  {
    parent.canvas.Refresh ();
  }
  
  private void killHists()
  {
      if ( hists.size() > 0 ) {
          Enumeration enum = hists.elements();
          while ( enum.hasMoreElements() ) {
              Histwin h = (Histwin) enum.nextElement();
	      if ( h != null ) {
                  h.kill (); h = null;
              }
          }
          hists.removeAllElements ();
      }
  }
  
  void kill()
  {
      killHists ();
      removeAll ();
  }
}
