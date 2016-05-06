import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;
import java.util.Vector;
import java.util.Enumeration;
import com.sun.java.swing.border.Border;

/**
 * This class creates the frame with buttons which stand for each
 * state type. The buttons are pressed to produce histogram frames
 */
public class StateButtons extends JToolBar
implements ActionListener, ItemListener {
  ClogDisplay parent;
  private Vector hists;
  private int numCols;
  private JCheckBox arrowsChkBox;
  
  public StateButtons (ClogDisplay p, int n) {
    super ();
    
    parent = p; numCols = n;
    hists = new Vector ();
    setupPanels ();
  }
  
  private void setupPanels () {
    Border border1, border2 = BorderFactory.createLoweredBevelBorder ();
    
    JPanel mainPanel = new JPanel (new GridBagLayout ());
    
    GridBagConstraints con = new GridBagConstraints (); con.fill = GridBagConstraints.HORIZONTAL; 
    con.weightx = 1; con.anchor = GridBagConstraints.NORTHWEST;
    
    //States
    JPanel statesP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (4, 4, 2, 4);
    statesP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    JPanel bP = new JPanel (new GridBagLayout ());
    
    int row = 0, col = 0;
    MyButton b;
    Enumeration enum = parent.stateDefs.elements ();
    
    while (enum.hasMoreElements ()) {
      CLOG_STATE s = (CLOG_STATE)enum.nextElement ();
      
      if (s.stateVector.size () > 0) {
	con.gridx = col; con.gridy = row;

	bP.add (b = new MyButton (s.description.desc, "Press for " + 
					   s.description.desc + " histogram",
					   this), con);
        b.setIcon (new ColoredRect (s.color)); b.setHorizontalAlignment (b.LEFT);
		   
	con.gridx = col + 1;
        bP.add (s.checkbox = new JCheckBox ("", true), con); 
	s.checkbox.addItemListener (this); 
	s.checkbox.setToolTipText ("Enable or disable " + s.description.desc);
	
	if ((col / 2) == numCols) {col = 0; row ++;}
	else col += 2;
      }
    }
    
    con.gridx = 0; con.gridy = 0; con.fill = GridBagConstraints.NONE; con.weightx = 0;
    statesP.add (bP, con);
    
    //Controls
    JPanel contP = new JPanel (new GridLayout (2, 1));
    contP.add (new MyButton ("All States On", "Enable all states", this)); 
    contP.add (new MyButton ("All States Off", "Disable all states", this));
    
    con.gridx = 1;
    statesP.add (contP, con);
    
    con.gridx = 0; con.gridy = 0;
    mainPanel.add (statesP, con);
    
    if (parent.arrowDef.stateVector.size () > 0) {
      //Arrows Panel
      JPanel arrowsP = new JPanel (new GridBagLayout ());
      border1 = BorderFactory.createEmptyBorder (2, 4, 4, 4);
      arrowsP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
      
      con.gridy = 0; con.gridx = 0;
      arrowsP.add (new MyButton ("Messages", "Press for Messages histogram", this));
      
      con.gridx = 1;
      arrowsP.add (arrowsChkBox = new JCheckBox ("", parent.canvas.arrowDispStatus), con); 
      arrowsChkBox.addItemListener (this); 
      arrowsChkBox.setToolTipText ("Enable or disable Messages");
      
      con.gridy = 2; con.gridx = 0;
      mainPanel.add (arrowsP, con);
    }
      
    //Add mainPanel to JScrollPane
    JScrollPane sP = new JScrollPane (mainPanel);
    sP.setPreferredSize (new Dimension (100, 100)); sP.setMinimumSize (new Dimension (100, 100));
    add (sP);
  }
  
  private void switchAll (boolean val) {
    Enumeration enum = parent.stateDefs.elements ();
    
    while (enum.hasMoreElements ()) {
      CLOG_STATE s = (CLOG_STATE)enum.nextElement ();
      if (s.stateVector.size () > 0) s.checkbox.setSelected (val);
    }
  }


  /**
   * causes the histogram frame to be displayed for the given state name
   */
  private void getStateHistogram (String name) {
    waitCursor ();
    parent.waitCursor ();
    parent.canvas.waitCursor ();
    parent.parent.waitCursor ();
    CLOG_STATE s = getState (name);
    if (s != null) hists.addElement (new Histwin (s, parent));
    parent.parent.normalCursor ();
    parent.canvas.normalCursor ();
    parent.normalCursor ();
    normalCursor ();
  }
  
  /**
   * This function returns the CLOG_STATE associated with the given name string.
   */
  private CLOG_STATE getState (String name) {
    Enumeration enum = parent.stateDefs.elements ();
    while (enum.hasMoreElements ()) {
      CLOG_STATE s = (CLOG_STATE)enum.nextElement ();
      if  (s.description.desc.compareTo (name) == 0) return s;
    }
    
    CLOG_STATE s = parent.arrowDef;
    if  (s.description.desc.compareTo ("Messages") == 0) {
      parent.canvas.calcArrowLens ();
      return s;
    }
    
    new ErrorDiag (this, "State " + name + " does not exist"); 
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
      CLOG_STATE s = (CLOG_STATE)enum.nextElement ();
      
      if (s.stateVector.size () > 0) {
	int strW = fm.stringWidth (s.description.desc);
	
	if ((xcord + rectW + hgap1 + strW) > (width + x)) {
          xcord = x; ycord += (charH + vgap);
        }
        
        g.setColor (s.color);
        g.fillRect (xcord, ycord, rectW, rectH);
        g.setColor (Color.black);
        g.drawRect (xcord, ycord, rectW - 1, rectH - 1);
        g.drawString (s.description.desc, xcord + rectW + hgap1, ycord + rectH - fm.getDescent () -
		      1);
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
  
  public void actionPerformed(ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (command.equals ("All States On")) {switchAll (true); parent.canvas.Refresh ();}
    else if (command.equals ("All States Off")) {switchAll (false); parent.canvas.Refresh ();}
    else getStateHistogram (command);
  }
  
  public void itemStateChanged (ItemEvent e) {
    Object source = e.getItem ();
    if (source == arrowsChkBox) parent.canvas.arrowDispStatus = arrowsChkBox.isSelected ();
    parent.canvas.Refresh ();
  }
  
  private void killHists () {
    if (hists.size () > 0) {
      Enumeration enum = hists.elements ();
      while (enum.hasMoreElements ()) {
	Histwin h = (Histwin)enum.nextElement ();
	if (h != null) {h.kill (); h = null;}
      }
      hists.removeAllElements ();
    }
  }
  
  void kill () {
    killHists (); removeAll ();
  }
}
