import java.io.*;
import java.awt.*;
import java.util.*;
import com.sun.java.swing.*;
/**
 * This file is an agglomeration of all classes representing the different record
 * types in clog. In this Jumpshot application we actually read only CLOG_HEADER, 
 * CLOG_STATE, CLOG_RAW, CLOG_CNAME and CLOG_DESC records. All other records are
 * skipped over.
 *
 * The function getSize () in all these classes is very important as it 
 * determines the number of bytes that should be skipped.
 */


/**
 * This is a static class which stores all global constants.
 */
class CONST {
  /*  public static final int CLOG_BLOCK_SIZE = 1024;  */
  public static final int CLOG_BLOCK_SIZE = 65536;
  public static final int CLOG_ENDLOG   = -2;
  public static final int CLOG_ENDBLOCK = -1;
  public static final int CLOG_UNDEF    =  0;
  public static final int CLOG_RAWEVENT =  1;
  public static final int CLOG_MSGEVENT =  2;
  public static final int CLOG_COLLEVENT=  3;
  public static final int CLOG_COMMEVENT=  4;
  public static final int CLOG_EVENTDEF =  5;
  public static final int CLOG_STATEDEF =  6;
  public static final int CLOG_SRCLOC   =  7;
  public static final int CLOG_SHIFT    =  8;
  public static final int LOG_MESG_SEND = -101;
  public static final int LOG_MESG_RECV = -102;
  public static final int TIMELINES = 0;
  public static final int MTN_RANGES = 1;
  public static final String DSTR [] = {"Time Lines", "Mountain Ranges"};
}
//-----------------------------------------------------------------------------------
//This class stores information about a message
class ArrowInfo  extends Info{
  int begProcId;
  int endProcId;
  int tag;
  int size;
  
  //Constructor for the object representing a message.
  //bid = Process Id of the process where the message originates
  //eid = Process Id of the process where the message ends.
  //beg = The time at which the message originates.
  //t = Value of tag contained in message.
  public ArrowInfo (int bid, int eid, double beg, double end, int t, int s) {
    super ();
    begProcId = bid; endProcId = eid; begT = beg; endT = end; tag = t; size = s;
  }
  
  public String toString () {
    return ("ArrowInfo [begProcId=" + begProcId + ", endProcId=" + endProcId +
	    ", begT=" + begT + ", endT=" + endT + ", tag=" + tag + ", size=" +
	    size + "]");
  }
} 
//--------------------------------------------------------------------------------
//This object is a repository for all ArrowInfo objects which represent messages
class CLOG_ARROW {
  Vector sends;         //Temporary vector to store incomplete ArrowInfo objects
                         //arising from unmatched RAW events
  Vector recvs;
  Vector arrowVector;         //Vector that stores completed ArrowInfo objects
  int startetype;        //startetype identifying the start of a message
  int endetype;          //endetype identifying the end of a message
    
  //Constructor
  public CLOG_ARROW () {
    sends = new Vector ();
    recvs = new Vector ();
    arrowVector = new Vector ();
  } 
  
  //This function takes a header associated with a RAW record whose
  //etype is of the start type for arrows. An arrowInfo object is created and
  //put in a temporary vector until its corresponding RAW record
  //with end etype is read.  
  void startArrowEvent (CLOG_HEADER header, CLOG_RAW raw) {
    if (recvs.size () > 0) {
      Enumeration enum = recvs.elements ();
      ArrowInfo  arrow = null;
      boolean found = false;
      
      while (enum.hasMoreElements () && !found) {
	arrow = (ArrowInfo)enum.nextElement ();
	if (header.procid == arrow.begProcId && raw.getTag () == arrow.tag &&
	    raw.data == arrow.endProcId) found = true;
      }
      
      if (found) {
	recvs.removeElement (arrow);
	arrowVector.addElement (arrow);
	arrow.begT = header.timestamp;
	return;
      }
    }
    ArrowInfo arrow = new ArrowInfo (header.procid, raw.data, header.timestamp, 
				     0, raw.getTag (), raw.getMsgSize ()); 
    sends.addElement (arrow);
  }
  
