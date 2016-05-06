import java.io.*;
import java.net.*;
import java.awt.BorderLayout;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.Cursor;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import com.sun.java.swing.JDialog;
import com.sun.java.swing.JFrame;
import com.sun.java.swing.JProgressBar;
import com.sun.java.swing.JLabel;
import com.sun.java.swing.SwingUtilities;
import com.sun.java.swing.JOptionPane;
  
/**
 * This class reads the data from the clog file and returns the finished
 * data structures to Mainwin. The clog file is traversed and records obtained.
 * Each record is passed on to a RecordHandler class which sifts out 
 * the needed information and forms the data structures
 * Length of status field will be set to 40 characters
 */
public class ClogReader extends JDialog {
  private double firsttime = 0;
  private boolean swap;
  private int numEvt1, numEvt2, numrawevt, rawevtno;
  RecordHandler mainTool;
  private String logFileName;
  private boolean aplt;
  private boolean cancel = false;
  private MyButton interruptButton;
  private JLabel statusField;
  private JProgressBar progressBar = new JProgressBar();
  
  /**
   * Constructor
   */
  public ClogReader (JFrame f, String s, boolean a) {
    super (f, "Reading logfile");
    logFileName = s;
    aplt = a;
    
    getContentPane ().setLayout(new BorderLayout ());
    MyJPanel p = new MyJPanel (new GridBagLayout (), null);
    
    GridBagConstraints con = new GridBagConstraints (); 

    con.fill = GridBagConstraints.HORIZONTAL;
    con.anchor = GridBagConstraints.WEST;
    p.add (new JLabel ("Reading " + logFileName), con);
    
    con.gridy = 1;
    p.add (progressBar = new JProgressBar (), con);
    progressBar.setMaximum(100);
    
    con.gridy = 2;
    char [] c = new char [40]; for (int i = 0; i < 40; i++) c[i] = ' ';
    p.add (statusField = new JLabel (new String (c)), con);
    
    con.gridy = 3; con.anchor = GridBagConstraints.EAST;
    con.fill = GridBagConstraints.NONE;
    p.add (interruptButton = new MyButton ("Cancel", interruptListener), con);
    
    getContentPane ().add(p, BorderLayout.CENTER);
    
    pack ();
    ROUTINES.makeCursor (this, new Cursor (Cursor.WAIT_CURSOR));
    setVisible (true);
    setResizable (false);
    
    ReadClogFile ();
    this.dispose ();
  }
   
  /**
   * reads the clog file and returns a RecordHandler object that contains
   * all the data structures
   */
  private void ReadClogFile () {    
    String filename = logFileName;
    mainTool = new RecordHandler ();
  
    //Check whether logfile is big endian. It it is little endian swapping
    //of bytes will occur. clog files are a collection of pairs
    //[CLOG_HEADER and corresponding CLOG_record]. We read a CLOG_HEADER
    //from the clog file and check if CLOG_HEADER.rectype makes sense ie.,
    //it is a valid rectype. If it is invalid we assume that the clog
    //file is little endian and swapping will have to be performed for
    //int and double values.
    statusField.setText ("Checking endian");
    boolean flag = checkEndian (filename, mainTool);
    if (!flag) {mainTool =  null; return;}
    
    //At this stage we do not know for sure where exactly the RAW records
    //will be specified. They may occur before, after or even in the middle
    //of RAW records. Thus, we take a fool proof approach by making 2 passes
    //through the file
    //Pass 1: traversing through the clog file we obtain all the state 
    //definitions
    statusField.setText ("Making first pass over file");
    flag = getStateDefs (filename, mainTool);
    if (!flag) {mainTool = null; return;} 
    mainTool.setDataSize (numrawevt);
    
    //    System.out.println ("numrawevt = " + numrawevt);
    //Pass 2: Having got the state definitions we now read RAW records
    flag = getRawEvents (filename, mainTool);
    if (!flag) {mainTool =  null; return;}
    //------------------------------or-------------------------------------
    
    //At a later date if state definitions will always occur before RAW 
    //records then we can replace the 2 passes with a single one by
    //calling the funcion below
    //boolean flag = getData (filename, mainTool);
    //if (!flag) mainTool =  null;
    
  }
  
