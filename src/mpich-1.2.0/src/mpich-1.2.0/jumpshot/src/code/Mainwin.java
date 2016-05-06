import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import com.sun.java.swing.*;
import com.sun.java.swing.border.*;

/*
 * Use line below if you are using:
 *   1) Java 1.1.6 + Swing 1.0.3
 */
//import com.sun.java.swing.preview.JFileChooser;

/*
 * You do not need the above if your are using:
 *   1) Java 1.1.6 or higher + Swing 1.1beta
 *   2) Java 1.2beta4 or higher
 * This is because filechooser is now part of regular swing package
 */

/*
  This class supervises the reading and display of a clog file. 
  
  It shows a Frame consisting of (New Frame), (Select File) and (Exit) buttons 
  along with a text field showing the current filename. 
  
  1. The log file to be read is selected via FileDialog object (openAppFileDlg)
  2. An instance of ClogReader class is created with this filename which traverses
     the log file and supervises the creation of data structures.
  3. The data structures created reside in RecordHandler object which is returned.
  4. This class (Mainwin) then takes the RecordHandler object along with the 
     data structures creates an instance of ClogDisplay class which handles all
     drawing, printing, manipulation, etc.
*/

public class Mainwin extends JFrame 
implements ActionListener {
  RecordHandler mainTool;                    //Object containing data
  private ClogDisplay disp;                          //Frame displaying all data                 
  private SwingWorker readWorker;                    //Thread doing reading
  
  private String setupFile = "jumpshot.setup";       //file containing default settings          
  private String distributionDir = "./";             //Directory where the distribution is
  String distributionUrl;
  private String classDir1 = "js_classes";      //Directory where byte code is stored
  private String classDir2 = "js_classes.jar";
  private String dataDir = "data";                   //Directory name where the ASCII file's are
  private String logFileDir = "logfiles";
  
  private Object openAppFileDlg;
  private ApltFileDlg openApltFileDlg;
  private MyTextField logFileField;
  String logFileName;
  private JMenuItem metalMenuItem, selectFileMenuItem;
  
  Font frameFont; 
  Color frameBColor, frameFColor;
  public Dimension dimPG;
  
  private HelpDlg helpDlg;
  private InputStream helpin;
  Mainwin startwin;
  private JMenuBar menuBar;
  
  //This boolean value tells us if this instance of Mainwin is a child or not
  //If it is a child then when u exit it only its descendents will be terminated
  //If however, u exit the father all children will also be exited
  private boolean child;
  
  boolean aplt;
  
  jumpshot parent;
  
  int dtype;
  
  private String about = "Jumpshot Logfile Analyzer V 1.2\nmpi-maint@mcs.anl.gov";
  
  private MyButton dispB;
  private boolean reader_alive;
  
  public Mainwin (jumpshot p, String fileName, boolean a, boolean c) {
    parent = p; aplt = a; child = c;
    logFileName = fileName;
    startwin = this;
    if (logFileName == null) logFileName = new String ("No Name");
    setup ();
  }
  
  //setup methods-----------------------------------------------------------------
  private void setup () {
    setupUI ();
    adjustFrameStuff ();
    setupData ();
    setupDlgs ();
    setupPanels ();
    disableDisp ();
    setupEventHandlers ();
    pack (); 
    setSize (getMinimumSize ()); setResizable (false);
    setVisible (true);
    if (!(logFileName.equals ("No Name"))) {
      if (aplt) logFileName = distributionUrl + logFileName;
      readLogFile (logFileName);
    }
  }
  
  private void setupUI () {
    // Force SwingSet to come up in the Cross Platform L&F
    try {
      UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
      // If you want the System L&F instead, comment out the above line and
      // uncomment the following:
      // UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    } catch (Exception exc) {
      new ErrorDiag (null, "Error loading L&F: " + exc);
    }
  }
  
  private void adjustFrameStuff () {
    if (child) setTitle ("Jumpshot: (Child)");
    else setTitle ("Jumpshot");
    Toolkit toolkit = getToolkit ();
    Dimension dimScreen = new Dimension (toolkit.getScreenSize ());
    dimPG = new Dimension ((int)Math.rint (dimScreen.width * 0.75), 
                           (int)Math.rint (dimScreen.height * 0.5));
    frameBColor = Color.lightGray;
    frameFColor = Color.black;
    frameFont = new Font ("Serif", Font.PLAIN, 10);
  }
  
  private void setupData () {
    //Obtain the settings specified by the user.
    Properties settings = new Properties ();
    InputStream setin, colorin;
    String path;
    
    if (!aplt) {
      path = getDataPath ();
      if (path == null) {
	new ErrorDiag (null, "Jumpshot cannot locate setup files. Exiting"); return;
      }
      setin = getFileIn (path +  setupFile); 
    }
    else {
      path = getDataUrl ();
      if (path == null) {
	new ErrorDiag (null, "Jumpshot cannot locate setup files. Exiting"); return;
      }
      setin = getUrlIn (path + setupFile);
    }
    
    try {settings.load (setin); setin.close ();}
    catch (IOException x) {new ErrorDiag (this, "IO Error:" + x.getMessage ()); return;}
    
    String colorFile = settings.getProperty ("COLORFILE", "jumpshot.colors");
    String helpFile  = settings.getProperty ("HELPFILE", "jumpshot.help");
    
    if (!aplt) {
      colorin = getFileIn (path + colorFile); helpin = getFileIn (path + helpFile);
    }
    else {
      colorin = getUrlIn (path + colorFile); helpin = getUrlIn (path + helpFile);
    }
        
    COLOR_UTIL.readColors (this, colorin);
    
    reader_alive = false;
    mainTool = null;
  }
    
  
  //Bug: Method setCurrentDirectory of JFileChooser does not work completely. So, method 
  //rescanCurrentDirectory has to be called also to get the desired effect. We want
  //the JFileChooser dialog to open up with a listing of files of the directory from
  //which Jumpshot was executed
  private void setupDlgs () {
    if (!aplt) {
      Class c = null; openAppFileDlg = null;
      try {c = Class.forName ("com.sun.java.swing.preview.JFileChooser");}
      catch (ClassNotFoundException e1) {
	try {c = Class.forName ("com.sun.java.swing.JFileChooser");}
	catch (ClassNotFoundException e2) {System.err.println (e2.toString ());}
      }
      
      if (c != null) {
	try {openAppFileDlg = c.newInstance ();}
	catch (InstantiationException e) {System.err.println (e.toString ());}
	catch (IllegalAccessException e) {System.err.println (e.toString ());}
      }

      //ExFileFilter filter = new ExFileFilter (new String [] {"clog"}, "clog logfiles");
      
      if (openAppFileDlg != null) {
	//	ROUTINES.invokeMethod (openAppFileDlg, "addChoosableFileFilter", filter, 
	//	       filter.getClass ());
	//	ROUTINES.invokeMethod (openAppFileDlg, "setFileFilter", filter, 
	//	       (filter.getClass ()).getSuperclass ());
	File f = new File ((System.getProperties ()).getProperty ("user.dir"));
	String s = new String ("Select Logfile");
	ROUTINES.invokeMethod (openAppFileDlg, "setDialogTitle", s, s.getClass ());
	ROUTINES.invokeMethod (openAppFileDlg, "setCurrentDirectory", f, f.getClass ());
	ROUTINES.invokeMethod (openAppFileDlg, "rescanCurrentDirectory", null, null);
      }
      else selectFileMenuItem.setEnabled (false);
    }
    else
      openApltFileDlg = new ApltFileDlg (this, "Select Logfile");
    helpDlg = new HelpDlg ((Frame)this, readLines (helpin));
  }
  
  private void setupPanels () {
    getContentPane ().setLayout (new GridBagLayout ());
    GridBagConstraints con = new GridBagConstraints (); con.anchor = GridBagConstraints.WEST; 
    con.fill = GridBagConstraints.HORIZONTAL;
    
    getContentPane ().add (setupMenuBar (), con);
    
    con.fill = GridBagConstraints.NONE;
    //JPanels are used now. But, if the problem with using menus is solved
    //they should be used instead
    
    Border border1, border2 = BorderFactory.createLoweredBevelBorder ();
    
    //Logfile field
    JPanel pl = new JPanel (new FlowLayout ());
    border1 = BorderFactory.createEmptyBorder (4, 4, 2, 4);
    pl.setBorder (BorderFactory.createCompoundBorder (border1, border2));

    pl.add (new JLabel ("Logfile")); //, loadImageIcon ("images/baskll1.gif"), JLabel.RIGHT));
    
    logFileField = new MyTextField (logFileName, 35, false);
    pl.add (logFileField);
    
    con.gridy = 1; 
    getContentPane ().add (pl, con);
    
    con.gridy = 2; con.gridx = 0;
    dispB = new MyButton ("Display", "Display loaded logfile", this);
    getContentPane ().add (dispB, con);
  }
  
  private JMenuBar setupMenuBar () {
    menuBar = new JMenuBar();
    
    //File Menu
    JMenu file = (JMenu) menuBar.add(new JMenu("File")); 
    
    JMenuItem menuItem = file.add (new JMenuItem ("New Frame"));
    menuItem.addActionListener (this); menuItem.setHorizontalTextPosition (JButton.RIGHT);
    
    selectFileMenuItem = new JMenuItem ("Select Logfile");
    file.add (selectFileMenuItem); selectFileMenuItem.addActionListener (this);  
    selectFileMenuItem.setHorizontalTextPosition (JButton.RIGHT);
    
    menuItem = file.add (new JMenuItem ("Exit"));
    menuItem.addActionListener (this);  menuItem.setHorizontalTextPosition (JButton.RIGHT);
    
    menuBar.add (file);
    
    //Display Menu
    JMenu dispM = (JMenu) menuBar.add(new JMenu("Display"));
    
    ButtonGroup dGroup = new ButtonGroup();
    DispListener dispListener = new DispListener();
    
    JRadioButtonMenuItem rbM;
    rbM = (JRadioButtonMenuItem)dispM.add(new JRadioButtonMenuItem("Time Lines"));
    dGroup.add(rbM); rbM.addItemListener(dispListener);
    rbM.setSelected (true); dtype = CONST.TIMELINES; //Setting dtype to time lines
    
    rbM = (JRadioButtonMenuItem)dispM.add(new JRadioButtonMenuItem("Mountain Ranges"));
    dGroup.add(rbM); rbM.addItemListener(dispListener);
    
    menuBar.add (dispM);
    
    // Sys Options Menu
    JMenu options = (JMenu) menuBar.add(new JMenu("System"));
    
    // Look and Feel Radio control
    ButtonGroup group = new ButtonGroup();
    
    ToggleUIListener toggleUIListener = new ToggleUIListener();
    
    JMenuItem windowsMenuItem, motifMenuItem, macMenuItem;
    
    windowsMenuItem = 
      (JRadioButtonMenuItem) options.add(new JRadioButtonMenuItem("Windows Style Look and Feel"));
    windowsMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Windows"));
    group.add(windowsMenuItem);
    windowsMenuItem.addItemListener(toggleUIListener);
    
    motifMenuItem = (JRadioButtonMenuItem) options.add(new JRadioButtonMenuItem("Motif Look and Feel"));
    motifMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("CDE/Motif"));
    group.add(motifMenuItem);
    motifMenuItem.addItemListener(toggleUIListener);
    
    metalMenuItem = (JRadioButtonMenuItem) options.add(new JRadioButtonMenuItem("Metal Look and Feel"));
    metalMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Metal"));
    metalMenuItem.setSelected(true);
    group.add(metalMenuItem);
    metalMenuItem.addItemListener(toggleUIListener);

    macMenuItem = 
      (JRadioButtonMenuItem) options.add(new JRadioButtonMenuItem("Macintosh Look and Feel"));
    macMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Macintosh"));
    group.add(macMenuItem);
    macMenuItem.addItemListener(toggleUIListener);

    // Tooltip checkbox
    options.add(new JSeparator());
    
    JCheckBoxMenuItem cb = new JCheckBoxMenuItem ("Show ToolTips");
    options.add(cb); cb.setSelected(true);
    
    cb.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	JCheckBoxMenuItem b = (JCheckBoxMenuItem)e.getSource();
	if(b.isSelected()) {
	  ToolTipManager.sharedInstance().setEnabled(true);
	} else {
	  ToolTipManager.sharedInstance().setEnabled(false);
	}
      }
    });
    
    //Help Menu
    JMenu help = (JMenu) menuBar.add(new JMenu("Help"));
    
    menuItem = help.add (new JMenuItem ("Manual"));
    menuItem.addActionListener (this);
    
    menuItem = help.add (new JMenuItem ("About"));
    menuItem.addActionListener (this);
    
    menuBar.add (help);
    
    return menuBar;
  }
  
  private void setupEventHandlers () {
    this.enableEvents (AWTEvent.ACTION_EVENT_MASK);
    
    // Define, instantiate and register a WindowListener object.
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {close ();}
    });
  }
  
  //End of setup methods---------------------------------------------------------
  
  //Event Handler methods--------------------------------------------------------
  //Only events from buttons are generated
  
  /**
   * Handles action events generated from buttons
   */
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (command.equals ("New Frame")) new Mainwin (parent, null, aplt, true);
    else if (command.equals ("Select Logfile")) selectFile ();
    else if (command.equals ("Exit")) close ();
    else if (command.equals ("Manual")) helpDlg.setVisible (true);
    else if (command.equals ("About")) 
      JOptionPane.showMessageDialog (this, about, "About", JOptionPane.INFORMATION_MESSAGE, 
				     loadImageIcon ("images/jumpshot.gif"));
    else if (command.equals ("Display")) displayStuff ();
  }
  //End of event Handler methods-------------------------------------------------
  
  private void displayStuff () {  
    if (mainTool != null) {
      disableDisp (); 
      waitCursor ();
      disp = new ClogDisplay (this);
      normalCursor ();
    }
  }
  
  private void disableDisp () {dispB.setEnabled (false);}
  
  private void enableDisp () {dispB.setEnabled (true);}
  
  /**
   * selects the log file to be read
   */
  private void selectFile () {
    if (!aplt) { //Application - Read file from machine's memory
      Component comp; Class c = this.getClass ();
      
      while (!(c.getName ().equals ("java.awt.Component"))) {
	c = c.getSuperclass();
      }
            
      int r = ((Integer)ROUTINES.invokeMethod 
	       (openAppFileDlg, "showOpenDialog", (Component)this, c)).intValue ();
      if (r == 0) {
	String file;
	File f = (File)ROUTINES.invokeMethod (openAppFileDlg, "getSelectedFile", null, null);
	if (f != null) file = f.toString (); else file = null;
	
	if (file != null) {
	  logFileName = file;
	  logFileField.setText (logFileName);
	  readLogFile (logFileName);
	}
      }
      else JOptionPane.showMessageDialog (this, "No file chosen");
    } else { //Applet - Read file from host machine's memory 
      openApltFileDlg.show ();
      if (openApltFileDlg.select) {
	String file = openApltFileDlg.getFile ();
	if (file != null) {
	  logFileName = file;
	  logFileField.setText (logFileName);
	  readLogFile (logFileName);
	}
      }
      else JOptionPane.showMessageDialog (this, "No file chosen");
    }
  }
  
  /**
   * reads a log file by creating a ClogReader instance 
   */
  private void readLogFile (String inFile) {
    //Clean up and free resources
    freeMem ();
    
    //Spawn new thread to read from clog file
    readWorker = new SwingWorker() {
      public Object construct() {
	waitCursor ();
	disableDisp ();
	reader_alive = true;
	return (new ClogReader ((JFrame)startwin, logFileName, aplt)).mainTool;
      }
      public void finished () {
	reader_alive = false;
	mainTool = (RecordHandler)get ();
	normalCursor ();
	if (mainTool != null) enableDisp ();
	displayStuff ();
      }
    };
  }
    
  /**
   * Destructor
   */
  private void close () {
    freeMem (); 
    this.setVisible (false); this.dispose ();
    //    System.runFinalization (); System.gc ();
    if (!child && !aplt) System.exit (0);
  }
  
  /**
   * frees up the memory - reader and display
   */
  private void freeMem () {
    if (reader_alive) {readWorker.interrupt (); readWorker = null;}
    if (disp != null) disp.kill ();
    mainTool = null;
    //    System.runFinalization (); System.gc ();
  } 
  
  /**
   * sets the current cursor to DEFAULT_CURSOR
   */
  void normalCursor () {
    ROUTINES.makeCursor (this, new Cursor (Cursor.DEFAULT_CURSOR));
  }
  
  /**
   * sets the current cursor to WAIT_CURSOR
   */
  void waitCursor () {
    ROUTINES.makeCursor (this, new Cursor (Cursor.WAIT_CURSOR));
  }
    
  /**
   * method reads all the lines of text from the given file. All lines are
   * stored in one string separated by '/n's.
   */
  private String readLines (InputStream file) {
    if (file == null) return "";

    BufferedReader in = new BufferedReader (new InputStreamReader (file));
    String line, text = "";
    
    try {
      while ((line = in.readLine ()) != null) text += (line + "\n");
      file.close ();
    }
    catch (IOException x) {new ErrorDiag (this, "IO Error:" + x.getMessage ());}

    return text;
  }
  
  private InputStream getFileIn (String name) {
    InputStream in = null;
    
    try {in = new FileInputStream (name);}
    catch (FileNotFoundException e) {
      new ErrorDiag (this, "file: " + name + " not found.");
      return null;
    }
    return in;
  }
  
  private InputStream getUrlIn (String name) {
    URL url = null;
    InputStream in = null;
    
    try {url = new URL (name);}
    catch (MalformedURLException e) {
      new ErrorDiag (this, "Bad URL:" + url);
      return null;
    }
   
    try {in = url.openStream ();}
    catch (IOException e) {
      new ErrorDiag (this, "IO Error:" + e.getMessage ());
      return null;
    }
    return in;
  }
 
   
  /**
   * This method returns the path to data directory (..../jumpshot.../data/) 
   * in which data files including jumpshot.setup reside. This path is extracted 
   * from Java class path present in the set of system properties. Java class path 
   * contains a pointer to the classes directory having all the bytecode. From this 
   * classes path, distribution path (...jumpshot-1.0) is got to which data/
   * is added to obtain data directory path
   */
  private String getDataPath () {
    Properties s = System.getProperties ();
    
    String classpath = s.getProperty ("java.class.path");
    
    StringTokenizer tokens = new StringTokenizer (classpath, File.pathSeparator);
    
    int ct = tokens.countTokens ();

    for (int i = 0; i < ct; i++) {
      String dir;
      File f = new File (tokens.nextToken ());
      try {dir = f.getCanonicalPath ();}
      catch (IOException x) {
	new ErrorDiag (this, "IO Exception:" + x.getMessage());
	continue;
      }
      if (dir.endsWith (classDir1)) {
        distributionDir = dir.substring (0, dir.lastIndexOf (classDir1));
	return (distributionDir + dataDir + File.separator);
      }
      else if (dir.endsWith (classDir2)) {
	distributionDir = dir.substring (0, dir.lastIndexOf (classDir2));
	return (distributionDir + dataDir + File.separator);
      }
    }
    new ErrorDiag (null, "Neither of " + classDir1 + " nor " + classDir2 + " could be found" +
		   " in java.class.classpath");
    return null;
  }
  
  /**
   * Similar explanation as above method
   */
  private String getDataUrl () {
    String url = (parent.getCodeBase ()).toString ();
    
    int i = url.lastIndexOf (classDir1);
    if (i == -1) distributionUrl = url;
    else
      distributionUrl = url.substring (0, i);
    
    return (distributionUrl + dataDir + "/"); 
    //Here front slash "/" is used because this is the specified character 
    //that is a file separator in url conventions
  }
  
  private ImageIcon loadImageIcon(String filename) {
    if(!aplt) {
      return new ImageIcon (distributionDir + filename);
    }
    else {
      URL url = null;

      try {url = new URL (distributionUrl + filename);}
      catch (MalformedURLException e) {
	new ErrorDiag (this, "Bad URL:" + url);
	return null;
      }
      return new ImageIcon(url);
    }
  }
  
  void delDisp () {
    enableDisp (); disp = null;
    //    System.runFinalization (); System.gc ();
  }
  
  /**
   * Switch the between the Windows, Motif, Mac, and the Metal Look and Feel
   */
  private class ToggleUIListener implements ItemListener {
    public void itemStateChanged(ItemEvent e) {
      waitCursor ();

      JRadioButtonMenuItem rb = (JRadioButtonMenuItem) e.getSource();
      try {
	if(rb.isSelected() && rb.getText().equals("Windows Style Look and Feel")) {
	  UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
	  makeUIChanges ();
	} 
	else if(rb.isSelected() && rb.getText().equals("Macintosh Look and Feel")) {
	  UIManager.setLookAndFeel("com.sun.java.swing.plaf.mac.MacLookAndFeel");
	  makeUIChanges ();
	} 
	else if(rb.isSelected() && rb.getText().equals("Motif Look and Feel")) {
	  UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
	  makeUIChanges ();
	} 
	else if(rb.isSelected() && rb.getText().equals("Metal Look and Feel")) {
	  UIManager.setLookAndFeel("com.sun.java.swing.plaf.metal.MetalLookAndFeel");
	  makeUIChanges ();
	} 
      } 
      catch (UnsupportedLookAndFeelException exc) {
	// Error - unsupported L&F
	rb.setEnabled(false);
	new ErrorDiag (null, "Unsupported LookAndFeel: " + rb.getText() + 
		       ".\nLoading cross platform look and feel");
	
	// Set L&F to Metal
	try {
	  metalMenuItem.setSelected(true);
	  UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
	  SwingUtilities.updateComponentTreeUI(startwin);
	} catch (Exception exc2) {
	  new ErrorDiag (null, "Could not load LookAndFeel: " + exc2);
	}
      } 
      catch (Exception exc) {
	rb.setEnabled(false);
	new ErrorDiag (null, "Could not load LookAndFeel: " + rb.getText());
      }
      normalCursor ();
    }
  }
  
  private class DispListener implements ItemListener {
    public void itemStateChanged(ItemEvent e) {
      JRadioButtonMenuItem rb = (JRadioButtonMenuItem) e.getSource();
      if(rb.isSelected() && rb.getText().equals("Mountain Ranges")) dtype = CONST.MTN_RANGES;
      else if(rb.isSelected() && rb.getText().equals("Time Lines")) dtype = CONST.TIMELINES;
    }
  }
  
  private void makeUIChanges () {
    SwingUtilities.updateComponentTreeUI(this);
    setResizable (true); setSize (getMinimumSize ()); setResizable (false);
    if (!aplt) 
      SwingUtilities.updateComponentTreeUI ((Component)openAppFileDlg);
    else
      SwingUtilities.updateComponentTreeUI (openApltFileDlg);
    
    SwingUtilities.updateComponentTreeUI (helpDlg);
    if (disp != null) disp.makeUIChanges ();
  }
  
  protected void finalize() throws Throwable {super.finalize();}
}
