import java.awt.*;
import java.awt.image.*;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.event.*;
import java.lang.Math;
import com.sun.java.swing.*;

/*
  This class is the canvas on which all drawing of Jumpshot data is done
  A new data structure containing a vector of JProcess objects resides here.
  Each of these JProcess objects represent a process and contain a vector
  'stateVector' which is a list of all states of that process, in ascending order of 
  their end timestamps. 
  
  Double Buffering is used for smooth scrolling. For an explanation see the documents.
*/

public class ProgramCanvas extends JComponent 
implements ComponentListener {
  Graphics currG;
  public ClogDisplay parent;
  boolean blink = false;
  boolean blFlag = true;
  
  //Data Structures
  
  //Vector containing all displayed processes including those whose
  //dispStatus is turned off and which are not deleted.
  Vector procVector;    
  Vector dprocVector;    //Vector containing all deleted processes
  Vector dtypeV;
  
  double xDensity;              //pixels / second
  double yDensity;              //pixels / process vertical thickness
  int _xPix;                    //width of each image
  int _yPix;                    //height of each image
  int widthCan;                 //height of the viewport 
  int heightCan;                //width of the viewport
  
  double maxT;                  //Maximum time in the display
  double tMaxT;                 //end time of the last state in all processes
  
  int maxH;                     //horizontal pixel limit on display
  //horizontal distance between the start of the viewport and the zoom lock line
  int zDist;

  int panePosX;                 //horizontal scroll postion of ScrollPane container
  int sbPos;             
  int cursorPos;                //Horizontal cursor position in this canvas
  int tbegH;                    //previous horizontal scrollbar value
  boolean fflagH, bflagH;       //flags describing forward or backward scrolling (horizontally) 
  
  public double zoomH;          //double value describing the zoom
  double zF;                    //zoom Factor
  
  double begTime;               //time at start of the viewport
  double endTime;               //time at the end of the viewport
  double currTime;              //time at the current cursor position
  double zXTime;                //time at which the zoom lock line was set
  double elapsedPoint;          //time at which the elapsed time line was set
  
  private boolean elTLineDispStatus = true;    //elapsed time line display status
  private boolean zoomLkLineDispStatus = true; //zoom lock line display status
  
  int proct;                    //process count
  int dproct;                   //deleted process count
  int pu = 1;
  int endGap = 5;              //horizontal gap at the end of drawings so that
                               //the drawings towards the right end are not
                               //covered by the ScrollPane Container
  
  MyImage [] img;               //array of 3 offscreen images
  MyImage currImg;              //current image placed in the viewport

  
  int lineSize, rulerHt, hBWt, allDispHt, eachDispHt, eachDispIHt, fDescent;

  public int dpi;               //number of pixels / inch on the screen
  public FontMetrics fm;        //FontMetrics object describing the curren font
 
  int maxLevel = 0;             //maximum level of nesting
  double nestFactor;            //value describing the height difference between levels
  //nestFactor * yDensity will be the difference in height between two consecutive levels
  
  boolean message = false;      //value decribing whether an arrow circle is present ??
  boolean Windows = false;      //catering for the extra scrollbar
  boolean arrowDispStatus = true; 
  public boolean setupComplete; //setup status 
  Timer timer;
  int totalProc;
  
  private int elTimeStrW, zLockStrW;
  
  //Constructor
  public ProgramCanvas () {super ();}
  
  //Setup methods -----------------------------------------------------------
  void init (ClogDisplay p) {
    setupComplete = false;
    parent = p;
    setupData ();
    adjustCanvasStuff ();
    setupStates ();
    //if (parent.dtype == CONST.TIMELINES)
    setupNestedStates ();
    dtypeV = new Vector ();
    dtypeV.addElement (new Integer (parent.dtype));
    parent.optionsDlg.addDisplay (parent.dtype);
    //else setupMtns ();
    totalProc = proct;
    tMaxT = maxT;
    timer = new Timer (200, new TimerListener ());
    Resize ();
    setupEventHandlers ();
    adjustStartEndTimes ();
    setupComplete = true;
  }
  
  private void setupData () {
    proct = 0; dproct = 0;
    procVector = new Vector ();
    dprocVector = new Vector ();
    nestFactor = 0.10;
    zoomH = 1.0;
    zF = 2.0;
    begTime = 0.0; zXTime = 0.0; elapsedPoint = 0.0;
    hBWt = 2;
  }
 
  private void adjustCanvasStuff () {
    Font f = parent.frameFont;
    setFont (f);
    fm = getToolkit ().getFontMetrics (f);
    lineSize = fm.getHeight (); rulerHt = 3 * lineSize;
    zLockStrW = fm.stringWidth ("Zoom Lock");
    elTimeStrW = fm.stringWidth ("Elapsed Time");
    fDescent = fm.getDescent ();
    dpi = getToolkit ().getScreenResolution ();
  }

  private void setupStates () {
    Enumeration enum = parent.data.elements (); 
    while (enum.hasMoreElements ()) {
      StateInfo evt = (StateInfo)enum.nextElement ();
      addState (evt);
    }
    
    enum = procVector.elements ();
    while (enum.hasMoreElements ()) {
      JProcess p = (JProcess)enum.nextElement ();
      p.begT = ((ProcessState)p.procStateVector.firstElement ()).info.begT;
    }
  }

  /*  private void setupMtns () {
    Enumeration enum = parent.mtns.elements (); 
    while (enum.hasMoreElements ()) {
      MtnInfo mtn = (MtnInfo)enum.nextElement ();
      if (mtn.endT > maxT) maxT = mtn.endT; 
    }
  }*/
  
  /**
   * This method goes trough the stateVector for each process and assigns the 
   * appropriate level to each state. If no nesting is present then a level of
   * 0 will be assigned
   */
  private void setupNestedStates () {
    waitCursor ();
    maxLevel = 0;

    Enumeration enum = procVector.elements ();
    while (enum.hasMoreElements ()) {
      //Scan each process
      StateInfo prev = null;
      
      JProcess p = (JProcess)enum.nextElement ();
      Vector a = p.procStateVector;
      
      //For each process assign levels to all states 
      for (int i = a.size () - 1; i >= 0; i--) {
        ProcessState currState = (ProcessState)a.elementAt (i);
        if (currState.info.stateDef.checkbox.isSelected ()) {
          if (prev != null) {
            if (prev.begT <= currState.info.begT) {
              //Entering here should mean that this state is nested inside the 
              //previous state and hence shoud be smaller.
              currState.info.level = prev.level + 1;
              currState.info.higher = prev;
            }
            else {
              //Entering here means that the this state is independent of the previous
              //state. however there may be higher states that contain both this and 
              //the previous state. An EFFICIENT WAY FOR THIS HAS TO BE DETERMINED.
              StateInfo temp = prev.higher;
              currState.info.level = prev.level;
              
              while (temp != null && temp.begT > currState.info.endT) {
                //Means that the current State is definately not containted in temp..
                currState.info.level --;
                temp = temp.higher;
              }
              currState.info.higher = temp;
            }
          }
          prev = currState.info;
        }
        
        if (currState.info.level > maxLevel) maxLevel = currState.info.level;
      }
    }
    normalCursor ();
  }
  
  /**
   * Draw initial view consisting of entire data in viewport
   */
  private void drawInitImg () {
    double b = begTime, e = getTime (_xPix);
    img [0].drawRegion (b, e);
    currImg = img [0];
    panePosX = _xPix;
    repaint ();
  }
  //end of setup methods-----------------------------------------------------------
  
  public void paintComponent (Graphics g) {
    //We request keyboard focus out here so that we can catch events for locking zoom, etc.
    requestFocus ();
    
    //3 Images are painted each time. The reason we use images rather than directly drawing
    //onto the graphics context is that the drawings of one image can be taking place
    //concurrently with the other using threads into offscreen graphic contexts assoc.
    //with the respective images. While printing we do the opposite
    
    if (setupComplete && currImg != null){
      int prevH = getPrev (currImg.imgIdH);
      int nextH = getNext (currImg.imgIdH);
      
      if (blFlag || !timer.isRunning ()) {
	img [prevH].paintStuff1 (0, g, this);
	currImg.paintStuff1 (_xPix, g, this);
	img [nextH].paintStuff1 (_xPix + _xPix, g, this);
	
	/*g.drawImage (img [prevH].img, 0, 0, this);
	g.drawImage (img [prevH].timeimg, 0, _yPix, this);
	
	g.drawImage (currImg.img, _xPix, 0, this);
	g.drawImage (currImg.timeimg, _xPix, _yPix, this);
	
	g.drawImage (img [nextH].img, _xPix + _xPix, 0, this);
	g.drawImage (img [nextH].timeimg, _xPix + _xPix, _yPix, this);*/
      }
      else {
	img [prevH].paintStuff2 (0, g, this);
	currImg.paintStuff2 (_xPix, g, this);
	img [nextH].paintStuff2 (_xPix + _xPix, g, this);
	
	/*g.drawImage (img [prevH].img1, 0, 0, this);
	g.drawImage (img [prevH].timeimg, 0, _yPix, this);
	
	g.drawImage (currImg.img1, _xPix, 0, this);
	g.drawImage (currImg.timeimg, _xPix, _yPix, this);
	
	g.drawImage (img [nextH].img1, _xPix + _xPix, 0, this);
	g.drawImage (img [nextH].timeimg, _xPix + _xPix, _yPix, this);*/
      }
    }
    
    if (zoomLkLineDispStatus && zXTime >= begTime && zXTime <= endTime) {
      int xcord = panePosX + getEvtXCord (zXTime - begTime);
      g.setColor (Color.white);
      g.drawLine (xcord, 0, xcord, _yPix);
      g.fillRect (xcord, _yPix - lineSize, zLockStrW + 2, lineSize);
      g.setColor (Color.black); 
      g.drawRect (xcord, _yPix - lineSize, zLockStrW + 1, lineSize - 1);
      g.setColor (Color.red);
      g.drawString ("Zoom Lock", xcord + 1, _yPix - fDescent);
    }
    
    if (elTLineDispStatus && elapsedPoint >= begTime && elapsedPoint <= endTime) {
      int xcord = panePosX + getEvtXCord (elapsedPoint - begTime);
      g.setColor (Color.white);
      g.drawLine (xcord, 0, xcord, _yPix);
      g.fillRect (xcord, _yPix - lineSize, elTimeStrW + 2, lineSize);
      g.setColor (Color.black); 
      g.drawRect (xcord, _yPix - lineSize, elTimeStrW + 1, lineSize - 1);
      g.setColor (Color.red);
      g.drawString ("Elapsed Time", xcord + 1, _yPix - fDescent);
    }
  }
  
  //methods for handling smooth scrolling, double buffereing------------------------
  
  /**
   * Allocate memory for 3 offscreen images
   */
  private void setupImg () {
    killImgs ();
    setValues ();
    parent.vcanvas1.repaint (); parent.vcanvas2.repaint ();
    
    img = new MyImage [3];
    for (int i = 0; i < 3; i++) {
      img [i] = new MyImage (i, _xPix, _yPix, dtypeV, this);
    }
  }
  
  protected void setValues () {
    int numDisp = dtypeV.size ();
    allDispHt = _yPix - rulerHt;
    eachDispHt = (int)Math.rint (allDispHt / (double)numDisp);
    eachDispIHt = eachDispHt - 2 * hBWt;
    
    if (eachDispHt < 1) eachDispHt = 1; //eachDispHt should be non zero
    
    calcYDensity ();
  }
  
  Image getOffScreenImage (int x, int y) {return createImage (x, y);}
  
  private void killImgs () {
    if (img != null) 
      for (int i = 0; i < 3; i++) if (img [i] != null) {
	img [i].kill (); img [i] = null;
      }
  }
    
  /**
   * Get the appropriate display for the given scroll bar position
   */
  void adjustImgH (int x) {
    MyImage prev = img [getPrev (currImg.imgIdH)];
    MyImage next = img [getNext (currImg.imgIdH)];
    
    int epos1 = getEvtXCord (currImg.begT);
    int epos2 = getEvtXCord (currImg.endT);

    int beg = x, end = x + widthCan, tendH = tbegH + widthCan; 
    
    int gap1 = epos1 - beg;
    int gap2 = end - epos2;
   
    panePosX += (beg - tbegH); //Move the scroll position according to the increment
    
    if ((beg < epos1 && tbegH > epos1)) bflagH = true;
    else if (beg > epos1 && tbegH < epos1) bflagH = false;
    if ((end > epos2 && tendH < epos2)) fflagH = true;
    else if (end < epos2 && tendH > epos2) fflagH = false;
    
    int gr = getCurrGridH (x);
    boolean change = ((bflagH && gap1 >= widthCan) || (fflagH && gap2 >= widthCan) ||
                      gap1 > _xPix || gap2 > _xPix)?
      true : false;
    
    if (change) {
      getImgH (gr);
      panePosX = _xPix + (x - (gr * _xPix));
      bflagH = false; fflagH = false;
    }    
    tbegH = beg; begTime = getTime (tbegH); adjustStartEndTimes ();
    //    zXTime = getTime (tbegH); 
  }
  
  private int getGridValue (int val) {return val - (getCurrGridH (val) * _xPix);}
  
  /**
   * Get the appropriate display for the given grid
   */
  private void getImgH (int grid) {
    double begT = getTime (grid * _xPix);
    double endT = begT + getTime (_xPix);
    MyImage i = searchImg (begT, endT);
    if (i != null) currImg = i;
    else currImg.drawRegion (begT, endT);
    centralizeH (); repaint ();
  }
 
  /**
   * search for a image with the given starting and ending times
   */
  private MyImage searchImg (double begT, double endT) {
    for (int i = 0; i < 3; i++) {
      MyImage cImg = img [i];
      if (cImg != null && cImg.begT == begT && cImg.endT == endT) return cImg;
    }
    return null;
  }

  /**
   * Return the appropriate value from the circular buffer
   */
  private int getNext (int t) {return (t + 1) % 3;}
  
  private int getPrev (int t) {
    int ret = (t - 1) % 3;
    if (t == 0) ret = 2;
    return ret;
  }

  /**
   * Draw the image data which is before and after the current Image
   */
  private void centralizeH () {
    double t = currImg.endT - currImg.begT;
    if (currImg.endT < maxT) {
      int nextId = getNext (currImg.imgIdH);
      if (img [nextId].begT != currImg.endT ||
          img [nextId].endT != (currImg.endT + t)) renderNextH (1);
    }
    if (currImg.begT > 0.0) {
      int prevId = getPrev (currImg.imgIdH);
      if (img [prevId].endT != currImg.begT ||
          img [prevId].begT != (currImg.begT - t))renderPrevH (1);
    }
  }
  
  /**
   * Draw the image data for the nth image before the current one
   */
  private void renderPrevH (int n) {
    int prevId = getPrev  (currImg.imgIdH);
    double tL = getTime (_xPix);
    img [prevId].drawRegion (currImg.begT - n * tL, currImg.begT - (n - 1) * tL);
  }
  
  /**
   * Draw the image data for the nth image after the current one
   */
  private void renderNextH (int n) {
    int nextId = getNext (currImg.imgIdH);
    double tL = getTime (_xPix);
    img [nextId].drawRegion (currImg.endT + (n - 1) * tL, currImg.endT + n * tL);
  }
 
  /**
   * Return the image # where the given scrollbar position points to
   */
  private int getCurrGridH (int x) {return x / _xPix;}
  //--------------------------------------------------------------------------------

  /**
   * place the given StateInfo object in the stateVector of corresponding process
   */
  private void addState (StateInfo event) {
    int pid = event.procId;
    
    if (event.endT > maxT) maxT = event.endT; 

    if ((proct - 1) < pid) addProcess (pid);
    
    JProcess currProc = (JProcess)procVector.elementAt (pid);
    ProcessState ps = new ProcessState (event, currProc);
    currProc.procStateVector.addElement (ps);
  }

  /**
   * Create a new JProcess object if needed
   */
  private void addProcess (int pid) {
    for (int x = proct; pid > proct - 1; x++) {
      JProcess currProc = new JProcess (x);
      procVector.addElement (currProc);
      proct ++;
    }
  }

  //Methods describing horizontal and vertical position-------------------------------
  private void calcYDensity () {
    yDensity = eachDispIHt / (proct + ((proct + 1) / 5.0));
  }
  
  double getProcYCord (int pid) {
    return (hBWt + ((2 * pid + 1) * yDensity / 2.0) + ((pid + 1) * yDensity / 5.0));
  }
  
  double getEvtYCord (int procId) {
    //This method calculates the y co-ordinate of the given event w.r.t the
    //y co-ordinate of the time line for the related processor
    //yDensity contains pixels / eventHeight
    return getProcYCord (procId) - yDensity  / 2.0;
  }

  /**
   * This method calculates the x co-ordinate of the givent event from its
   * starting time
   */
  int getEvtXCord (double t) {
    return (int)(Math.rint (t * xDensity));
  }

  double getTime (int pos) {
    double time = (1.0 / xDensity) * pos;
    return time;
  }
  
  /**
   * This method gets the length for the given event based on its starting and
   * ending timestamps
   */
  int getW (double begT, double endT) {
    return  ((int)Math.rint (endT * xDensity) - (int)Math.rint (begT * xDensity));
  }
  
  void updateH () {
    maxH = getW (0, maxT);
  }
  //---------------------------------------------------------------------------------------
  
  /**
   * Return index of the process where the given y position is placed
   */
  int findIndex (int y) {
    int index = -1;
    for (int i = 0; i < proct; i++) {
      int a = (int)Math.rint (getEvtYCord (i));
      int b = a + (int)Math.rint (yDensity);
      if (y >= a && y <= b) {index = i; break;}
    }
    return index;
  }
  
  /**
   * Return the state upon which the given x and y coordinates are placed
   */
  private ProcessState findState (int x, int y) {
    int dIndex = y / eachDispHt;
    
    if (((Integer)dtypeV.elementAt (dIndex)).intValue () != CONST.TIMELINES) return null;
    
    int index = findIndex (y % eachDispHt);
    
    y = y % eachDispHt;
    
    if (index != -1) {
      JProcess currProc = (JProcess)(procVector.elementAt (index));
      if (currProc.dispStatus) {
        Enumeration e = currProc.procStateVector.elements ();
        while (e.hasMoreElements ()) {
          ProcessState currState = (ProcessState)(e.nextElement());
          //Determining using position (less accurate)
          if (currState.info.stateDef.checkbox.isSelected () &&
              (getEvtXCord (currState.info.begT - currImg.begT) <= cursorPos) &&
              (cursorPos <= getEvtXCord (currState.info.endT - currImg.begT)) &&
              checkLevel (currState, y))
            return currState;
        }
      }
    }
    return null;
  }

  /**
   * Method checks whether the given y coordinate is valid above the given state  
   */
  private boolean checkLevel (ProcessState currState, int y) {
    //This check is not very efficient as for small nestFactors differentiating between
    //nested states may not yield correct results. Here we give preference to the boundry
    //of the inner state over the outer state --> y => currState.pt.y && y <= (curr.......
    //A choice had to be made and we chose this.
    if (y >= currState.y && y <= (currState.y + currState.h))
      return true;
    return false;
  }
  
  /**
   * This method returns the index in the given vector of the process with the
   * given procId
   */
  int getIndex (Vector v, int procId) {
    Enumeration enum = v.elements ();
    int index = 0;
    while (enum.hasMoreElements ()) {
      JProcess currProc = (JProcess)(enum.nextElement ());
      if (currProc.procId == procId) break;
      index ++;
    }
    return index;
  }
  
  /**
   * Return the ArrowInfo object upon which the given x and y coordinates are placed
   */
  private ArrowInfo findMsg (int x, int y) {
    if (arrowDispStatus) {
      int dIndex = y / eachDispHt;
      if (((Integer)dtypeV.elementAt (dIndex)).intValue () != CONST.TIMELINES) return null;
      
      int index = findIndex (y % eachDispHt);
      
      y = y % eachDispHt;
      
      if (index != -1) {
        JProcess currProc = (JProcess)(procVector.elementAt (index));
        Enumeration enum = parent.quiver.arrowVector.elements ();
        while (enum.hasMoreElements ()) {
          ArrowInfo arrow = (ArrowInfo)enum.nextElement ();
          if (arrow.begProcId == currProc.procId) {
            int x1 = getEvtXCord (arrow.begT - currImg.begT);
            int y1 = currProc.y;
            if (((x1 - img [0].brad) <= (x - _xPix)) && 
                ((x - _xPix) <= (x1 + img [0].brad)) &&
                ((y1 - img [0].brad) <= y) && (y <= (y1 + img [0].brad))) 
              return arrow;
          }
        }
      }
    }
    return null;
  }
  
  void calcArrowLens () {
    Enumeration enum = parent.quiver.arrowVector.elements ();
    while (enum.hasMoreElements ()) {
      ArrowInfo arrow = (ArrowInfo)enum.nextElement ();
      arrow.lenT = arrow.size / (double)(arrow.endT - arrow.begT);
    }
  }
  
  //Event Handler methods----------------------------------------------------------------
  //Events may be generated when the mouse is pressed, released or moved and also if
  //keyboard keys 'Z' or 'T' are pressed
  private void setupEventHandlers () {
    this.enableEvents  (AWTEvent.MOUSE_MOTION_EVENT_MASK | 
                        AWTEvent.MOUSE_EVENT_MASK |
                        AWTEvent.KEY_EVENT_MASK);
  }

  /**
   * Handles the event when the mouse is moved
   */
  public void processMouseMotionEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_MOVED) {
      adjustTimeField (e.getX ());
      adjustElTimeField ();
    }
    else super.processMouseMotionEvent (e);
  }
  
  /**
   * Handles the event when the mouse is pressed or released
   */
  public void processMouseEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_PRESSED && parent.dtype == CONST.TIMELINES) {
      message = handleMsgDlg (e);
      if (!message) handleEventDlg (e);
    }
    else super.processMouseEvent (e);
  }
  
  /**
   * Check if a state dialog is to be displayed??
   */
  private void handleEventDlg (MouseEvent e) {
    int x = e.getX (), y = e.getY ();
    if (y >= allDispHt) return;
    ProcessState currEvt = findState (x, y);
    if (currEvt != null) {
      StateDlg sd = new StateDlg (parent, currEvt);
      
      Point p = getLocationOnScreen ();
      int tx = x, ty = y;
      tx += p.x; ty += p.y;
      sd.setLocation (tx, ty); 
      
      sd.setVisible (true);
    }
  }
  
  /**
   * Check if a message dialog is to be displayed??
   */
  private boolean handleMsgDlg (MouseEvent e) {
    int x = e.getX (), y = e.getY ();
    if (y >= allDispHt) return false;
    ArrowInfo arrow = findMsg (x, y);
    if (arrow != null) {
      MsgDlg md = new MsgDlg (parent, arrow);
      
      Point p = getLocationOnScreen ();
      int tx = x, ty = y;
      tx += p.x; ty += p.y;
      md.setLocation (tx, ty);
      
      md.setVisible (true);
      
      return true;
    }
    return false;
  }
  
  /**
   * Handles the event when the key is pressed
   */
  public void processKeyEvent (KeyEvent e) {
    int keyCode = e.getKeyCode ();
    
    if (e.getID () == KeyEvent.KEY_PRESSED) {
      if (keyCode == KeyEvent.VK_Z) lockZoom ();
      else if (keyCode == KeyEvent.VK_T) 
        fixElTimePointer (currTime);
    }
    else super.processKeyEvent (e);
  }
  //---------------------------------------------------------------------------------------
  
  /**
   * lock zoom at the current cursor position
   */
  private void lockZoom () {
    if (zoomLkLineDispStatus) {
      zXTime = currTime;
      repaint ();
    }
  }
  
  /**
   * fix the elapsed time line to the given time
   */
  void fixElTimePointer (double time) {
    if (elTLineDispStatus) {
      elapsedPoint = time;
      //Refresh ();
      // currImg.reDrawElTimeLine (); //Not yet implemented
      repaint ();
    }
  }
  //Methods controlling zooming of data--------------------------------------------------

  /**
   * zoom in horizontally
   */
  void zoomInH () {waitCursor (); zoomH (zF); normalCursor ();}
  
  /**
   * zoom in Vertically
   */
  void zoomOutH () {waitCursor (); zoomH (1 / zF); normalCursor ();} 
    
  /**
   * reset view so that all the data fits in a viewport
   */
  void resetView () {
    waitCursor ();
    
    changeConst (1 / zoomH);
    
    if (maxH != widthCan) {
      maxT = tMaxT;
      xDensity = widthCan / maxT;
      maxT += (endGap / xDensity);
      xDensity = widthCan / maxT;      
      updateH ();
    }
    adjustZoomImg (0);
    normalCursor ();
  }
  
  /**
   * method called whenever a horizontal zoom is performed
   */
  void zoomH (double z) {
    //Calculations required whenever a horizontal zoom is needed.
    int tmaxH = maxH;

    zDist = getW (getTime (tbegH), zXTime);
    
    changeConst (z);
    
    //Get the scrollbar Position in the zoomed Image
    int xcord = getEvtXCord (zXTime);
    sbPos = ((tmaxH > widthCan || maxH > widthCan) && xcord > zDist)? xcord - zDist : 0;
     
    if (sbPos + parent.hbar.getVisibleAmount () >= maxH)
      sbPos =  maxH - parent.hbar.getVisibleAmount ();
    if (sbPos < 0) sbPos = 0;
    
    //Get the appropriate image
    adjustZoomImg (sbPos);
  }
  
  private void changeConst (double z) {
    zoomH *= z; xDensity *= z;
    updateH ();
 
  }
  
  //  public void update (Graphics g) {paint (g);}

  private void adjustZoomImg (int sbPos) {
    getImgH (getCurrGridH (sbPos)); 
    bflagH = false; fflagH = false; 
    tbegH = sbPos;
    if (sbPos > 0) {
      int extra = getW (currImg.begT, zXTime) - zDist;
      int rem = getW (currImg.begT, maxT) - widthCan;
      if (extra > rem) extra = rem;
      panePosX = _xPix + extra;
    }  
    else panePosX = _xPix;
    begTime = getTime (tbegH);
    adjustStartEndTimes ();
  }
  
  void changeZoomFactor (double z) {
    if (z <= 0) {new ErrorDiag (null, "Value must be > 0"); return;}
    zF = z;
  }
  //---------------------------------------------------------------------------------
  
  /**
   * This function renders in viewport view corresponding to given start and end times
   */
  void changeExTime (double start, double end) {
    double diff = end - start; 
    if (diff <= 0.0) return;
    
    int pos = getW (0, start);
    parent.hbar.setValue (pos);  //Position the scrollbar at the start time
    parent.setPosition (pos);    //render the view with this start time

    //Now zoom in/out so that the end time corresponds with the given one.
    zoomH ((endTime - begTime) / diff); 
    parent.zoomH ();
  }
  
  /**
   * This function renders in viewport view corresponding to current start and 
   * given end times
   */
  void changeToTime (double end) {
    if (end <= begTime) {
      new ErrorDiag (null, "value must be > " + Double.toString (begTime)); return;
    }
    
    double diff = end - begTime; 
    
    int pos = getW (0, begTime);
    parent.hbar.setValue (pos);  //Position the scrollbar at the start time
    parent.setPosition (pos);    //render the view with this start time

    //Now zoom in/out so that the end time corresponds with the given one.
    zoomH ((endTime - begTime) / diff); 
    parent.zoomH ();
  }
  
  /**
   * This function renders in viewport view corresponding to given start and 
   * current end times
   */
  void changeFromTime (double start) {
    if (start >= endTime || start < 0) {
      new ErrorDiag (null, "value must be >= 0 and < " + Double.toString (endTime)); return;
    }
    
    double diff = endTime - start; 
    
    int pos = getW (0, start);
    parent.hbar.setValue (pos);  //Position the scrollbar at the start time
    parent.setPosition (pos);    //render the view with this start time

    //Now zoom in/out so that the end time corresponds with the given one.
    zoomH ((endTime - begTime) / diff); 
    parent.zoomH ();
  }

  /**
   * Method called whenver the canvas has to be resized
   */
  void Resize () {
    waitCursor ();

    Dimension dimVP = parent.vport.getExtentSize ();

    widthCan = dimVP.width; heightCan = dimVP.height;
    
    _xPix = 3 * widthCan; _yPix = heightCan;
    
    if (_yPix < 1) _yPix = 1; if (_xPix < 1) _xPix = 1;
    
    //Calling both methods may not be needed
    setPreferredSize (getPreferredSize ()); setSize (getPreferredSize ());
    
    maxT = tMaxT;
    xDensity = (widthCan / maxT) * zoomH;
    maxT += (endGap / xDensity);
    xDensity = (widthCan / maxT) * zoomH;
    
    updateH ();
    
    setupImg ();

    if (!setupComplete) drawInitImg ();
    else {
      sbPos = getEvtXCord (begTime);
      currImg = img [0];
      adjustZoomImg (sbPos);
    }

    panePosX = _xPix + getW (currImg.begT, begTime);
    parent.zoomH ();
    normalCursor ();
  }
 
  /**
   * Redraw all the images so that changes in options can take effect
   * CAUTION: setupNestedStates will hog up some of CPU's time but, it has to be performed
   * to keep nesting upto date
   */
  void Refresh () {
    waitCursor ();
    blink = false; blFlag = true; if (timer.isRunning ()) timer.stop ();
    int prevH = getPrev (currImg.imgIdH);
    int nextH = getNext (currImg.imgIdH);
    setupNestedStates ();
    img [prevH].drawStuff ();
    img [nextH].drawStuff ();
    currImg.drawStuff ();
    if (blink) timer.start ();
    repaint ();
    parent.vcanvas1.repaint (); parent.vcanvas2.repaint ();
    parent.vport.setViewPosition (new Point (panePosX, 0));
    normalCursor ();
  }

  //--------------------------------------------------------------------------------------
  /**
   * Methods to handle deleting, swapping and horizontally translating process display
   * Delete the process at the given index of procVector
   */
  void DeleteProc (int index) {
    JProcess p = (JProcess)(procVector.elementAt (index));
    procVector.removeElementAt (index);
    dprocVector.addElement (p);
    proct --; dproct ++;
    calcYDensity ();
  }
  
  /**
   * Delete process at the given index from dprocVector and insert it on top
   * of the process with the given pid
   * dindex = index into dProcVector which specifies the JProcess object to be removed
   * pid = pid of the process before which the removed process is to be inserted
   */
  void InsertProc (int dindex, int pid) {
    JProcess dp = (JProcess)(dprocVector.elementAt (dindex));
    dprocVector.removeElementAt (dindex);
    procVector.insertElementAt (dp, getIndex (procVector, pid));
    proct++; dproct--;
    calcYDensity ();
  }
  
  /**
   * push the timeLine and all states of the given process.
   * p = process whose time line is to be moved
   * time = this specifies the time in seconds the time line is to be moved
   * (time > 0) => right, (time < 0) => left
   */
  private void pushTimeLine (JProcess p, double time) {
    Enumeration enum = p.procStateVector.elements ();
    ProcessState currEvt = (ProcessState)(p.procStateVector.firstElement ());
    if (currEvt != null) {
      currEvt.info.begT += time;
      if (currEvt.info.begT < 0) {
        time -= currEvt.info.begT;
        currEvt.info.begT = 0;
      }
      currEvt.info.endT += time;
      enum.nextElement ();
    }
    
    while (enum.hasMoreElements ()) {
      currEvt = (ProcessState)(enum.nextElement ());
      currEvt.info.begT += time;
      currEvt.info.endT += time;
    }
    
    //Push the arrows
    pushArrows (p, time);
    
    //position the image appropriately
    adjustPosition ();
  }
  
  /**
   * push the time line and all states of the given states
   * p = process whose time line is to be moved
   * amount = displacement in pixels to move
   * dir = direction. true => right, false => left
   */
  void pushTimeLine (JProcess p, int amount, boolean dir) {
    double time = getTime (amount);
    if (!dir) time *= (- 1);
    pushTimeLine (p, time);
  }
  
  private void pushArrows (JProcess p, double time) {
    Enumeration enum = parent.quiver.arrowVector.elements ();
    while (enum.hasMoreElements ()) {
      ArrowInfo arrow = (ArrowInfo)enum.nextElement ();
      if (arrow.begProcId == p.procId) arrow.begT += time;
      if (arrow.endProcId == p.procId) arrow.endT += time;
    }
  }
  
  /**
   * This method adjusts the scroll position of scrollPane container when
   * the time line is being moved
   */
  private void adjustPosition () {
    double max = 0;
    Enumeration enum = procVector.elements ();
    
    while (enum.hasMoreElements ()) {
      JProcess p = (JProcess)enum.nextElement ();
      ProcessState ps = (ProcessState)p.procStateVector.lastElement ();
      if (ps.info.endT > max) max = ps.info.endT;
    }
    
    boolean end = false;
    
    if (max < tMaxT || max > tMaxT) {
      if (max < tMaxT) end = true; 
      double diff = tMaxT - max;
      tMaxT = max;
      maxT -= diff;
      updateH ();
      parent.setHoriz ();
      centralizeH (); 
    }
    if (end) {
      if ((parent.hbar.getValue () + parent.hbar.getVisibleAmount ()) >= parent.hbar.getMaximum ()) {
        parent.hbar.setValue (maxH - widthCan);
        adjustImgH (parent.hbar.getValue ());
	parent.vport.setViewPosition (new Point (panePosX, 0));
      }
    }
  }
  
  /**
   * This method returns the maximum value in pixels that the time line can be moved
   * to the left before reaching the 0 time position
   * p = the process whose time line is to be moved
   */
  int getMaxDiff (JProcess p) {
    if (p.procStateVector.size () > 0) {
      ProcessState ps = (ProcessState)(p.procStateVector.firstElement ());
      return getEvtXCord (ps.info.begT - 0.0);
    }
    return 0;
  }
  
  /**
   * Repostions the time line to its original position
   * p = the process whose time line is to be repositioned
   */
  void resetTimeLine (JProcess p) {
    ProcessState ps = (ProcessState)p.procStateVector.firstElement ();
    pushTimeLine (p, p.begT - ps.info.begT);
  }
  //end of methods dealing with process position manipulation------------------------------

  /** 
   * change nest factor to given value and cause change to take effect on display
   */
  void changeNestFactor (double n) {
    if (maxLevel < 1) return;
    
    double uBound = 1 / (double)maxLevel;
    
    if (n < 0 || n >= uBound) {
      new ErrorDiag (null, "Value must be > 0 and < " + (new Double (uBound)).toString ());
      return;
    }
    nestFactor = n;
    Refresh ();
  }
  
  /**
   * Calculates most appropriate nest factor by dividing total thickness
   * equally among all nested levels
   */
  double doAppropNesting () {
    changeNestFactor (1 / ((double)maxLevel + 1));
    return nestFactor;
  }
  
  /**
   * returns the desired scroll position in the scrollpane
   */
  int getPanePosX () {return panePosX;}
  
  /**
   * Method required by the scrollpane container to adjust values
   */
  public Dimension getPreferredSize () {return new Dimension (_xPix * 3, 
                                                              _yPix);}
  /**
   * Adjust field values
   */
  void adjustTimeField (int x) {
    cursorPos = (x - _xPix);
    parent.adjustTimeField (currTime = currImg.begT + getTime (cursorPos));
  }
  
  void adjustElTimeField () {
    parent.adjustElTimeField (currTime - elapsedPoint);
  }
  
  void adjustStartEndTimes () {
    endTime = begTime + getTime (widthCan);
    parent.optionsDlg.adjustTimes (begTime, endTime);
  }
  
  void addDisplay (int dtype) {
    waitCursor ();
    dtypeV.addElement (new Integer (dtype));
    setupImg ();
    sbPos = getEvtXCord (begTime);
    currImg = img [0];
    adjustZoomImg (sbPos);
    panePosX = _xPix + getW (currImg.begT, begTime);
    parent.zoomH ();
    normalCursor ();
  }
  
  void removeDisplay (int index) {
    waitCursor ();
    dtypeV.removeElementAt (index);
    setupImg ();
    sbPos = getEvtXCord (begTime);
    currImg = img [0];
    adjustZoomImg (sbPos);
    panePosX = _xPix + getW (currImg.begT, begTime);
    parent.zoomH ();
    normalCursor ();
  }
  
  /**
   * sets the current cursor to WAIT_CURSOR type
   */
  void waitCursor () {setCursor (new Cursor (Cursor.WAIT_CURSOR));}
  
  /** 
   * sets the WAIT_CURSOR to cursor associated with this canvas
   */
  void normalCursor () {setCursor (new Cursor (Cursor.CROSSHAIR_CURSOR));}
  
  /**
   * Handle the event when JViewport is resized
   */
  public void componentResized (ComponentEvent e) {
    parent.waitCursor (); 
    if (parent.setupComplete) Resize (); 
    parent.normalCursor ();
  } 
   
  /**
   * Unused methods of ComponentListener interface
   */
  public void componentHidden (ComponentEvent e) {;}
  public void componentMoved (ComponentEvent e) {;}
  public void componentShown (ComponentEvent e) {;}
  
  public void kill () {
    resetProcTimes ();
    procVector = null;
    dprocVector = null;
    killImgs ();
  }
  
  private void resetProcTimes () {
    for (int i = 0; i < procVector.size (); i++) 
      resetTimeLine ((JProcess)procVector.elementAt (i));
    for (int i = 0; i < dprocVector.size (); i++) 
      resetTimeLine ((JProcess)dprocVector.elementAt (i));
  }
  
  protected void finalize() throws Throwable {super.finalize();}
  
  class TimerListener implements ActionListener {
    public void actionPerformed(ActionEvent evt) {
      blFlag = !blFlag;
      repaint ();
    }
  }
  
  boolean getZLkLineDStat () {return zoomLkLineDispStatus;}
  boolean getElTLineDStat () {return elTLineDispStatus;}
  void updateZLkLineDStat (boolean b) {zoomLkLineDispStatus = b; repaint ();}
  void updateElTLineDStat (boolean b) {elTLineDispStatus = b; repaint ();}
}
