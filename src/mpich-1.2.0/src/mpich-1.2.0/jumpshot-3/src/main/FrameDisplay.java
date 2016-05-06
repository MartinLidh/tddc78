import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.border.Border;

/*
  This class supervises all display operations and printing for the given
  Jumpshot data. 
  
  It shows a frame consisting of buttons (Options), (Print), (Refresh), 
  (Close), (In), (Out), etc. and textFields including time cursor and
  elapsed time. It also contains the canvas showing the different states
  
  Once a slog frame is read by FrameReader, the data structures are passed
  onto this class which then produces the display.
*/
public class FrameDisplay extends JFrame 
implements AdjustmentListener, ActionListener {
  public Mainwin             init_win;
  public ViewFrameChooser    frame_chooser;
 
         // SLOG Data Structures
         Vector              stateDefs;  // List of state definitions
         StateGroupListPair  all_states;
         Vector              arrowDefs;  // List of arrow definitions
         ArrowList           all_arrows;
         Vector              mtns;

         String              view_indexes_label;
  
  Properties printPrefs;      //stores the default settings from printing 
  
  //Dialogs and frames
  //  PrintDlg printDlg;      // dialog containing print options 
  RecDefButtons def_btns;     // frame containing various records def'n buttons
  CanOptions optionsDlg;      // dialog containing options
  StateGroupDialog procDlg;            // dialog containing process options

  //canvas's
  ProgramCanvas canvas;       // canvas where states are drawn
  VertScaleCanvas vcanvas1, vcanvas2; //canvas's where process numbers are drawn
  
  // object that controls view into the much larger JComponent
  JViewport vport;
  JScrollBar hbar;            //scrollbar that controls horizontal scrolling
  private MyTextField timeField, elTimeField;
  
  Font frameFont, printFont;
  Color frameBColor, frameFColor, printBColor, printFColor;
  Color printImgBColor, normImgBColor, rulerColor;

  boolean setupComplete = false;

  int dtype = CONST.TIMELINES, vertRulerGap;
  
  /**
   * constructor
   */
  public FrameDisplay( Mainwin            in_init_win,
                       ViewFrameChooser   in_frame_chooser,
                       PlotData           slog_plotdata,
                       double             starttime,
                       double             endtime,
                       String             title_str ) 
  {
      super();
    
      init_win           = in_init_win;
      frame_chooser      = in_frame_chooser;
      dtype              = init_win.dtype;
      setTitle( CONST.DISPLAY_TYPES[ dtype ] + " : " + title_str );
     
      stateDefs          = slog_plotdata.stateDefs;
      all_states         = slog_plotdata.all_states;
      arrowDefs          = slog_plotdata.arrowDefs;
      all_arrows         = slog_plotdata.all_arrows;
      mtns               = slog_plotdata.mtns;

      view_indexes_label = slog_plotdata.view_indexes_label;

      setup( starttime, endtime );
  }
  
  // Setup methods--------------------------------------------------------------
  private void setup( double starttime, double endtime )
  {
      adjustFrameStuff ();
      setupCanvas ();
      setupPanels ();
      setupDlgs ();
      setupPrintPrefs ();
      setSize( init_win.dimPG );
      setVisible (true);
    
      // Before we draw the states on to the image, which is on the canvas the
      // frame needs to be made visible so that we get a graphics context to
      // draw upon. If the frame was not made visible a null graphics context
      // was returned
      waitCursor ();  
      drawData( starttime, endtime );
      setupEventHandlers ();
      setupComplete = true;
      normalCursor ();
      init_win.normalCursor ();
  }
  
  private void adjustFrameStuff()
  {
    frameBColor = init_win.frameBColor;
    frameFColor = init_win.frameFColor;
    frameFont = init_win.frameFont;
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
  private void setupCanvas()
  {
      canvas = new ProgramCanvas();
      vcanvas1 = new VertScaleCanvas( this, view_indexes_label );
      vcanvas2 = new VertScaleCanvas( this, view_indexes_label );
  }
  
  private void setupPanels()
  {
    getContentPane ().setLayout (new BorderLayout ());
    
    JPanel mainPanel = new JPanel (new BorderLayout ());
    
    GridBagConstraints con = new GridBagConstraints (); 
    con.anchor = GridBagConstraints.SOUTHWEST;
    con.weightx = 1; con.fill = GridBagConstraints.HORIZONTAL;
    
    Border border1; 
    Border border2 = BorderFactory.createLoweredBevelBorder ();
    
    JPanel northP = new JPanel (new GridBagLayout ());
    
        // ** The TextField in North West corner of the Main Display
        JPanel textFieldP = new JPanel (new GridBagLayout ());
        border1 = BorderFactory.createEmptyBorder (4, 4, 2, 2);
        textFieldP.setBorder(BorderFactory.createCompoundBorder(border1,
                                                                border2));
    
        con.gridy = 0; con.gridx = 0; 
        textFieldP.add (new JLabel ("Pointer "), con);
    
        con.gridx = 1; 
        textFieldP.add (timeField = new MyTextField ("", 10, false), con); 
        timeField.setToolTipText ("Cursor Position (in second) in display");
    
        con.gridy = 1; con.gridx = 0;
        textFieldP.add (new JLabel ("Elapsed Time "), con);
    
        con.gridx = 1;
        textFieldP.add (elTimeField = new MyTextField ("", 10, false), con);
        elTimeField.setToolTipText( "Cursor Position (in second) in display "
                                  + "w.r.t. Elapsed Time line");
        // **
    
    con.gridy = 0; con.gridx = 0;
    northP.add (textFieldP, con);
    
        // ** The North Central 3 buttons controlling the zoom operations
        JPanel zoomP = new JPanel (new GridLayout (1, 3));
        // border1 = BorderFactory.createEmptyBorder (4, 2, 2, 2);
        border1 = BorderFactory.createTitledBorder( new EtchedBorder(),
                                                    "Zoom Operations" );
        zoomP.setBorder( BorderFactory.createCompoundBorder(border1, border2) );
    
        zoomP.add( new MyButton( "In", "Zoom In Horizontally", this ) );
        zoomP.add( new MyButton( "Out", "Zoom Out Horizontally", this ) );
        zoomP.add( new MyButton( "Reset", "Redraws entire data in viewport",
                                 this ) );
        // **
    
    con.gridy = 0; con.gridx = 1;
    northP.add (zoomP, con);
    
        // ** The North East 3 buttons controlling misc. operations
        JPanel oP = new JPanel (new GridLayout (1, 3));
        // border1 = BorderFactory.createEmptyBorder (4, 2, 2, 4);
        border1 = BorderFactory.createTitledBorder( new EtchedBorder(),
                                                   "Miscellaneous Operations" );
        oP.setBorder( BorderFactory.createCompoundBorder(border1, border2) );
    
        oP.add( new MyButton( "Options",
                              "Display options for arrows, zoom lock line, "
                            + "elapsed time line and nesting", this));
        MyButton button = new MyButton( "Print",
                                        "Print display to file or printer",
                                        this);
            // if (init_win.aplt) button.setEnabled (false);
        oP.add (button);
        oP.add (new MyButton ("Close", "Close window", this));
        // **
    
    con.gridy = 0; con.gridx = 2;
    northP.add (oP, con);
    
    mainPanel.add (northP, BorderLayout.NORTH);
    
    // The Main Central panel  which hold the drawing canvas, srollbar...
    JPanel canPanel = new JPanel (new BorderLayout ());
    border1 = BorderFactory.createEmptyBorder (2, 4, 4, 4);
    canPanel.setBorder (BorderFactory.createCompoundBorder (border1, border2));
    
        // ** The Viewport that holds the drawing canvas
        vport = new JViewport();
        vport.setView( canvas );
        vport.addComponentListener( canvas );
    
        // Bug: JViewport does not support setBorder 
        // so we put JViewport in a Panel with a border
        JPanel vpp = new JPanel( new BorderLayout() ); 
        vpp.setBorder( BorderFactory.createCompoundBorder(
                          BorderFactory.createRaisedBevelBorder (),
                          BorderFactory.createLoweredBevelBorder ())
                     );
        vpp.add (vport, BorderLayout.CENTER);
        // **
    
        // ** The ScrollBar panel
        JPanel p = new JPanel (new BorderLayout ());
        p.add (vpp, BorderLayout.CENTER);
        p.add (hbar = new JScrollBar (JScrollBar.HORIZONTAL),
               BorderLayout.SOUTH);
        // **

    canPanel.add (p, BorderLayout.CENTER);
    
        vertRulerGap = (vpp.getInsets()).top + (p.getInsets()).top;
        vcanvas1.setVerticalGap( vertRulerGap ); 
        vcanvas2.setVerticalGap( vertRulerGap );
    
    canPanel.add(vcanvas1, BorderLayout.EAST);
    canPanel.add(vcanvas2, BorderLayout.WEST);

    mainPanel.add(canPanel, BorderLayout.CENTER);
    
    getContentPane().add(mainPanel, BorderLayout.CENTER);
  }

    private void setupEventHandlers()
    {
        hbar.addAdjustmentListener (this);
    
        addWindowListener( new WindowAdapter()
                           {
                               public void windowClosing( WindowEvent e )
                               { kill(); }
                            }
                         ); 
    }
  
  private void setupDlgs() 
  {
    optionsDlg = new CanOptions (this);
    //    printDlg = new PrintDlg (this);
    def_btns = new RecDefButtons( this, 4 );
    getContentPane().add( def_btns, BorderLayout.SOUTH );
  }   
  
  /**
   * printPrefs is the property set that stores the default settings for
   * printing.
   */
  private void setupPrintPrefs() 
  {
    printPrefs = new Properties ();
    printPrefs.put("awt.print.fileName", "jumpshot.ps");
    printPrefs.put("awt.print.numCopies", "1");
    printPrefs.put("awt.print.paperSize", "letter");
    printPrefs.put("awt.print.destination", "file");
    printPrefs.put("awt.print.orientation", "landscape");
  }
  
  // End of Setup methods -----------------------------------------------------
  
  // Event Handler methods ----------------------------------------------------
  /**
   * Handles events when scrollbar is modified
   */
  public void adjustmentValueChanged (AdjustmentEvent e)
  {
      setPosition (hbar.getValue ());
  }
  
  void setPosition (int x) {
    canvas.adjustImgH (x); 
    vport.setViewPosition (new Point (canvas.getPanePosX (), 0));
    debug.println( "FrameDisplay.setPosition(" + x + " ) : "
                 + "vport.setViewPosition(" + canvas.getPanePosX() + ", 0)" );
  }
  
  //Handler the event when one of the buttons is pressed 
    public void actionPerformed( ActionEvent evt )
    {
        String command = evt.getActionCommand();
        if ( setupComplete ) {
            if ( command.equals( "In" ) ) zoomInH();
            else if ( command.equals( "Out" ) ) zoomOutH();
            else if ( command.equals("Reset") ) resetView();
            else if ( command.equals("Options") ) {
                optionsDlg.reset();
                optionsDlg.show(); 
                optionsDlg.toFront();
            }
            else if ( command.equals ("Print") ) Print();
            else if ( command.equals ("Close") ) kill();
        }
    }

  //End of event Handler methods--------------------------------------------
  
    /**
     * draw states on to canvas
     */
    private void drawData( double starttime, double endtime )
    {
        canvas.init( this, starttime, endtime );
        vcanvas1.setupComplete();
        vcanvas2.setupComplete();
        procDlg = new StateGroupDialog( canvas, frameFont );
    }  
  
  //Zoom methods------------------------------------------------------------
  /**
   * zoom in horizontally using the current zoom factor along zoomlock if any.
   */
  private void zoomInH()
  {
    waitCursor ();
    canvas.zoomInH ();
    zoomH ();
    normalCursor ();
  }
  
  /**
   * zoom out horizontally using the current zoom factor along zoomlock if any.
   */
  private void zoomOutH()
  {
    waitCursor ();
    canvas.zoomOutH ();
    zoomH ();
    normalCursor ();
  }   

  /**
   * Reset view so that the entire data occupies one screen
   */
  private void resetView()
  {
    waitCursor ();
    canvas.resetView ();
    zoomH (); 
    normalCursor ();
  }
  
  /**
   * Method called whenever a horizontal zoom takes place
   */
  void zoomH()
  {
    setHoriz ();
    hbar.setValue (canvas.sbPos);
    vport.setViewPosition (new Point (canvas.getPanePosX (), 0));
    debug.println( "FrameDisplay.zoomH() : " );
    debug.println( "\t" + "hbar.setValue(" + canvas.sbPos + ")" );
    debug.println( "\t" + "vport.setViewPosition("
                 + canvas.getPanePosX() + ", 0)" );
  }
  
  /**
   * Reset scrollbar values when horizontal extent of data changes (e.g during zoom)
   * BUG: Id 4138681 - ScrollBar doesn't update visible amount when not at top (or left) of bar
   * http://developer.java.sun.com/developer/bugParade/bugs/4138681.html
   * They say that it is fixed in JDK1.2beta4 but, since we are not yet using JDK1.2beta4 or later
   * we will use the simple workaround
   */
  void setHoriz()
  {
    hbar.setMaximum (canvas.maxH);
    hbar.setMinimum (0);

    //Work around for the Bug Id: 4149649 /**/
    /**/int x = hbar.getValue (); hbar.setValue (0);

    hbar.setVisibleAmount ((int)Math.rint (canvas._xPix / 3.0));
    
    /**/hbar.setValue (x);
    
    hbar.setUnitIncrement (10);
    hbar.setBlockIncrement ((int)Math.rint (canvas._xPix / 3.0));
  }
  //--------------------------------------------------------------------------
  
  /**
   * modify cursor value in the time field
   */
  void adjustTimeField (double d)
  {
    timeField.setText ((new Float (d)).toString ());
  }
  
  /**
   * Modify elapsed time field value
   */
  void adjustElTimeField(double d)
  {
    elTimeField.setText ((new Float (d)).toString ());
  }
   
  /**
   * Prints out jumpshot data
   */
  private void Print()
  {
    waitCursor ();
    
    if (checkPrintAllowed ()) {
      String jobTitle = "Jumpshot:" + init_win.logFileName;
    
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
  
  boolean checkPrintAllowed()
  {
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
  
  PrintDlg getPrintDlg() {return new PrintDlg (this);}
  
  /**
   * sets the current cursor to WAIT_CURSOR type
   */
  void waitCursor() {setCursor (new Cursor (Cursor.WAIT_CURSOR));} 
  
  /**
   * sets the WAIT_CURSOR to cursor associated with this frame
   */
  void normalCursor() {setCursor (new Cursor (Cursor.DEFAULT_CURSOR));}
  
  /**
   * destructor
   */
  void kill()
  {
      init_win.waitCursor();
      setVisible( false );
      if ( def_btns != null ) {
          def_btns.kill();
          def_btns = null;
      }
      if ( canvas != null ) {
          canvas.kill ();
          canvas = null;
      }
      vcanvas1 = null;
      vcanvas2 = null;
    
      if ( optionsDlg != null) {
          optionsDlg.dispose();
          optionsDlg = null;
      }
      if ( procDlg != null) {
          procDlg.dispose();
          procDlg = null;
      }
      //  if (printDlg != null) {printDlg.dispose (); printDlg = null;}
    
      mtns       = null; 
      stateDefs  = null;
      all_states = null;
      arrowDefs  = null;
      all_arrows = null;
      removeAll();
      dispose();
      init_win.normalCursor ();
      try { frame_chooser.EnableFrameButtonsIdxFld(); }
      catch ( NullPointerException err ) {}
  }
  
  protected void finalize() throws Throwable {super.finalize();}
  
  void makeUIChanges ()
  {
    SwingUtilities.updateComponentTreeUI(this);
    optionsDlg.makeUIChanges ();
    procDlg.makeUIChanges ();
    //    SwingUtilities.updateComponentTreeUI(printDlg);
    SwingUtilities.updateComponentTreeUI (def_btns);
    rulerColor = getBackground ();
    canvas.Resize ();
  }
}