  /**
   * checks whether file is big endian by reading CLOG_HEADER
   */
  private boolean checkEndian (String filename, RecordHandler mainTool) {
    int n = CLOG_HEADER.getSize ();
    byte [] block = new byte [n];
    InputStream ins;
    
    if (!aplt) {
      if ((ins = getFileIn (filename)) == null) return false;
    }
    else if ((ins = getUrlIn (filename)) == null) return false;
    
    //See explanation on java io bug
    try {(new DataInputStream (new BufferedInputStream (ins, n))).readFully (block);}
    catch (EOFException x) {
      new ErrorDiag (null, x.getMessage ()); close (ins, filename); return false;
    }
    catch (IOException x) {
      new ErrorDiag (null, "IOException while reading logfile"); close (ins, filename); 
      return false;
    }
        
    DataInputStream in = new DataInputStream (new ByteArrayInputStream (block));
    
    CLOG_HEADER hdr = new CLOG_HEADER ();
    hdr.readBigEnd (in);
    int rtype = hdr.rectype;
    switch (rtype) {
    case CONST.CLOG_STATEDEF: case CONST.CLOG_RAWEVENT:
    case CONST.CLOG_SRCLOC: case CONST.CLOG_COMMEVENT:
    case CONST.CLOG_MSGEVENT: case CONST.CLOG_COLLEVENT:
    case CONST.CLOG_EVENTDEF: case CONST.CLOG_ENDBLOCK:
    case CONST.CLOG_ENDLOG: swap = false; break;
    default:
      JOptionPane.showMessageDialog (null, "Unrecognized record type:" + rtype + 
				     ". Maybe the file is little endian", 
				     "Unrecognized record type",
				     JOptionPane.INFORMATION_MESSAGE);
      swap = true; 
    }
    
    try {ins.close ();}
    catch (IOException x) {
      new ErrorDiag (null, "logfile:" + filename + " cannot be closed.");
      return false;
    }
    
    if (Thread.interrupted () || cancel) return false; Thread.yield ();
    
    return true;
  }
  
  /**
   * reads CLOG_STATE records and skips over everything else
   */
  private boolean getStateDefs (String filename, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, retval;
    InputStream in;
    
    if (!aplt) {
      if ((in = getFileIn (filename)) == null) return false;
    }
    else if ((in = getUrlIn (filename)) == null) return false;
    
    BufferedInputStream inFile = new BufferedInputStream (in, n);
    
    numEvt1 = 0; numrawevt = 0;
    for (retval = readStates (inFile, mainTool);
         retval == n;
         retval = readStates (inFile, mainTool)) {
      updateStatus (numEvt1 % 100); 
      if (Thread.interrupted () || cancel) {close (inFile, filename); return false;}
      Thread.yield ();
    }
    
    try {inFile.close ();} 
    catch (IOException y) {
      new ErrorDiag (null, "logfile:" + filename + " cannot be closed");
      return false;
    }
    
    if (retval != 0) return false;
    
    if (mainTool.stateDefs.size () < 1) {
      new ErrorDiag (null, "No states specified in logfile. Exiting read");
      return false;
    }
    return true;
  }

  private int readStates (BufferedInputStream inFile, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, bytesRead = n;
    byte[] block = new byte [n];

    try {(new DataInputStream (inFile)).readFully (block);}
    catch (EOFException x) {return 0;}
    catch (IOException x) {
      new ErrorDiag (null, "IOException occurred while reading logfile.");
      return -1;
    }
    
    DataInputStream in = new DataInputStream (new ByteArrayInputStream (block));
    
    int rtype;
    CLOG_HEADER hdr = new CLOG_HEADER ();
    CLOG_RAW raw = new CLOG_RAW ();

    rtype = CONST.CLOG_UNDEF;
    while (rtype != CONST.CLOG_ENDBLOCK && rtype != CONST.CLOG_ENDLOG) {
      
      if (swap) hdr.readLitEnd (in); else hdr.readBigEnd (in);
      rtype = hdr.rectype;
      numEvt1 ++;
      try {
        switch (rtype){
        case CONST.CLOG_STATEDEF:
          CLOG_STATE state = new CLOG_STATE();
          if (swap) state.readLitEnd (in); else state.readBigEnd (in);

          mainTool.HandleStateDef (state);
          break;    
        case CONST.CLOG_RAWEVENT:
	  numrawevt ++;
          in.skipBytes (CLOG_RAW.getSize ()); break;   
        case CONST.CLOG_SRCLOC:
          in.skipBytes (CLOG_SRC.getSize ()); break;
        case CONST.CLOG_COMMEVENT:
          in.skipBytes (CLOG_COMM.getSize ()); break;
        case CONST.CLOG_MSGEVENT:
          in.skipBytes (CLOG_MSG.getSize ()); break;
        case CONST.CLOG_COLLEVENT:
          in.skipBytes (CLOG_COLL.getSize ()); break;
        case CONST.CLOG_EVENTDEF:
          in.skipBytes (CLOG_EVENT.getSize ()); break;
        case CONST.CLOG_ENDBLOCK: break;
        case CONST.CLOG_ENDLOG: break;
          
        default: 
          new ErrorDiag (null, "Unrecognized record type:" + rtype);
	  return -1;
        }
      }
      catch (IOException x) {
        new ErrorDiag (null, "IOException occured while reading logfile");
        return -1;
      }
    }

    return bytesRead;
  }
  