  //Once a RAW record with a end etype is read, its corresponding RAW
  //record with the start etype is assumed to have been read earlier.
  //That RAW record is searched for in the temporary vector. When found
  //the ArrowInfo object is removed from the temporary vector and we
  //have a completed arrow that we add to arrows
  void endArrowEvent (CLOG_HEADER header, CLOG_RAW raw) {
    if (sends.size () > 0) {
      Enumeration enum = sends.elements ();
      ArrowInfo  arrow = null;
      boolean found = false;
      
      while (enum.hasMoreElements () && !found) {
	arrow = (ArrowInfo)enum.nextElement ();
	if (header.procid == arrow.endProcId && raw.getTag () == arrow.tag &&
	    raw.data == arrow.begProcId) found = true;
      }
      
      if (found) {
	sends.removeElement (arrow);
	arrowVector.addElement (arrow);
	arrow.endT = header.timestamp;
	return;
      }
    }
    
    ArrowInfo arrow = new ArrowInfo (raw.data, header.procid, 0,
				     header.timestamp, 
				     raw.getTag (), raw.getMsgSize ()); 
    recvs.addElement (arrow);
  }
}
//-----------------------------------------------------------------------------------
class Info {
  double begT;
  double endT;
  double lenT;
  boolean blink = false;

  public Info () {}
}

//This class represents each state
class StateInfo extends Info { 
  int level = 0;           //For nested states
  StateInfo higher = null; //Pointer to the next higher state enveloping this.

  CLOG_STATE stateDef;
  int procId;
  
  //Constructor 
  public StateInfo () {super ();};
  
  //Constructor
  public StateInfo (int pid, double beg, double end, CLOG_STATE s) {
    super ();
    procId = pid;
    begT = beg;
    endT = end;
    stateDef = s;
  } 
  
  public String toString () {
    return  ("StateInfo [level=" + level + ", procId=" + procId + 
             ", begT=" + begT + ", endT=" + endT + ", lenT=" + lenT +
             ", name=" + stateDef.description.desc +
             "]");
  }
}
//-------------------------------------------------------------------------------
//Record just keeps together a RAW record and its associated header
class RECORD {
  CLOG_HEADER header;
  Object data;
  
  //Constructor
  public RECORD (CLOG_HEADER header, Object data) {
    this.header = header;
    this.data = data;
  }
}
//----------------------------------------------------------------------------------
class CLOG_HEADER {
  static final int size = (1 * 8) + (4 * 4);
  double timestamp;
  int rectype;
  int length;	
  int procid;			// currently rank in COMM_WORLD
  int pad;
  
  //read the record from the givne input stream
  void readBigEnd (DataInputStream in) {
    try {
      timestamp = in.readDouble ();  
      rectype = in.readInt ();
      length = in.readInt ();
      procid = in.readInt ();
      pad = in.readInt ();
    }
    catch (IOException x) {
      System.out.println ("IOException in CLOG_HEADER.read");
      return;
    }
  }
  
  //read the record from the givne input stream
  void readLitEnd (DataInputStream in) {
    try {
      timestamp = FUNCTS.swapDouble (in);  
      rectype = FUNCTS.swapInt (in);
      length = FUNCTS.swapInt (in);
      procid = FUNCTS.swapInt (in);
      pad = in.readInt ();
    }
    catch (IOException x) {
      System.out.println ("IOException in CLOG_HEADER.read");
      return;
    }
  }
  
  //Returns the number of bytes this record occupies
  public static int getSize () {return size;};

  //Copy Constructor
  CLOG_HEADER Copy () {
    CLOG_HEADER cp = new CLOG_HEADER ();
    cp.timestamp = this.timestamp;
    cp.rectype = this.rectype;
    cp.length = this.length;
    cp.procid = this.procid;
    cp.pad = this.pad;
    return cp;
  }
  
  public String toString () {
    return ("CLOG_HEADER [timestamp=" + timestamp + ", rectype=" + rectype + 
            ", length=" + length + ", procid=" + procid + ", pad =" + pad + 
            ", size=" + size + "]");
  } 
}
//-----------------------------------------------------------------------------------
//class representing a RAW record
class CLOG_RAW {
  final static int size = (4 * 4) + CLOG_DESC.getSize ();
  int etype;			// raw event
  int data;			// uninterpreted data
  int srcloc;			// id of source location
  int pad;
  CLOG_DESC string = new CLOG_DESC ();   	// uninterpreted string
 
  //read the record from the given input stream
  void readBigEnd (DataInputStream in) {
    try {
      etype = in.readInt ();
      data = in.readInt ();
      srcloc = in.readInt ();
      pad = in.readInt ();
      string.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException has occurred in CLOG_RAW.");
      return;
    }
  }
  
