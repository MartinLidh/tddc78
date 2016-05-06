import java.awt.*;
import java.util.Vector;
import java.awt.event.*;
import com.sun.java.swing.*;
import com.sun.java.swing.event.*;

//This dialog displays options
public class CanOptions extends JFrame
implements ActionListener, ItemListener {
  ClogDisplay parent;
  
  private JCheckBox elTLineChkBox, zoomLkLineChkBox;
  private MyTextField nFacField, zFacField, fromField, toField;
  private JTabbedPane tabbedP;
  private JComboBox dChoiceBox, dSelectedBox;

  
  public CanOptions (ClogDisplay p) {
    super ("Options");
    parent = p;
    setup ();
  }
  
  //setup methods--------------------------------------------------------
  private void setup () {
    setupPanels ();
    setupEventHandlers ();
    pack ();
    makeSize ();
  }
  
  private void setupPanels () {
    getContentPane ().setLayout (new GridBagLayout ());
    
    GridBagConstraints con = new GridBagConstraints (); con.anchor = GridBagConstraints.SOUTHWEST;
    
    tabbedP = new JTabbedPane ();
    
    //Zoom and time options
    JPanel zoomP = new JPanel (new GridBagLayout ());
        
    con.gridx = 0;
    zoomP.add (new JLabel ("Zoom Factor "), con);
    
    con.gridx = 1;
    zoomP.add (zFacField = new MyTextField ("", 5, true), con);
    zFacField.setToolTipText ("This value is multiplied or divided " +
			      "with length of display during zooming");
    
    con.gridy = 1; con.gridx = 0; con.gridwidth = GridBagConstraints.REMAINDER;
    zoomP.add (new JLabel ("Starting and ending times in viewport"), con);
    
    con.gridy = 2; con.gridx = 0; con.gridwidth = 1;
    zoomP.add (new JLabel ("From "), con);
    
    con.gridx = 1;
    zoomP.add (fromField = new MyTextField ("", 10, true), con);
    fromField.setToolTipText ("starting time");
    
    con.gridx = 2;
    zoomP.add (new JLabel ("sec"), con);
    
    con.gridy = 3; con.gridx = 0; con.gridwidth = 1;
    zoomP.add (new JLabel ("To "), con);
    
    con.gridx = 1;
    zoomP.add (toField = new MyTextField ("", 10, true), con);
    toField.setToolTipText ("ending time");
    
    con.gridx = 2;
    zoomP.add (new JLabel ("sec"), con);
    
    tabbedP.add ("Zoom & Time", zoomP);
    
    //Nest Options
    JPanel nestP = new JPanel (new GridBagLayout ());
    
    con.gridy = 0; con.gridx = 0;
    nestP.add (new JLabel ("Nest Factor "), con);
    
    con.gridx = 1;
    nestP.add (nFacField = new MyTextField ("", 10, true), con);
    nFacField.setToolTipText ("This value defines difference in height between successive" + 
			      " nested levels");
    
    con.gridy = 1; con.gridx = 0; con.gridwidth = GridBagConstraints.REMAINDER;
    nestP.add (new MyButton ("Do optimal nesting", "Do nesting using optimal nest factor",
			     this), con);
    
    tabbedP.add ("Nesting", nestP);
    
    //Line Display
    JPanel lineP = new JPanel (new GridBagLayout ());
    
    con.gridy = 0; con.gridx = 0; con.gridwidth = 1;
    lineP.add (elTLineChkBox = new JCheckBox ("Elapsed Time Line Display"), 
	       con);
    elTLineChkBox.addItemListener (this);
    elTLineChkBox.setToolTipText ("Enable or disable display of elapsed time line");
    
    con.gridy = 1;
    lineP.add (new MyButton ("Reset Elapsed Time", 
					 "Reset elapsed time line to 0", 
					 this), con);
    
    con.gridy = 2; con.gridx = 0;
    lineP.add (zoomLkLineChkBox = new JCheckBox ("Zoom Lock Line Display"), 
	       con);
    zoomLkLineChkBox.addItemListener (this);
    zoomLkLineChkBox.setToolTipText ("Enable or disable display of zoom lock line");
    
    tabbedP.add ("Line Display", lineP);
    
    //Display Options
    JPanel dispP = new JPanel (new GridBagLayout ());
    
    MyJPanel p = new MyJPanel (new GridBagLayout (), "Display Choices");
    
    dChoiceBox = new JComboBox (CONST.DSTR);
    dChoiceBox.setSelectedIndex (0);
    
    con.gridy = 0; con.gridx = 0; con.anchor = GridBagConstraints.NORTHWEST; 
    p.add (dChoiceBox, con);
    
    con.gridx = 1;
    p.add (new MyButton ("Add", "Add selected Display Item", this), con);
    
    con.gridy = 0; con.gridx = 0;
    dispP.add (p, con);
    
    p = new MyJPanel (new GridBagLayout (), "Selected Displays");
    
    dSelectedBox = new JComboBox ();
    
    con.gridy = 0; con.gridx = 0;
    p.add (dSelectedBox, con);
    
    con.gridx = 1;
    p.add (new MyButton ("Cut", "Cut selected Display Item", this), con);
    
    con.gridy = 1; con.gridx = 0;
    dispP.add (p, con);
    
    tabbedP.add ("Display Options", dispP);
    
    con.gridy = 0; con.gridx = 0; con.fill = GridBagConstraints.BOTH; 
    con.weightx = con.weighty = 1;
    getContentPane ().add (tabbedP, con);
    
    con.gridy = 1; con.anchor = GridBagConstraints.SOUTHEAST; con.fill = GridBagConstraints.NONE;
    con.weightx = con.weighty = 0;
    getContentPane ().add (new MyButton ("Close", "Close window", this), con);
  }
  
  private void setupEventHandlers () {
    zFacField.addActionListener (this); zFacField.setActionCommand ("Change zFac");
    fromField.addActionListener (this); fromField.setActionCommand ("Change from");
    toField.addActionListener (this); toField.setActionCommand ("Change to");
    nFacField.addActionListener (this); nFacField.setActionCommand ("Change nFac");
  
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {setVisible (false);}
    });  
  }
  //end of setup methods----------------------------------------------------------
  
  //event handler methods--------------------------------------------------------
  //events generated by buttons and checkboxes are processed
  /**
   * event handler method for ActionEvents generated by buttons
   */
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (command.equals ("Close")) setVisible (false);
    else if (command.equals ("Reset Elapsed Time")) 
      parent.canvas.fixElTimePointer (0);
    else if (command.equals ("Change nFac")) {
      double nFac;
      try {nFac = (new Double (nFacField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required");
	return;
      }
      parent.canvas.changeNestFactor (nFac);
    }
    else if (command.equals ("Do optimal nesting"))
      nFacField.setText ((new Float (parent.canvas.doAppropNesting ())).toString ());
    else if (command.equals ("Change zFac")) {
      double zFac;
      try {zFac = (new Double (zFacField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required");
	return;
      }
      parent.canvas.changeZoomFactor (zFac);
    }
    else if (command.equals ("Change from")) {
      double f;
      try {f = (new Double (fromField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required");
	return;
      }
      parent.canvas.changeFromTime (f);
    }
    else if (command.equals ("Change to")) {
      double t;
      try {t = (new Double (toField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required");
	return;
      }
      parent.canvas.changeToTime (t);
    }
    else if (command.equals ("Add")) {
      int i = dChoiceBox.getSelectedIndex ();
      parent.canvas.addDisplay (i);
      dSelectedBox.addItem (CONST.DSTR [i]);
    }
    else if (command.equals ("Cut")) {
      if (dSelectedBox.getItemCount () < 2) return;
      int i = dSelectedBox.getSelectedIndex ();
      parent.canvas.removeDisplay (i);
      dSelectedBox.removeItemAt (i);
      dSelectedBox.setSelectedIndex (0);
    }
  }
  
  /**
   * event handler method for ItemEvents generated by the checkboxes
   */
  public void itemStateChanged (ItemEvent e) {
    Object o = e.getItemSelectable ();

    if (o == elTLineChkBox) parent.canvas.updateElTLineDStat (elTLineChkBox.isSelected ());
    else parent.canvas.updateZLkLineDStat (zoomLkLineChkBox.isSelected ());
  }
  
  //end of event handler methods------------------------------------------------------
  //Method used by ClogDisplay to reset values into this object
  public void reset () {
    elTLineChkBox.setSelected (parent.canvas.getZLkLineDStat());
    zoomLkLineChkBox.setSelected (parent.canvas.getElTLineDStat ());
    nFacField.setText ((new Float (parent.canvas.nestFactor)).toString ());
    zFacField.setText ((new Float (parent.canvas.zF)).toString ());
    if (parent.canvas.maxLevel == 0) tabbedP.setEnabledAt (tabbedP.indexOfTab ("Nesting"), false);
    makeSize ();
  }
  
  void addDisplay (int i) {dSelectedBox.addItem (CONST.DSTR [i]);}
  
  void adjustTimes (double b, double e) {
    fromField.setText ((new Float (b)).toString ());
    toField.setText ((new Float (e)).toString ());
  }
  
  private void makeSize () {
    setResizable (true);
    Dimension d = getMinimumSize (); d.height += 40; d.width += 40;
    setSize (d);
    //setResizable (false);
  }
  
  void makeUIChanges () {
    SwingUtilities.updateComponentTreeUI(this);
    makeSize ();
  }
}