  /**
   * reads only RAW records
   */
  private boolean getRawEvents (String filename, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, retval;
    InputStream in;
    
    if (!aplt) {
      if ((in = getFileIn (filename)) == null) return false;
    }
    else if ((in = getUrlIn (filename)) == null) return false;

    BufferedInputStream inFile = new BufferedInputStream (in, n);
    
    firsttime = 0; numEvt2 = 0; rawevtno = 0;
    for (retval = readRaw (inFile, mainTool);
         retval == n;
         retval = readRaw (inFile, mainTool)) {
      int a = (int)((numEvt2 * 100) / numEvt1);
      updateStatus (a); updateStatField (a);
      if (Thread.interrupted () || cancel) {close (inFile, filename); return false;}
      Thread.yield ();
    }
    
    try {inFile.close ();} 
    catch (IOException y) {
      new ErrorDiag (null, "logfile:" + filename + " cannot be closed." + 
                     " Proceeding anyway.");
    }
    
    if (retval != 0) return false;
    
    return true;
  }
  
  private int readRaw (BufferedInputStream inFile, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, bytesRead = n;
    byte[] block = new byte [n];
    
    try {(new DataInputStream (inFile)).readFully (block);}
    catch (EOFException x) {return 0;}
    catch (IOException x) {
      new ErrorDiag (null, "IOException occured while reading logfile.");
      return -1;
    }
       
    DataInputStream in = new DataInputStream (new ByteArrayInputStream (block));
    
    int rtype;
    CLOG_HEADER hdr = new CLOG_HEADER ();
    CLOG_RAW raw = new CLOG_RAW ();

    rtype = CONST.CLOG_UNDEF;
    while (rtype != CONST.CLOG_ENDBLOCK && rtype != CONST.CLOG_ENDLOG) {
      if (swap) hdr.readLitEnd (in); else hdr.readBigEnd (in);
      rtype = hdr.rectype;
      numEvt2++;
      
      try {
        switch (rtype){
          
        case CONST.CLOG_RAWEVENT:
          if (rawevtno++ == 0) firsttime = hdr.timestamp;
          if (swap) raw.readLitEnd (in); else raw.readBigEnd (in);
          hdr.timestamp = hdr.timestamp - firsttime;

          mainTool.HandleRaw (new RECORD (hdr, raw));
          break;
          
        case CONST.CLOG_STATEDEF:
          in.skipBytes (CLOG_STATE.getSize ()); break;
        case CONST.CLOG_SRCLOC:
          in.skipBytes (CLOG_SRC.getSize ()); break;
        case CONST.CLOG_COMMEVENT:
          in.skipBytes (CLOG_COMM.getSize ()); break;
        case CONST.CLOG_MSGEVENT:
          in.skipBytes (CLOG_MSG.getSize ()); break;
        case CONST.CLOG_COLLEVENT:
          in.skipBytes (CLOG_COLL.getSize ()); break;
        case CONST.CLOG_EVENTDEF:
          in.skipBytes (CLOG_EVENT.getSize ()); break;
        case CONST.CLOG_ENDBLOCK: break;
        case CONST.CLOG_ENDLOG: break;
          
        default: 
          new ErrorDiag (null, "Unrecognized record type:" + rtype);
	  return -1;
        }
      }
      catch (IOException x) {
        new ErrorDiag (null, "IOException occured while reading logfile");
        return -1;
      }
    }
    return bytesRead;
  }

  /**
   * reads both RAW and CLOG_STATE records and assumes that CLOG_STATE
   * records will occur before RAW.
   * If this method is used the progress bar will not show progress relative to
   * the size of the file since there is no way of knowing the number of events 
   * in the file during a single pass
   */
  private boolean getData (String filename, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, retval;
    InputStream in;
    
    if (!aplt) {
      if ((in = getFileIn (filename)) == null) return false;
    }
    else if ((in = getUrlIn (filename)) == null) return false;
    
    BufferedInputStream inFile = new BufferedInputStream (in, n);
    
    firsttime = 0; rawevtno = 0; numEvt1 = 0;
    for (retval = readBlock (inFile, mainTool);
         retval == n;
         retval = readBlock (inFile, mainTool)) {
      updateStatus (numEvt1 % 100); 
      if (Thread.interrupted () || cancel) {close (inFile, filename); return false;} 
      Thread.yield ();
    }

    try {inFile.close ();} 
    catch (IOException y) {
      new ErrorDiag (null, "logfile:" + filename + " cannot be closed." + 
                     " Proceeding anyway.");
    }
    
    return true;
  }