  //read the record from the given input stream
  void readLitEnd (DataInputStream in) {
    try {
      etype = FUNCTS.swapInt (in);
      data = FUNCTS.swapInt (in);
      srcloc = FUNCTS.swapInt (in);
      pad = in.readInt ();
      string.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException has occurred in CLOG_RAW.");
      return;
    }
  }
  
  //Returns the number of bytes the record occupies
  public static int getSize () {return size;}

  //Returns tag specified in string field. This is pertinent when
  //RAW record represents a start or end of a message arrow
  //If tag cannot be parsed a 0 is returned
  int getTag () {
    int i = string.desc.indexOf (' '); if (i < 1) return 0;
    String s = string.desc.substring (0, i);
    int j;
    try {j = (Integer.valueOf (s)).intValue ();}
    catch (NumberFormatException x) {return 0;}
    return j;
  }
  
  //Returns message size specified in string field. This is pertinent
  //when RAW record represents a start or end of a message arrow
  //If message cannot be parsed a 0 is returned
  int getMsgSize () {
    int i = string.desc.indexOf (' '); if (i < 0) i = 0;
    if (i > (string.desc.length () - 2)) return 0;
    String s = string.desc.substring (i + 1);
    int j;
    try {j = (Integer.valueOf (s)).intValue ();}
    catch (NumberFormatException x) {return 0;}
    return j;
  }
  
  //Copy Constructor
  CLOG_RAW Copy () {
    CLOG_RAW cp = new CLOG_RAW ();
    cp.etype = this.etype;
    cp.data = this.data;
    cp.srcloc = this.srcloc;
    cp.pad = this.pad;
    cp.string = this.string;
    return cp;
  }

  public String toString () {
    return ("CLOG_RAW [etype=" + etype + ", data=" + data + ", srcloc=" + 
            srcloc + ", string=" + string.desc + ", pad=" + pad + 
            ", size=" + size + "]");
  }
}
//--------------------------------------------------------------------------------
//This class represents a state definition
class CLOG_STATE {
  final static int size = (4 * 4) + CLOG_DESC.getSize () + CLOG_CNAME.getSize ();
  int stateid;		// integer identifier for state
  int startetype;	// starting event for state 
  int endetype;		// ending event for state 
  int pad;
  
  CLOG_DESC description = new CLOG_DESC ();   //string describing state
  
  Vector events;        // Temporary data structure storing unmatched events
  Vector stateVector;   // Vector storing completed states (paired up events)
  Color color;		// Color given to this state

  //This checkbox determines whether states beloging to this state def., should
  //be displayed or not.
  JCheckBox checkbox;
  

  //Contructor
  public CLOG_STATE () {
    events = new Vector ();
    stateVector = new Vector ();
  }
  
  //read the record from the given input stream
  void readBigEnd (DataInputStream in) {
    try {
      stateid = in.readInt ();
      startetype = in.readInt ();
      endetype = in.readInt ();
      pad = in.readInt ();
      CLOG_CNAME tempColor = new CLOG_CNAME ();
      tempColor.read (in);
      color = COLOR_UTIL.getColor (tempColor.name);
      description.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException in reading CLOG_STATE.");
      return;
    }
  }
  
  //read the record from the given input stream
  void readLitEnd (DataInputStream in) {
    try {
      stateid = FUNCTS.swapInt (in);
      startetype = FUNCTS.swapInt (in);
      endetype = FUNCTS.swapInt (in);
      pad = in.readInt ();
      CLOG_CNAME tempColor = new CLOG_CNAME ();
      tempColor.read (in);
      color = COLOR_UTIL.getColor (tempColor.name);
      description.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException in reading CLOG_STATE.");
      return;
    }
  }
  
  //returns the number of bytes this record occupies
  public static int getSize () {return size;}

  //This function takes a header associated with a RAW record whose
  //etype is of the start type. A stateInfo object is created and
  //put in a temporary vector until its corresponding RAW record
  //with end etype is read.
  void startEvent (CLOG_HEADER header) {
    StateInfo evt = new StateInfo (header.procid, header.timestamp, 0, this);
    events.addElement (evt);
  }
  
  //Once a RAW record with a end etype is read, its corresponding RAW
  //record with the start etype is assumed to have been read earlier.
  //That RAW record is searched for in the temporary vector. When found
  //the StateInfo object is removed from the temporary vector and we
  //have a completed state that we add to stateVector
  StateInfo endEvent (CLOG_HEADER header) {
    //The assumption here is that events of the same state and
    //same procids will not overlap. 
    Enumeration enum = events.elements ();
    StateInfo evt = null;
    while (enum.hasMoreElements ()) {
      evt = (StateInfo)enum.nextElement ();
      if (header.procid == evt.procId) break;
    }
    events.removeElement (evt);
    evt.endT = header.timestamp;
    evt.lenT = evt.endT - evt.begT;
    stateVector.addElement (evt);
    return evt;
  }

