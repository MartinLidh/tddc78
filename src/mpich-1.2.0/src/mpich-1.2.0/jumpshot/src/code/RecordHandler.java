import java.util.Vector;
import java.util.Random;
import java.util.Enumeration;

/**
 * This class is responsible for building data structures from records
 *  of the clog file. Each record read by ClogReader is passed here and
 * RecordHandler sifts out the relevent data from it.
 * 
 * Once the clog file is read, 4 data structures exist.
 * 
 * 1. (stateDefs) This is a vector of all the state definitions in the 
 *    order they were read. 
 * 2. Each of the state definition objects mentioned above contains a vector
 *    'stateVector' that is a collection of states of the same type
 *    in the ascending order of their end timestamps.
 * 3. (data) This is vector or all states of all types in ascending order 
 *    of their end timestamps.
 * 4. (a) This is an object that contains a vector 'arrows' which is a 
 *    list of ArrowInfo objects representing messages.
 */

public class RecordHandler {
  Vector stateDefs;
  Vector data;
  Vector mtns;
  CLOG_ARROW a;
  MtnInfo m;
  private int evtct;

  /**
   * Constructor
   */
  public RecordHandler () {
    stateDefs = new Vector ();
    data = new Vector ();
    mtns = new Vector ();
    evtct = 0;
    setupArrows ();
  }
  
  private void setupArrows () {
    a = new CLOG_ARROW ();
    a.startetype = CONST.LOG_MESG_SEND;
    a.endetype = CONST.LOG_MESG_RECV;
  }
   
  void setDataSize (int n) {
    if ((n % 2) > 0) { 
      System.err.println ("Error Reading logfile. Incorrect number of rawevents");
      n ++;
    }
    
    data.ensureCapacity (n / 2);
  }
  
  /**
   * Get information from a RAW record and its header
   */
  void HandleRaw (RECORD rec) {
    CLOG_HEADER header = (CLOG_HEADER)(rec.header);
    
    CLOG_RAW raw = (CLOG_RAW)(rec.data);
    int etype = raw.etype;
    
    if (etype == a.startetype) a.startArrowEvent (header, raw);
    else if (etype == a.endetype) a.endArrowEvent (header, raw);
    else {
      CLOG_STATE state = resolve (etype);
      if (state == null) return;
      //Do poll here for the mountains
      if (evtct++ == 0) {
	m = new MtnInfo (stateDefs.size ());
	m.begT = header.timestamp;
      }
      else {
	m.endT = header.timestamp;
	//	System.out.print (m.toString ());
	for (int i = 0; i < stateDefs.size (); i++) {
	  CLOG_STATE s = (CLOG_STATE)stateDefs.elementAt (i);
	  m.s [i] += s.events.size ();
	  //  if (s.stateid == state.stateid && s.endetype == etype)
	  //   m.s [i] ++;
	  //	  System.out.print ("|" + m.s[i] + "|");
	}
	//	System.out.println ("");
	mtns.addElement (m);
	m = new MtnInfo (stateDefs.size ());
	m.begT = header.timestamp;
      }
      
      if (state.startetype == etype) state.startEvent (header);
      else data.addElement (state.endEvent (header));
    }
  }
  
  /**
   * Get information from a CLOG_STATE record
   */
  void HandleStateDef (CLOG_STATE state) {stateDefs.addElement (state);}
  
  /**
   * This function traverses through the vector containing state definitions
   * looking for the state definition whose starting or ending 
   * etype corresponds with the given etype. If no such state def. is found
   * a null is returned.
   */
  private CLOG_STATE resolve (int etype) {
    Enumeration enum = stateDefs.elements ();
    while (enum.hasMoreElements ()) {
      CLOG_STATE s = (CLOG_STATE)(enum.nextElement ());
      if (etype == s.startetype || etype == s.endetype) 
        return s;
    }
    System.out.println ("Error: Unrecognized etype in RAW record");
    return null;
  }

  protected void finalize () throws Throwable {super.finalize ();}
}