  private int readBlock (BufferedInputStream inFile, RecordHandler mainTool) {
    int n = CONST.CLOG_BLOCK_SIZE, bytesRead = 0;
    byte[] block = new byte [n];
    
    try {(new DataInputStream (inFile)).readFully (block);}
    catch (EOFException x) {return 0;}
    catch (IOException x) {
      new ErrorDiag (null, "IOException occured while reading logfile.");
      return -1;
    }
       
    DataInputStream in = new DataInputStream (new ByteArrayInputStream (block));
    
    int rtype;
    CLOG_HEADER hdr = new CLOG_HEADER ();
    CLOG_RAW raw = new CLOG_RAW ();

    rtype = CONST.CLOG_UNDEF;
    while (rtype != CONST.CLOG_ENDBLOCK && rtype != CONST.CLOG_ENDLOG) {
      
      if (swap) hdr.readLitEnd (in); else hdr.readBigEnd (in);
      rtype = hdr.rectype;
      numEvt1++;
      try {
        switch (rtype){
          
        case CONST.CLOG_RAWEVENT:
          if (rawevtno++ == 0) firsttime = hdr.timestamp;
          if (swap) raw.readLitEnd (in); else raw.readBigEnd (in);
          hdr.timestamp = hdr.timestamp - firsttime;
          mainTool.HandleRaw (new RECORD (hdr, raw));
          break;
          
        case CONST.CLOG_STATEDEF:
          CLOG_STATE state = new CLOG_STATE();
          if (swap) state.readLitEnd (in); else state.readBigEnd (in);
          mainTool.HandleStateDef (state);
          break;    
          
        case CONST.CLOG_SRCLOC:
          in.skipBytes (CLOG_SRC.getSize ()); break;
        case CONST.CLOG_COMMEVENT:
          in.skipBytes (CLOG_COMM.getSize ()); break;
        case CONST.CLOG_MSGEVENT:
          in.skipBytes (CLOG_MSG.getSize ()); break;
        case CONST.CLOG_COLLEVENT:
          in.skipBytes (CLOG_COLL.getSize ()); break;
        case CONST.CLOG_EVENTDEF:
          in.skipBytes (CLOG_EVENT.getSize ()); break;
        case CONST.CLOG_ENDBLOCK: break;
        case CONST.CLOG_ENDLOG: break;
          
        default: 
          new ErrorDiag (null, "Unrecognized record type:" + rtype);
	  return -1;
        }
      }catch (IOException x) {
        new ErrorDiag (null, "IOException occured while reading logfile.");
        return -1;
      }
    }
    return bytesRead;
  }

  private InputStream getFileIn (String name) {
    InputStream in = null;
    
    try {in = new FileInputStream (name);}
    catch (FileNotFoundException e) {
      new ErrorDiag (null, "file: " + name + " not found.");
      return null;
    }
    return in;
  }
  
  private InputStream getUrlIn (String name) {
    URL url = null;
    InputStream in = null;
    
    try {url = new URL (name);}
    catch (MalformedURLException e) {
      new ErrorDiag (null, "Bad URL:" + url);
      return null;
    }
   
    try {in = url.openStream ();}
    catch (IOException e) {
      new ErrorDiag (null, "IO Error:" + e.getMessage ());
      return null;
    }
    return in;
  }
  
  /**
   * When the worker needs to update the GUI we do so by queuing
   * a Runnable for the event dispatching thread with 
   * SwingUtilities.invokeLater().  In this case we're just
   * changing the progress bars value.
   */
  private void updateStatus(final int i) {
    Runnable doSetProgressBarValue = new Runnable() {
            public void run() {
	      progressBar.setValue(i);
	      
            }
    };
    SwingUtilities.invokeLater(doSetProgressBarValue);
  }
  
  /**
   * In this case we're just updating status field.
   */
  private void updateStatField (final int i) {
    Runnable setField = new Runnable() {
      public void run() {
	statusField.setText ("Making second pass over file: " + i + "% done");
      }
    };
    SwingUtilities.invokeLater(setField);
  }
  
  /**
   * This is an action listener, called by the "Cancel" button
   */
  private ActionListener interruptListener = new ActionListener() {
    public void actionPerformed(ActionEvent event) {cancel = true;}
  };
  
  /**
   * Close InputStream
   */
  private void close (InputStream in, String filename) {
    try {in.close ();}
    catch (IOException x) {
      new ErrorDiag (null, "logfile:" + filename + " cannot be closed"); 
    }
  }

  protected void finalize () throws Throwable {super.finalize ();}
}
