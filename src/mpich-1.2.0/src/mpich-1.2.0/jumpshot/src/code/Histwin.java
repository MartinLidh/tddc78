import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.PrintJob;
import java.awt.Cursor;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Color;
import com.sun.java.swing.JMenuBar;
import com.sun.java.swing.JMenu;
import com.sun.java.swing.JSlider;
import com.sun.java.swing.JScrollBar;
import com.sun.java.swing.BorderFactory;
import com.sun.java.swing.JLabel;
import com.sun.java.swing.JPanel;
import com.sun.java.swing.JFrame;
import com.sun.java.swing.border.Border;

public class Histwin extends JFrame 
implements ActionListener {
  ClogDisplay parent;                 
  HistCanvas canvas;          
  public CLOG_STATE stateDef;
  JSlider binSlider;
  JScrollBar hbar;
  
  JLabel stateNameLabel; 
  MyTextField numInstField, maxLenField, leastLenField;
  MyTextField statesInViewField, pcInViewField, areaField, bAreaField;
  MyTextField startLenField, endLenField, numBinsField, maxNumBinsField, zFacField, cursorField;
  
  JMenu regionsMenu;
  
  public Histwin (CLOG_STATE s, ClogDisplay p) {
    super ("Histogram: " + s.description.desc);
    parent = p;
    stateDef = s;
    setup ();
  }
  
  void setup () {
    waitCursor ();
    Dimension dimScreen = new Dimension (getToolkit ().getScreenSize ());
    setSize (new Dimension (dimScreen.width / 2, dimScreen.height / 2));
    setupCanvas ();
    setupPanels ();
    setVisible (true);
    drawCanvas ();
    setupEventHandlers ();
    normalCursor ();
  }
  
  void setupEventHandlers () {
    hbar.addAdjustmentListener (canvas);
    binSlider.addChangeListener (canvas);
    startLenField.addActionListener (canvas); 
    startLenField.setActionCommand ("Change Start Len");
    endLenField.addActionListener  (canvas); 
    endLenField.setActionCommand ("Change End Len");
    numBinsField.addActionListener (canvas); 
    numBinsField.setActionCommand ("Change numBins");
    maxNumBinsField.addActionListener (canvas);
    maxNumBinsField.setActionCommand ("Change maxNumBins"); 
    zFacField.addActionListener (canvas);
    zFacField.setActionCommand ("Change Zoom Fac");
    
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {kill ();}
    });
  }
  
  void setupCanvas () {canvas = new HistCanvas (this);}
  
  void drawCanvas () {canvas.init ();}
  
  /**
   * Lays out the entire panel
   */
  void setupPanels () {
    getContentPane ().setLayout (new BorderLayout ());
    
    Border border2 = BorderFactory.createLoweredBevelBorder (), border1;
    
    GridBagConstraints con = new GridBagConstraints (); con.anchor = GridBagConstraints.WEST;
    con.fill = GridBagConstraints.HORIZONTAL; con.weightx = 1;
    
    /* North panel **************************************************************************/
    
    JPanel northP = new JPanel (new GridBagLayout ());
    
    //Constants
    JPanel constP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (4, 4, 2, 2);
    constP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
  
    con.gridy = 0; con.gridx = 0;
    constP.add (new JLabel ("cursor"), con);
    
    con.gridx = 1;
    constP.add (cursorField = new MyTextField ("", 10, false), con);
    
    con.gridx = 2;
    constP.add (new JLabel ("sec"), con);
    
    con.gridy = 1; con.gridx = 0;
    constP.add (new JLabel ("Total number of instances"), con);
    
    con.gridx = 1;
    constP.add (numInstField = new MyTextField (10, false), con);
    
    con.gridy = 2; con.gridx = 0;
    constP.add (new JLabel ("Duration of shortest instance "), con);
    
    con.gridx = 1;
    constP.add (leastLenField = new MyTextField (10, false), con);
    
    con.gridx = 2;
    constP.add (new JLabel ("sec"), con);
    
    con.gridy = 3; con.gridx = 0;
    constP.add (new JLabel ("Duration of longest instance"), con);
    
    con.gridx = 1;
    constP.add (maxLenField = new MyTextField (10, false), con);
    
    con.gridx = 2;
    constP.add (new JLabel ("sec"), con);
    
    con.gridy = 0; con.gridx = 0;
    northP.add (constP, con);
    
    //Controls
    JPanel contP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (4, 2, 2, 4);
    contP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    con.gridy = 0; con.gridx = 0; con.weighty = 0;
    contP.add (new JLabel ("States in View"), con);
    
    con.gridx = 1;
    contP.add (statesInViewField = new MyTextField ("", 10, false), con);
    
    con.gridy = 1; con.gridx = 0;
    contP.add (new JLabel ("Percent of total states "), con);
    
    con.gridx = 1;
    contP.add (pcInViewField = new MyTextField ("", 10, false), con);
    
    con.gridx = 2;
    contP.add (new JLabel ("%"), con);
    
    con.gridy = 2; con.gridx = 0;
    contP.add (new JLabel ("Start state length"), con);
    
    con.gridx = 1;
    contP.add (startLenField = new MyTextField ("", 10, true), con);
    
    con.gridx = 2;
    contP.add (new JLabel ("sec"), con);
    
    con.gridy = 3; con.gridx = 0;
    contP.add (new JLabel ("End state length"), con);
  
    con.gridx = 1;
    contP.add (endLenField = new MyTextField ("", 10, true), con);
    
    con.gridx = 2;
    contP.add (new JLabel ("sec"), con);
    
    con.gridy = 0; con.gridx = 1;
    northP.add (contP, con);
    
    getContentPane ().add (northP, BorderLayout.NORTH);
    
    /* West Panel *****************************************************************************/
    JPanel westP1 = new JPanel (new BorderLayout ());
    
    JPanel westP = new JPanel (new GridBagLayout ());
    
    con.fill = GridBagConstraints.NONE;
    con.gridy = 0; con.gridx = 0; con.weightx = 0;

    westP.add (stateNameLabel = new JLabel (), con);
    stateNameLabel.setFont (new Font ("Serif", Font.BOLD, 18));
    stateNameLabel.setBorder (BorderFactory.createEmptyBorder (2, 4, 2, 2));
	
    //Zoom Controls
    JPanel zoomP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 2, 2);
    zoomP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    con.gridy = 0; con.gridx = 0;
    zoomP.add (new JLabel ("Zoom Factor "), con);
    
    con.gridx = 1; 
    zoomP.add (zFacField = new MyTextField ("", 5, true), con);
    
    con.gridy = 1; con.gridx = 0; con.gridwidth = 2;
    JPanel zb = new JPanel (new GridLayout (1,3));
    zb.add (new MyButton ("In", "Zoom In Horizontally", canvas)); 
    zb.add (new MyButton ("Out", "Zoom Out Horizontally", canvas));
    zb.add (new MyButton ("Reset", "Draw entire data in viewport", canvas));
    zoomP.add (zb, con);
    
    con.gridy = 1; con.gridx = 0; con.gridwidth = 1; con.fill = GridBagConstraints.HORIZONTAL;
    westP.add (zoomP, con);
    
    con.fill = GridBagConstraints.NONE;
    //Bin controls 
    JPanel binP = new JPanel (new GridBagLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 2, 2);
    binP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    con.gridy = 0; con.gridx = 0; con.gridwidth = 1;
    binP.add (new JLabel ("Max Number of bins "), con);
    
    con.gridx = 1;
    binP.add (maxNumBinsField = new MyTextField ("", 5, true), con);
    
    con.gridy = 1; con.gridx = 0; con.gridwidth = 2; con.fill = GridBagConstraints.HORIZONTAL;
    binP.add (binSlider = new JSlider (), con); con.fill = GridBagConstraints.NONE;
    
    con.gridy = 2; con.gridx = 0; con.gridwidth = 1;
    binP.add (new JLabel ("Number of bins"), con);
    
    con.gridx = 1;
    binP.add (numBinsField = new MyTextField ("", 5, true), con);
    
    con.gridy = 2; con.gridx = 0; con.gridwidth = 1; con.fill = GridBagConstraints.HORIZONTAL;
    westP.add (binP, con); con.fill = GridBagConstraints.NONE;
    
    //Normal distribution
    JPanel normP = new JPanel (new BorderLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 2, 2);
    normP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    normP.add (new JLabel ("Regions (Top X %) assuming normal distribution"), BorderLayout.NORTH);
    
    JPanel bP = new JPanel (new GridLayout (3, 3));
    bP.add (new MyButton ("1%", canvas.tAL)); bP.add (new MyButton ("5%", canvas.tAL));
    bP.add (new MyButton ("10%", canvas.tAL)); bP.add (new MyButton ("20%", canvas.tAL));
    bP.add (new MyButton ("30%", canvas.tAL)); bP.add (new MyButton ("50%", canvas.tAL));
    bP.add (areaField = new MyTextField ("", 4, false), con);
    normP.add (bP, BorderLayout.SOUTH);
    
    con.gridy = 3; con.gridx = 0; con.gridwidth = 1; con.fill = GridBagConstraints.HORIZONTAL;
    westP.add (normP, con);
    
    normP = new JPanel (new BorderLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 2, 2);
    normP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    normP.add (new JLabel ("Regions (Bottom X %) assuming normal distribution"), 
	       BorderLayout.NORTH);
    bP = new JPanel (new GridLayout (3, 3));
    bP.add (new MyButton ("1%", canvas.bAL)); bP.add (new MyButton ("5%", canvas.bAL));
    bP.add (new MyButton ("10%", canvas.bAL)); bP.add (new MyButton ("20%", canvas.bAL));
    bP.add (new MyButton ("30%", canvas.bAL)); bP.add (new MyButton ("50%", canvas.bAL));
    bP.add (bAreaField = new MyTextField ("", 4, false), con);
    normP.add (bP, BorderLayout.SOUTH);
    
    con.gridy = 4; con.gridx = 0;
    westP.add (normP, con);
    
    westP1.add (westP, BorderLayout.NORTH);
    //This panel is added to ensure correct positioning of westP during resizing of frame
    westP1.add (new JPanel (), BorderLayout.CENTER);
    getContentPane ().add (westP1, BorderLayout.WEST);
    
    /* Center Panel ***************************************************************************/
    
    JPanel cenP = new JPanel (new BorderLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 2, 2, 4);
    cenP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
    JPanel canPanel = new JPanel (new BorderLayout ());
    canPanel.add (canvas, BorderLayout.CENTER);
    cenP.add (canPanel, BorderLayout.CENTER);

    cenP.add (hbar = new JScrollBar (JScrollBar.HORIZONTAL), BorderLayout.SOUTH);
    
    getContentPane ().add (cenP, BorderLayout.CENTER);
  
    /* South Panel ****************************************************************************/
    JPanel southP = new JPanel (new GridLayout (1, 5));
    border1 = BorderFactory.createEmptyBorder (2, 4, 4, 4);
    southP.setBorder (BorderFactory.createCompoundBorder (border1, border2));
  
    southP.add (new MyButton ("Resize to fit", 
			      "Set height of histograms to fit display", canvas));
    southP.add (new MyButton ("Blink states", "Blink states in histogram in time lines display",
			      canvas));
    
    JMenuBar menuBar = new JMenuBar ();
    regionsMenu = new JMenu ("Selected Regions", true);
    regionsMenu.setToolTipText ("Contains list of all selected regions. Click on selected region"
				+ " to disable blinking");
    menuBar.add (regionsMenu);
    southP.add (menuBar);
    
    MyButton button = new MyButton ("Print", "Print display to file or printer", this);
    //    if (parent.parent.aplt) button.setEnabled (false);
    southP.add (button);
    southP.add (new MyButton ("Close", "Close window", this));
    
    getContentPane ().add (southP, BorderLayout.SOUTH);
  }
  
  /**
   * Prints out jumpshot data
   */
  private void Print () {
    waitCursor ();
    
    if (parent.checkPrintAllowed ()) {
      String jobTitle = "Jumpshot:" + parent.parent.logFileName;
    
      //Obtain a PrintJob Object. This posts a Print dialog
      PrintJob pjob = getToolkit ().getPrintJob (this, jobTitle, parent.printPrefs);
    
      //If the user clicked Cancel in the print dialog, then do nothing
      if (pjob != null) {
	PrintDlg p = parent.getPrintDlg ();
	p.reset (pjob, this);
      }
    }
    normalCursor ();
  }

  /**
   * sets the current cursor to the WAIT_CURSOR type
   */
  void waitCursor () {setCursor (new Cursor (Cursor.WAIT_CURSOR));} 

  /**
   * sets the WAIT_CURSOR to cursor associated with this frame
   */
  void normalCursor () {setCursor (new Cursor (Cursor.DEFAULT_CURSOR));}  
 
  /**
   * Handles action events from which this is the listener
   */
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    
    if (command.equals ("Close")) {kill ();}
    else if (command.equals ("Print")) {Print ();}
   
  }
  
  void kill () {
    if (canvas != null) {
      canvas.unmarkBlinkAll (); parent.canvas.Refresh ();
      canvas.kill (); canvas = null;
    }
    dispose ();
  }  
  
  protected void finalize () throws Throwable {super.finalize ();}
}







