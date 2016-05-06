import java.awt.*;
import java.awt.event.*;
import java.util.*;
import com.sun.java.swing.*;
import com.sun.java.swing.border.Border;

/*
  This class supervises all display operations and printing for the given
  Jumpshot data. 
  
  It shows a frame consisting of buttons (Options), (Print), (Refresh), 
  (Close), (In), (Out), etc. and textFields including time cursor and
  elapsed time. It also contains the canvas showing the different states
  
  Once a clog file is read by ClogReader, the data structures are passed
  onto this class which then produces the display.
*/
public class ClogDisplay extends JFrame 
implements AdjustmentListener, ActionListener {
  public Mainwin parent;
 
  //Data Structures
  Vector mtns;
  Vector stateDefs;      //List of state definitions
  Vector data;           //List of all states of all process's
  CLOG_ARROW quiver;     //Object containing list of all arrows
  CLOG_STATE arrowDef;
  
  Properties printPrefs;        //stores the default settings from printing 
  
  //Dialogs and frames
  //  PrintDlg printDlg;            //dialog containing print options 
  StateButtons stateButtons;    //frame containing state buttons
  CanOptions optionsDlg;        //dialog containing options
  ProcDlg procDlg;       //dialog containing process options

  //canvas's
  ProgramCanvas canvas;  //canvas where states are drawn
  VertScaleCanvas vcanvas1, vcanvas2; //canvas's where process numbers are drawn
  
  JViewport vport;              //object that controls view into the much larger JComponent
  JScrollBar hbar;              //scrollbar that controls horizontal scrolling
  private MyTextField timeField, elTimeField;
  
  Font frameFont, printFont;
  Color frameBColor, frameFColor, printBColor, printFColor;
  Color printImgBColor, normImgBColor, rulerColor;

  boolean setupComplete = false;

  int dtype = CONST.TIMELINES, vertRulerGap;
  
  /**
   * constructor
   */
  public ClogDisplay (Mainwin p) {
    super ();
    
    parent = p;
    dtype = parent.dtype;
    setTitle (CONST.DSTR [dtype]);
     
    quiver = p.mainTool.a;
    mtns = p.mainTool.mtns;
    data = p.mainTool.data;
    stateDefs = p.mainTool.stateDefs;
    setupArrowDef ();
    setup ();
  }
  
  private void setupArrowDef () {
    arrowDef = new CLOG_STATE ();
    arrowDef.description.desc = new String ("Messages");
    arrowDef.color  = Color.white;
    arrowDef.stateVector = quiver.arrowVector;
  }
  
  //Setup methods-------------------------------------------------------------------------
  private void setup () {
    adjustFrameStuff ();
    setupCanvas ();
    setupPanels ();
    setupDlgs ();
    setupPrintPrefs ();
    setSize (parent.dimPG);
    setVisible (true);
    
    //Before we draw the states on to the image, which is on the canvas the
    //frame needs to be made visible so that we get a graphics context to
    //draw upon. If the frame was not made visible a null graphics context
    //was returned
    waitCursor ();  
    drawData ();
    setupEventHandlers ();
    setupComplete = true;
    normalCursor ();
    parent.normalCursor ();
  }
  
  private void adjustFrameStuff () {
    frameBColor = parent.frameBColor;
    frameFColor = parent.frameFColor;
    frameFont = parent.frameFont;
    rulerColor = getBackground ();
    printBColor = new Color (35, 129, 174);            //A lighter shade of blue
    printFColor = Color.white; 
    printImgBColor = Color.lightGray;
    normImgBColor = Color.black;
    printFont = new Font ("Serif", Font.BOLD, 14);
  }
  
  /**
   * Initialize canvas's. canvas has to be non-null to be placed in a 
   * panel
   */
  private void setupCanvas () {
    canvas = new ProgramCanvas ();
    vcanvas1 = new VertScaleCanvas (this);
    vcanvas2 = new VertScaleCanvas (this);
  }
  
  private void setupPanels () {
    getContentPane ().setLayout (new BorderLayout ());
    
    JPanel mainPanel = new JPanel (new BorderLayout ());
    
    GridBagConstraints con = new GridBagConstraints (); con.anchor = GridBagConstraints.SOUTHWEST;
    con.weightx = 1; con.fill = GridBagConstraints.HORIZONTAL;
    
    Border border1, border2 = BorderFactory.createLoweredBevelBorder ();
    
    JPanel northP = new JPanel (new GridBagLayout ());
    
    JPanel textFieldP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (4, 4, 2, 2);
    textFieldP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    con.gridy = 0; con.gridx = 0; 
    textFieldP.add (new JLabel ("Pointer "), con);
    
    con.gridx = 1; 
    textFieldP.add (timeField = new MyTextField ("", 10, false), con); 
    timeField.setToolTipText ("Cursor position (sec) in display");
    
    con.gridy = 1; con.gridx = 0;
    textFieldP.add (new JLabel ("Elapsed Time "), con);
    
    con.gridx = 1;
    textFieldP.add (elTimeField = new MyTextField ("", 10, false), con);
    elTimeField.setToolTipText ("Cursor position (sec) in display wrt. elapsed time line");
    
    con.gridy = 0; con.gridx = 0;
    northP.add (textFieldP, con);
    
    JPanel zoomP = new JPanel (new GridLayout (1, 3));
    border1 = BorderFactory.createEmptyBorder (4, 2, 2, 2);
    zoomP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    zoomP.add (new MyButton ("In", "Zoom In Horizontally", this));
    zoomP.add (new MyButton ("Out", "Zoom Out Horizontally", this));
    zoomP.add (new MyButton ("Reset", "Redraws entire data in viewport", this));
    
    con.gridy = 0; con.gridx = 1;
    northP.add (zoomP, con);
    
    JPanel oP = new JPanel (new GridLayout (1, 3));
    border1 = BorderFactory.createEmptyBorder (4, 2, 2, 4);
    oP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    oP.add (new MyButton ("Options", "Display options for arrows, zoom lock line, " + 
			  "elapsed time line and nesting", this));
    MyButton button = new MyButton ("Print", "Print display to file or printer", this);
    //    if (parent.aplt) button.setEnabled (false);
    oP.add (button);
    oP.add (new MyButton ("Close", "Close window", this));
    
    con.gridy = 0; con.gridx = 2;
    northP.add (oP, con);
    
    mainPanel.add (northP, BorderLayout.NORTH);
    
    JPanel canPanel = new JPanel (new BorderLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 4, 4);
    canPanel.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    JPanel p = new JPanel (new BorderLayout ());
    
    vport = new JViewport (); vport.setView (canvas); vport.addComponentListener (canvas);
    
    //Bug: JViewport does not support setBorder so we put JViewport in a Panel with a border
    JPanel vpp = new JPanel (new BorderLayout ()); 
    vpp.setBorder (BorderFactory.createCompoundBorder (BorderFactory.createRaisedBevelBorder (),
						       BorderFactory.createLoweredBevelBorder ()));
    vpp.add (vport, BorderLayout.CENTER);
    

    
    p.add (vpp, BorderLayout.CENTER);

    p.add (hbar = new JScrollBar (JScrollBar.HORIZONTAL), BorderLayout.SOUTH);
    canPanel.add (p, BorderLayout.CENTER);
    
    vertRulerGap = (vpp.getInsets ()).top + (p.getInsets ()).top;
    vcanvas1.setVerticalGap (vertRulerGap); vcanvas2.setVerticalGap (vertRulerGap);
    
    canPanel.add (vcanvas1, BorderLayout.EAST);
    canPanel.add (vcanvas2, BorderLayout.WEST);
    mainPanel.add (canPanel, BorderLayout.CENTER);
    
    getContentPane ().add (mainPanel, BorderLayout.CENTER);
  }

  private void setupEventHandlers () {
    hbar.addAdjustmentListener (this);
    
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {kill ();}
    });
  }
  
  private void setupDlgs () {
    optionsDlg = new CanOptions (this);
    //    printDlg = new PrintDlg (this);
    stateButtons = new StateButtons (this, 4);
    getContentPane ().add (stateButtons, BorderLayout.SOUTH);
  }   
  
  /**
   * printPrefs is the property set that stores the default settings for
   * printing.
   */
  private void setupPrintPrefs () {
    printPrefs = new Properties ();
    printPrefs.put ("awt.print.fileName", "jumpshot.ps");
    printPrefs.put ("awt.print.numCopies", "1");
    printPrefs.put ("awt.print.paperSize", "letter");
    printPrefs.put ("awt.print.destination", "file");
    printPrefs.put ("awt.print.orientation", "landscape");
  }
  
  //End of Setup methods-------------------------------------------------------------------
  
  //Event Handler methods------------------------------------------------------------------
  /**
   * Handles events when scrollbar is modified
   */
  public void adjustmentValueChanged (AdjustmentEvent e) {setPosition (hbar.getValue ());}
  
  void setPosition (int x) {
    canvas.adjustImgH (x); 
    vport.setViewPosition (new Point (canvas.getPanePosX (), 0));
  }
  
  //Handler the event when one of the buttons is pressed 
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (setupComplete) {
      if (command.equals ("In")) zoomInH ();
      else if (command.equals ("Out")) zoomOutH ();
      else if (command.equals ("Reset")) resetView ();
      else if (command.equals ("Options")) {
        optionsDlg.reset ();
        optionsDlg.show (); 
	optionsDlg.toFront ();
      }
      else if (command.equals ("Print")) Print ();
      else if (command.equals ("Close")) kill ();
    }
  }

  //End of event Handler methods--------------------------------------------
  
  /**
   * draw states on to canvas
   */
  private void drawData () {
    canvas.init (this);
    vcanvas1.setupComplete (); vcanvas2.setupComplete ();
    procDlg = new ProcDlg (this);
  }  
  
  //Zoom methods------------------------------------------------------------
  /**
   * zoom in horizontally using the current zoom factor along zoomlock if any.
   */
  private void zoomInH () {
    waitCursor ();
    canvas.zoomInH ();
    zoomH ();
    normalCursor ();
  }
  
  /**
   * zoom out horizontally using the current zoom factor along zoomlock if any.
   */
  private void zoomOutH () {
    waitCursor ();
    canvas.zoomOutH ();
    zoomH ();
    normalCursor ();
  }   

  /**
   * Reset view so that the entire data occupies one screen
   */
  private void resetView () {
    waitCursor ();
    canvas.resetView ();
    zoomH (); 
    normalCursor ();
  }
  
  /**
   * Method called whenever a horizontal zoom takes place
   */
  void zoomH () {
    setHoriz ();
    hbar.setValue (canvas.sbPos);
    vport.setViewPosition (new Point (canvas.getPanePosX (), 0));
  }
  
  /**
   * Reset scrollbar values when horizontal extent of data changes (e.g during zoom)
   * BUG: Id 4138681 - ScrollBar doesn't update visible amount when not at top (or left) of bar
   * http://developer.java.sun.com/developer/bugParade/bugs/4138681.html
   * They say that it is fixed in JDK1.2beta4 but, since we are not yet using JDK1.2beta4 or later
   * we will use the simple workaround
   */
  void setHoriz () {
    hbar.setMaximum (canvas.maxH);
    hbar.setMinimum (0);
        
    //Work around for the Bug Id: 4149649 /**/
    /**/int x = hbar.getValue (); hbar.setValue (0);
        
    hbar.setVisibleAmount ((int)Math.rint (canvas._xPix / 3.0));
    
    /**/hbar.setValue (x);
    
    hbar.setUnitIncrement (1);
    hbar.setBlockIncrement ((int)Math.rint (canvas._xPix / 3.0));
  }
  //--------------------------------------------------------------------------
  
  /**
   * modify cursor value in the time field
   */
  void adjustTimeField (double d) {
    timeField.setText ((new Float (d)).toString ());
  }
  
  /**
   * Modify elapsed time field value
   */
  void adjustElTimeField (double d) {
    elTimeField.setText ((new Float (d)).toString ());
  }
   
  /**
   * Prints out jumpshot data
   */
  private void Print () {
    waitCursor ();
    
    if (checkPrintAllowed ()) {
      String jobTitle = "Jumpshot:" + parent.logFileName;
    
      //Obtain a PrintJob Object. This posts a Print dialog
      //printprefs stores user printing prefrences
      PrintJob pjob = getToolkit ().getPrintJob (this, jobTitle, printPrefs);
      
      //If the user clicked Cancel in the print dialog, then do nothing
      if (pjob != null) {
	PrintDlg p = getPrintDlg ();
	p.reset (pjob, this);
      }
    }
    normalCursor ();
  }
  
  boolean checkPrintAllowed () {
    try {
      SecurityManager sm = System.getSecurityManager();
      if (sm != null) sm.checkPrintJobAccess();
    }
    catch (SecurityException e) {
      new ErrorDiag (null, ("Sorry. Printing is not allowed."));
      return false;
    }
    return true;
  }
  
  PrintDlg getPrintDlg () {return new PrintDlg (this);}
  
  /**
   * sets the current cursor to WAIT_CURSOR type
   */
  void waitCursor () {setCursor (new Cursor (Cursor.WAIT_CURSOR));} 
  
  /**
   * sets the WAIT_CURSOR to cursor associated with this frame
   */
  void normalCursor () {setCursor (new Cursor (Cursor.DEFAULT_CURSOR));}
  
  /**
   * destructor
   */
  void kill () {
    parent.waitCursor ();
    setVisible (false);
    if (stateButtons != null) {stateButtons.kill (); stateButtons = null;}
    if (canvas != null) {canvas.kill (); canvas = null;}
    vcanvas1 = null;
    vcanvas2 = null;
    
    if (optionsDlg != null) {optionsDlg.dispose (); optionsDlg = null;}
    if (procDlg != null) {procDlg.dispose (); procDlg = null;}
    //    if (printDlg != null) {printDlg.dispose (); printDlg = null;}
    
    mtns = null; 
    stateDefs = null;
    data = null;
    quiver = null;
    removeAll ();
    dispose ();
    parent.normalCursor ();
    parent.delDisp ();
  }
  
  protected void finalize() throws Throwable {super.finalize();}
  
  void makeUIChanges () {
    SwingUtilities.updateComponentTreeUI(this);
    optionsDlg.makeUIChanges ();
    procDlg.makeUIChanges ();
    //    SwingUtilities.updateComponentTreeUI(printDlg);
    SwingUtilities.updateComponentTreeUI (stateButtons);
    rulerColor = getBackground ();
    canvas.Resize ();
  }
}