  public String toString () { 
    return ("CLOG_STATE: [stateid=" + stateid + ", startetype=" + startetype +
            ", endetype=" + endetype + ", color=" + color.toString () +
            ", description=" + description.desc + ", pad=" + pad + 
            ", size=" + size + "]");
  }
}
//-----------------------------------------------------------------------------------

class CLOG_DESC {
  static final int size = (2 * 8);
  String desc;
  
  //Constructor
  public CLOG_DESC (){desc = new String ();}
  
  //read the record
  void read (DataInputStream in) {
    byte []b = new byte [2 * 8];
    try {
      in.readFully (b);
    }
    catch (IOException x) {
      System.out.println ("IOException in reading CLOG_DESC.");
      return;
    }
    //Here we use platform's default character encoding. 
    //Its probably from ascii but, don't know whether 
    //will work always."INVESTIGATE!"
    desc = (new String (b, 0, 2*8)).trim ();
  }

  public static int getSize () {return size;}
  
  public String toString () {
    return ("CLOG_DESC [desc=" + desc + ", size=" + size + "]");
  }
}
//---------------------------------------------------------------------------------
class CLOG_CNAME {
  static final int size = (3 * 8);
  String name;
  
  //Constructor
  public CLOG_CNAME () {name = new String ();}
  
  //read the record
  void read (DataInputStream in) {
    byte []b = new byte [3 * 8];
    try {
      in.readFully (b);
    }
    catch (IOException x) {
      System.out.println ("IOException in CLOG_CNAME.read"); 
      return;
    }
    //Here we use platform's default character encoding. 
    //Its probably from ascii but, don't know whether 
    //will work always."INVESTIGATE!"
    name = (new String (b, 0, 3 * 8)).trim ();
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_CNAME [name=" + name + ", size=" + size + "]");
  }
}
//----------------------------------------------------------------------------------
class CLOG_MSG {
  static final int Size = (6 * 4);
  int etype;			// kind of message event 
  int tag;			// message tag 
  int partner;		        // source or destination in send/recv
  int comm;			// communicator
  int size;			// length in bytes 
  int srcloc;			// id of source location
  
  void read (DataInputStream in) {
    try {
      etype = in.readInt ();
      tag = in.readInt ();
      partner = in.readInt ();
      comm = in.readInt ();
      size = in.readInt ();
      srcloc = in.readInt ();
    }
    catch (IOException x) {
	System.out.println ("IOException while reading CLOG_MSG.");
	return;
    }
  }

  public static int getSize () {return Size;}

  public String toString () {
    return ("CLOG_MSG [etype=" + etype + ", tag=" + tag + ", partner=" +  
            partner + ", comm=" + comm + ", size=" + size + 
            ", srcloc=" + srcloc + "Size(in Bytes)=" + Size + "]");
  }
}
//-----------------------------------------------------------------------------------
class CLOG_COLL {
  static final int Size = (6 * 4);
  int etype;			// type of collective event 
  int root;			// root of collective op 
  int comm;			// communicator
  int size;			// length in bytes 
  int srcloc;			// id of source location 
  int pad;
 
  void read (DataInputStream in) {
    try {
      etype = in.readInt ();
      root = in.readInt ();
      comm = in.readInt ();
      size = in.readInt ();
      srcloc = in.readInt ();
      pad = in.readInt ();
      
      }
    catch (IOException x) {
      System.out.println ("IOException while reading CLOG_COLL.");
      return;
    }
  }
  
  public static int getSize () {return Size;}

  public String toString () {
    return ("CLOG_COLL [etype=" + etype + ", root=" + root + 
            ", comm=" +  comm + ", size=" + size + ", srcloc=" + srcloc +
            ", pad=" + pad + ", Size(in Bytes)=" + Size + "]");
  }
}
//----------------------------------------------------------------------------------
class CLOG_COMM {
  static final int size = (4 * 4);
  int etype;			// type of communicator creation
  int parent;			// parent communicator
  int newcomm;		        // new communicator
  int srcloc;			// id of source location

