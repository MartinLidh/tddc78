import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import com.sun.java.swing.*;

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
public class ProcDlg extends JFrame
implements AdjustmentListener, ActionListener, ItemListener {
  private ClogDisplay parent;
  
  private Font dlgFont;
  private Choice procChoice, dprocChoice;
  private JCheckBox displayChkBox;
  private MyButton directionButton;
  private MyTextField procField;
  private Scrollbar hbar;

  private JProcess currProc, selectedProc;
  private Vector procVector, dprocVector;

  private int tval, maxBackValue;
  private boolean direction, reflag;

  public ProcDlg (ClogDisplay p) {
    super ("Adjust Process");
    parent = p;
    setup ();
  }
  /**
   * setup methods------------------------------------------------------------------
   */
  private void setup () {
    adjustDlgStuff ();
    setupData ();
    setuphbar ();
    setupPanel ();
        
    pack ();
    setSize (getMinimumSize ()); 
    setResizable (false);
    
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
    setFont (dlgFont = parent.frameFont);
  }

  private void setupData () {
    reflag = true;
    procChoice = new Choice (); procChoice.setFont (dlgFont);
    dprocChoice = new Choice (); procChoice.setFont (dlgFont);
    procChoice.addItemListener (this); dprocChoice.addItemListener (this);
    procVector = parent.canvas.procVector;
    dprocVector = parent.canvas.dprocVector;
    for (int i = 0; i < procVector.size (); i++) 
      procChoice.addItem (Integer.toString (((JProcess)procVector.elementAt (i)).procId));
    for (int i = 0; i < dprocVector.size (); i++) 
      dprocChoice.addItem (Integer.toString (((JProcess)dprocVector.elementAt (i)).procId));
  }
  
  private void setuphbar () {
    hbar = new Scrollbar (Scrollbar.HORIZONTAL, 0, 0, 0, parent.canvas.widthCan);
    hbar.addAdjustmentListener (this);
    directionButton = new MyButton ("Forward", "Press for opposite direction", this); 
    direction = true;
  }
  
  private void setupPanel () {
    getContentPane ().setLayout (new GridBagLayout ());
    
    GridBagConstraints con = new GridBagConstraints ();
    con.fill = GridBagConstraints.BOTH;
    
    //Current Process
    MyJPanel p = new MyJPanel (new GridLayout (2,1), "Current Process");
    
    JPanel pl = new JPanel (new GridBagLayout ());
    
    pl.add (new JLabel ("Process #"), con);
        
    con.gridx = 1; 
    pl.add (procField = new MyTextField (3, false), con);
    procField.setToolTipText ("Current process");
    
    p.add (pl);
    
    pl = new JPanel (new GridBagLayout ());
    
    con.gridx = 0; 
    pl.add (directionButton, con);
    
    con.gridx = 1; con.gridwidth = 2; con.weightx = 1.0;
    pl.add (hbar, con);
    
    con.gridx = 3; con.gridwidth = 1; con.weightx = 0;
    pl.add (new MyButton ("Reset", "Place current process at original time position",
			 this), con);
    p.add (pl);
    
    con.gridx = 0;
    getContentPane ().add (p, con);
    
    //Available Process
    p = new MyJPanel (new GridBagLayout (), "Available Processes");
    
    con.gridx = 0;
    p.add (new JLabel ("Available"), con);
    
    con.gridx = 1;
    p.add (procChoice, con);
    
    con.gridx = 2;
    p.add (displayChkBox = new JCheckBox ("Display"), con);
    displayChkBox.addItemListener (this);
    displayChkBox.setToolTipText ("Enable or disable selected available process");
    
    con.gridx = 3;
    p.add (new MyButton ("Swap", this), con);

    con.gridx = 4;
    p.add (new MyButton ("Delete", this), con);
    
    con.gridx = 0; con.gridy = 1;
    getContentPane ().add (p, con);
    
    //Deleted Process
    p = new MyJPanel (new GridBagLayout (), "Deleted Processes");
    
    con.gridx = 0; con.gridy = 0;
    p.add (new JLabel ("Deleted"), con);
    
    con.gridx = 1;
    p.add (dprocChoice, con);
    
    con.gridx = 3;
    p.add (new MyButton ("Insert", this), con);
    
    con.gridx = 0; con.gridy = 2;
    getContentPane ().add (p, con);
    
    //Close
    con.gridy = 3; con.anchor = GridBagConstraints.EAST; 
    con.fill = GridBagConstraints.VERTICAL;
    getContentPane ().add (new MyButton ("Close", "Close window", this), con);
  }
  //end of setup methods-----------------------------------------------------------------

  // event handler methods---------------------------------------------------------------
  
  /**
   * Handles action events generated by buttons
   */
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (command.equals ("Insert")) {
      if (dprocVector.size () > 0) {
        String selected = dprocChoice.getSelectedItem ();
        int selectedId = (Integer.valueOf (selected)).intValue ();
        parent.canvas.InsertProc (parent.canvas.getIndex (dprocVector, selectedId),
                                  getSelectedProc ().procId);
        adjustChoices ();
        parent.canvas.Refresh ();
      }
    }
    else if (command.equals ("Swap")) {
      if (procVector.size () > 1) {
        int selectedId = (Integer.valueOf (procChoice.getSelectedItem ())).intValue ();
        
        if (currProc.procId != selectedProc.procId) {
          int selectedIndex = parent.canvas.getIndex (procVector, selectedProc.procId);
          int currIndex = parent.canvas.getIndex (procVector, currProc.procId);
          procVector.setElementAt (currProc, selectedIndex);
          procVector.setElementAt (selectedProc, currIndex);
	  parent.canvas.Refresh ();
	}
      }
    }
    else if (command.equals ("Delete")) {
      if (procVector.size () > 1) {
        String selected = procChoice.getSelectedItem ();
        int selectedId = (Integer.valueOf (selected)).intValue ();
        parent.canvas.DeleteProc (parent.canvas.getIndex (procVector, selectedId));
        adjustChoices ();
        parent.canvas.Refresh ();
	setVisible (false);
      }
    }
    else if (command.equals ("Close")) setVisible (false);
    else if (command.equals ("Forward")) {
      directionButton.setText ("Backward");
      maxBackValue = parent.canvas.getMaxDiff (currProc);
      if (maxBackValue > parent.canvas.widthCan) 
        maxBackValue = parent.canvas.widthCan;
      //Range of values [0 .. max]
      tval = 0;
      hbar.setMaximum (maxBackValue);
      hbar.setMinimum (0);
      hbar.setValue (maxBackValue);
      direction = false;
    }
    else if (command.equals ("Backward")) {
      directionButton.setText ("Forward");
      hbar.setValue (0); tval = 0;
      hbar.setMinimum (0);
      hbar.setMaximum (parent.canvas.widthCan);
      direction = true;
    }
    else if (command.equals ("Reset")) {
      parent.canvas.resetTimeLine (currProc); parent.canvas.Refresh ();
      hbar.setValue (0); tval = 0;
      hbar.setMaximum (parent.canvas.widthCan);
      directionButton.setText ("Forward"); direction = true;
    }
  }
  
  /**
   * Handles AdjustmentEvents generated by the scrollbar
   */
  public void adjustmentValueChanged (AdjustmentEvent e) {
    int val = hbar.getValue ();
    if (!direction) val = maxBackValue - val;
    parent.canvas.pushTimeLine (currProc, val - tval, direction); parent.canvas.Refresh ();
    tval = val;
  }
  
  /**
   * Handles ItemEvents generated by the checkbox and the choice lists
   */
  public void itemStateChanged (ItemEvent e) {
    Object o = e.getItemSelectable ();
    if (o == displayChkBox) {
      getSelectedProc ().dispStatus = displayChkBox.isSelected ();
      if (reflag) parent.canvas.Refresh (); else reflag = true;
    }
    else if (o == procChoice) {
      reflag = false; //since setSelected will fire off an ItemEvent
      displayChkBox.setSelected (getSelectedProc ().dispStatus);
    }
  }
  //end of event handler methods---------------------------------------------------------
  
  /**
   * This method is used to reset values of this object for the given process
   * It must be noted that this object is reused for all processes
   */
  void reset (JProcess p) {
    currProc = p;
    procField.setText (Integer.toString (p.procId));
    reflag = false; //since setSelected will fire off an ItemEvent
    displayChkBox.setSelected (getSelectedProc ().dispStatus);
    directionButton.setText ("Forward"); direction = true;
    hbar.setValue (0); tval = 0;
    hbar.setMaximum (parent.canvas.widthCan);
  }
  
  /**
   * This method returns a pointer to the JProcess selected in procChoice
   */
  private JProcess getSelectedProc () {
    String selected = procChoice.getSelectedItem ();
    int selectedId = (Integer.valueOf (selected)).intValue ();
    int selectedIndex = parent.canvas.getIndex (procVector, selectedId);
    selectedProc = (JProcess)(procVector.elementAt (selectedIndex));
    return selectedProc;
  }

  private void adjustChoices () {
    procChoice.removeAll ();
    dprocChoice.removeAll ();
    for (int i = 0; i < procVector.size (); i++) 
      procChoice.addItem 
        (Integer.toString (((JProcess)procVector.elementAt (i)).procId));
    for (int i = 0; i < dprocVector.size (); i++) 
      dprocChoice.addItem 
        (Integer.toString (((JProcess)dprocVector.elementAt (i)).procId));
    sort (procChoice); sort (dprocChoice);
  }
  
  //this method sorts the contents of the given choice
  private void sort (Choice ch) {
    int total = ch.getItemCount ();
    int x [] = new int [total];
    for (int i = 0; i < total; i ++)
      x [i] =   (Integer.valueOf (ch.getItem (i))).intValue ();
    
    for (int pass = 1; pass < total; pass++)
      for (int i = 0; i < total - 1; i++) {
        int curr = x [i];
        int next = x [i + 1];
        if (curr > next) {
          int temp = x [i];
          x [i] = x [i + 1];
          x [i + 1] = temp;
        }
      }
    ch.removeAll ();
    
    for (int i = 0; i < total; i++) 
      ch.addItem (Integer.toString (x [i]));
  }
  
  void makeUIChanges () {
    SwingUtilities.updateComponentTreeUI(this);
    setResizable (true); setSize (getMinimumSize ()); setResizable (false);
  }
}