  void read (DataInputStream in) {
    try {
      etype = in.readInt ();
      parent = in.readInt ();
      newcomm = in.readInt ();
      srcloc = in.readInt ();
    }
    catch (IOException x) {
      System.out.println ("IOException in reading CLOG_COMM.");
      return;
    }
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_COMM [etype=" + etype + ", pt=" + parent + 
            ", newcomm=" +  newcomm + " srcloc=" + srcloc + 
            ", size" + size + "]");
  }
}  
//---------------------------------------------------------------------------------
class CLOG_EVENT {
  static final int size = (2 * 4) + CLOG_DESC.getSize ();
  int etype;			            // event
  int pad;                                  // pad 
  CLOG_DESC description = new CLOG_DESC (); // string describing event
  
  void read (DataInputStream in) {
    try {
      etype = in.readInt ();
      pad = in.readInt ();
      description.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException in reading CLOG_EVENT.");
      return;
    }
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_EVENT [etype=" + etype + ", desc=" + description.desc + 
            ", pad=" + pad + ", size=" + size + "]");
  }
}
//---------------------------------------------------------------------------------
class CLOG_FILE {
  static final int size = (5 * 8);
  String file;

  public CLOG_FILE (){file  = new String ();}

  void read (DataInputStream in) {
    byte []b = new byte [5 * 8];
    try {
      in.readFully (b);
    }
    catch (IOException x) {
      System.out.println ("IOException in CLOG_FILE.read");
      return;
    }
    //Here we use platform's default character encoding. 
    //Its probably from ascii but, don't know whether 
    //will work always."INVESTIGATE!"
    file = (new String (b, 0, 5 * 8)).trim ();  
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_FILE [file=" + file + ", size=" + size + "]");}
}
//--------------------------------------------------------------------------------
class CLOG_SRC {
  static final int size = (2 * 4) + CLOG_FILE.getSize (); 
  int srcloc;			         // id of source location
  int lineno;			         // line number in source file
  CLOG_FILE filename = new CLOG_FILE (); // source file of log statement
  
  void read (DataInputStream in) {
    try {
      srcloc = in.readInt ();
      lineno = in.readInt ();
      filename.read (in);
    }
    catch (IOException x) {
      System.out.println ("IOException has occurred in reading CLOG_SRC");
    }
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_SRC [srcloc=" + srcloc + ", lineno=" + lineno +
            ", file=" + filename.file + ", size=" + size + "]");
  }
}
//---------------------------------------------------------------------------------
class CLOG_TSHIFT {
  static final int size = (1 * 8);
  double timeshift;		// time shift for this process 
  
  void read (DataInputStream in) {
    try {
      timeshift = in.readDouble ();
      System.out.print (" shift=" + (new Float (timeshift)).toString()); 
    }
    catch (IOException x) {
	System.out.println ("IOException has occurred in TSHIFT");
	return;
    }
  }
  
  public static int getSize () {return size;}

  public String toString () {
    return ("CLOG_TSHIFT [timeshift=" + timeshift+ ", size=" + size + "]");
  }
}

//---------------------------------------------------------------------------------
class FUNCTS {
  public static int swapInt (DataInputStream in)
       throws IOException
  {
    byte [] b = new byte [4];
    byte t;
    in.read (b);
    t = b [0]; b [0] = b [3]; b [3] = t;
    t = b [1]; b [1] = b [2]; b [2] = t;
    return (new DataInputStream (new ByteArrayInputStream (b))).readInt ();
  }
  
  //This method may be not be producing correct results in some cases
  //So the above method is used. However this may be faster if fixed??
  public static int swapInt (int i) {
    int a = (i & 0xFF000000) >> 24;
    int b = (i & 0x00FF0000) >> 8;
    int c = (i & 0x0000FF00) << 8;
    int d = (i & 0x000000FF) << 24;
    return (a | b | c | d);
  }
  
  public static double swapDouble (DataInputStream in)
       throws IOException
  {
    byte [] b = new byte [8];
    byte t;
    in.read (b);
    t = b [0]; b [0] = b [7]; b [7] = t;
    t = b [1]; b [1] = b [6]; b [6] = t;
    t = b [2]; b [2] = b [5]; b [5] = t;
    t = b [3]; b [3] = b [4]; b [4] = t;
    return (new DataInputStream (new ByteArrayInputStream (b))).readDouble ();
  }
}
//-------------------------------------------------------------------------------
class MtnInfo { 
  double begT;
  double endT;
  int []s;
  
  //Constructor 
  public MtnInfo (int n) {
    s = new int [n];
  };
  
  public String toString () {
    return  ("MtnInfo [begT=" + begT + ", endT=" + endT
	     + ", number = " + s.length + "]");
  }
}
