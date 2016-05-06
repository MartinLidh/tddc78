import java.io.*;
import java.awt.*;
import java.awt.image.*;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.event.*;
import java.lang.Math;
import javax.swing.*;

/*
  This class is the canvas on which all drawing of Jumpshot data is done
  A new data structure containing a vector of StateGroup objects resides here.
  Each of these StateGroup objects represent a process and contain a vector
  'stateVector' which is a list of all states of that process, in 
  ascending order of their end timestamps. 
  
  Double Buffering is used for smooth scrolling. For an explanation see 
  the documents.
*/

public class ProgramCanvas extends JComponent 
implements ComponentListener {
  Graphics currG;
  public FrameDisplay parent;
  boolean blink = false;
  boolean blFlag = true;
  
  //Data Structures
  
  // Data Structures containing all displayable states
  StateGroupListPair            all_states;
  Vector dtypeV;
  
  double xDensity;              //pixels / second
  double yDensity;              //pixels / process vertical thickness
  int _xPix;                    //width  of each image
  int _yPix;                    //height of each image
  int widthCan;                 //width  of the viewport 
  int heightCan;                //height of the viewport
  
  double maxT;                  //Maximum duration in time in the display
  double tMaxT;                 //end time of the last state in all tasks
                                //relative to the FrameBegTime
  
  int maxH;                     //horizontal pixel limit on display
  //horizontal distance between the start of the viewport and the zoom lock line
  int zDist;

  //horizontal scroll postion of JViewport in the 3 images canvas
  int panePosX; 
  int sbPos;             
  int cursorPos;                //Horizontal cursor position in this canvas
  int tbegH;                    //previous horizontal scrollbar value
  boolean fflagH, bflagH;       //flags describing forward or backward 
                                //      scrolling (horizontally) 
  
  public double zoomH;          //double value describing the zoom
  double zF;                    //zoom Factor
  
  double FrameBegTime;          //the start time of the Frame
  double FrameEndTime;          //the end time of the Frame
  double begTime;               //time at start of the viewport
  double endTime;               //time at the end of the viewport
  double currTime;              //time at the current cursor position
  double zXTime;                //time at which the zoom lock line was set
  double elapsedPoint;          //time at which the elapsed time line was set
  
  private boolean elTLineDispStatus = true;    //elapsed time line display 
                                               //status
  private boolean zoomLkLineDispStatus = true; //zoom lock line display status
  
  // int pu = 1;
  int endGap = 0;              //horizontal gap at the end of drawings so that
                               //the drawings towards the right end are not
                               //covered by the ScrollPane Container
  
  MyImage [] img;               //array of 3 offscreen images
  MyImage currImg;              //current image placed in the viewport

  
  int lineSize, rulerHt, hBWt, allDispHt, eachDispHt, eachDispIHt, fDescent;

  public int dpi;               //number of pixels / inch on the screen
  public FontMetrics fm;        //FontMetrics object describing the curren font
 
  // int maxLevel = 0;             //maximum level of nesting
  double nestFactor;            //value describing the height difference 
                                //between levels
  //nestFactor * yDensity will be the difference in height between two consecutive levels
  
  boolean message = false;      //value decribing whether an arrow circle is present ??
  boolean Windows = false;      //catering for the extra scrollbar
  public boolean setupComplete; //setup status 
  Timer timer;
  int totalProc;
  
  private int elTimeStrW, zLockStrW;
  
  //Constructor
  public ProgramCanvas() { super (); }

  //Setup methods -----------------------------------------------------------
    void init ( FrameDisplay parent_win, double starttime, double endtime ) 
    {
        setupComplete = false;
        parent = parent_win;
        all_states = parent_win.all_states;
        setupData ( starttime, endtime );
        adjustCanvasStuff ();
        //  setupStates ();
        //if (parent.dtype == CONST.TIMELINES)
        waitCursor();
        all_states.visible.SetupNestedStates();
        normalCursor();
        dtypeV = new Vector ();
        dtypeV.addElement (new Integer (parent.dtype));
        parent.optionsDlg.addDisplay (parent.dtype);
        //else setupMtns ();
        totalProc = all_states.visible.size();
        tMaxT = maxT;
        timer = new Timer (200, new TimerListener ());
        Resize ();
        setupEventHandlers ();
        debug.println( "In ProgramCanvas's Init() " );
        adjustStartEndTimes ();
        setupComplete = true;
    }

  private void setupData( double starttime, double endtime ) {
    nestFactor = 0.10;
    zoomH = 1.0;
    zF = 2.0;
    //begTime = 0.0;
    FrameBegTime = starttime; FrameEndTime = endtime;
    begTime = FrameBegTime; endTime = FrameEndTime;
    zXTime = FrameBegTime;
    elapsedPoint = FrameBegTime;
    // zXTime = 0.0; elapsedPoint = 0.0;
    maxT = FrameEndTime - FrameBegTime;
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


/*  
    private void setupMtns () {
    Enumeration enum = parent.mtns.elements (); 
    while (enum.hasMoreElements ()) {
      MtnInfo mtn = (MtnInfo)enum.nextElement ();
      if (mtn.endT > maxT) maxT = mtn.endT; 
    }
  }
*/
  
  /**
   * Draw initial view consisting of entire data in viewport
   */
  private void drawInitImg () {
    // double b = begTime, e = endTime;
    // double b = begTime, e = getTime (_xPix);
    double b = begTime, e = b + getTime(_xPix);
    debug.println( "drawInitImg() : " );
    debug.println( "\t" + "img[0].drawRegion( " + b + ", " + e + " )" );
    img [0].drawRegion (b, e);
    currImg = img [0];
    debug.println( "currImg.begT = " + currImg.begT + ",  "
                 + "currImg.endT = " + currImg.endT );
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
  private void setupImg()
  {
    killImgs();
    setValues();
    parent.vcanvas1.repaint();
    parent.vcanvas2.repaint();
    
    img = new MyImage[ 3 ];
    for (int i = 0; i < 3; i++) {
      img [i] = new MyImage( i, _xPix, _yPix, dtypeV, this );
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
    //  int epos1 = getEvtXCord (currImg.begT);
    //  int epos2 = getEvtXCord (currImg.endT);
    int epos1 = getEvtXCord( currImg.begT - FrameBegTime );
    int epos2 = getEvtXCord( currImg.endT - FrameBegTime );

    int beg = x, end = x + widthCan, tendH = tbegH + widthCan; 
    
    int gap1 = epos1 - beg;
    int gap2 = end - epos2;
   
    panePosX += (beg - tbegH); //Move the scroll position according to the increment
    
    if ((beg < epos1 && tbegH > epos1)) bflagH = true;
    else if (beg > epos1 && tbegH < epos1) bflagH = false;
    if ((end > epos2 && tendH < epos2)) fflagH = true;
    else if (end < epos2 && tendH > epos2) fflagH = false;
    
    int gr = getCurrGridH (x);
    boolean change = ( (bflagH && gap1 >= widthCan)
                    || (fflagH && gap2 >= widthCan)
                    || gap1 > _xPix || gap2 > _xPix ) ?
                    true : false;
    
    if (change) {
      getImgH (gr);
      panePosX = _xPix + (x - (gr * _xPix));
      bflagH = false; fflagH = false;
    }    
    tbegH = beg;
    debug.println( "adjustImgH(" + x + ") : A) begTime = " + begTime
                 + ",  currImg.begT = " + currImg.begT );
    begTime = FrameBegTime + getTime (tbegH);
    //  begTime = getTime (tbegH);
    //  begTime = currImg.begT + getTime (tbegH);
    debug.println( "adjustImgH(" + x + ") : B) tbegH = " + tbegH
                 + ",  " + "begTime = " + begTime );
    adjustStartEndTimes ();
    //    zXTime = getTime (tbegH); 
  }
  
  private int getGridValue (int val) {return val - (getCurrGridH (val) * _xPix);}
  
  /**
   * Get the appropriate display for the given grid
   */
  private void getImgH (int grid) {
    // double begT = getTime (grid * _xPix);
    // double begT = currImg.begT + getTime (grid * _xPix);
    double begT = FrameBegTime + getTime (grid * _xPix);
    double endT = begT + getTime (_xPix);
    MyImage the_img = searchImg (begT, endT);
    if ( the_img != null ) {
        currImg = the_img;
        debug.println( "getImgH(" + grid + ") : "
                     + "currImg = Img[" + the_img.imgIdH + "]" );
    }
    else { 
        debug.println( "getImgH(" + grid + ") : " );
        debug.println( "\t" + "currImg[" + currImg.imgIdH + "].drawRegion("
                     + begT + ", " + endT + ")" );
        currImg.drawRegion (begT, endT);
    }
    centralizeH ();
    repaint ();
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
    private int getNext( int t )
    {
        return (t + 1) % 3;
    }
  
    private int getPrev( int t )
    {
        if ( t == 0 )
            return 2;
        else
            return (t - 1) % 3;
    }

  /**
   * Draw the image data which is before and after the current Image
   */
  private void centralizeH () {
    double t = currImg.endT - currImg.begT;
    debug.println( "centralizeH : " );
    debug.println( "\t" + "currImg.begT = " + currImg.begT + ",  "
                        + "currImg.endT = " + currImg.endT );
    // if (currImg.endT < maxT) {
    if ( currImg.endT < FrameBegTime + maxT ) {
      int nextId = getNext (currImg.imgIdH);
      if (img [nextId].begT != currImg.endT ||
          img [nextId].endT != (currImg.endT + t)) renderNextH (1);
      debug.println( "\t" + "nextImg.begT = " + img[nextId].begT + ",  "
                          + "nextImg.endT = " + img[nextId].endT );
    }
    //  if (currImg.begT > 0.0) {
    if ( currImg.begT > FrameBegTime ) {
      int prevId = getPrev (currImg.imgIdH);
      if (img [prevId].endT != currImg.begT ||
          img [prevId].begT != (currImg.begT - t))renderPrevH (1);
      debug.println( "\t" + "prevImg.begT = " + img[prevId].begT + ",  "
                          + "prevImg.endT = " + img[prevId].endT );
    }
  }
  
  /**
   * Draw the image data for the nth image before the current one
   */
  private void renderPrevH (int n) {
    int prevId = getPrev  (currImg.imgIdH);
    double tL = getTime (_xPix);
    debug.println( "renderPrevH(" + n + ") : " );
    debug.println( "\t" + "img["+prevId+"].drawRegion("
                 + (currImg.begT - n * tL) + ", "
                 + (currImg.begT - (n - 1) * tL) + " )" );
    img [prevId].drawRegion (currImg.begT - n * tL, currImg.begT - (n - 1) * tL);
  }
  
  /**
   * Draw the image data for the nth image after the current one
   */
  private void renderNextH (int n) {
    int nextId = getNext (currImg.imgIdH);
    double tL = getTime (_xPix);
    debug.println( "renderNextH(" + n + ") : " );
    debug.println( "\t" + "img["+nextId+"].drawRegion("
                 + (currImg.endT + (n - 1) * tL) + ", "
                 + (currImg.endT + n * tL) + " )" );
    img [nextId].drawRegion (currImg.endT + (n - 1) * tL, currImg.endT + n * tL);
  }
 
  /**
   * Return the image # where the given scrollbar position points to
   */
  private int getCurrGridH (int x) {return x / _xPix;}
  //--------------------------------------------------------------------------



  //Methods describing horizontal and vertical position------------------------
    private void calcYDensity()
    {
        int groups_size = all_states.visible.size();
        yDensity = eachDispIHt / (groups_size + ((groups_size + 1) / 5.0));
    }

    public void UpdateYDensity()
    {
        calcYDensity();
    }
  
    double GetStateGroupYCord( int seq_idx )
    {
        return ( hBWt + ((2 * seq_idx + 1) * yDensity / 2.0)
               + ( (seq_idx + 1) * yDensity / 5.0 ) );
    }
  
    //This method calculates the y co-ordinate of the given event w.r.t the
    //y co-ordinate of the time line for the related processor
    //yDensity contains pixels / eventHeight
    double getEvtYCord( int seq_idx )
    {
        return GetStateGroupYCord( seq_idx ) - yDensity  / 2.0;
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


  //---------------------------------------------------------------------------
  
  /**
   * Return index of the process where the given y position is placed
   */
  int findIndex (int y) {
    int index = -1;
    // for (int i = 0; i < proct; i++) {
    int groups_size = all_states.visible.size();
    for (int i = 0; i < groups_size; i++) {
      int a = (int)Math.rint (getEvtYCord (i));
      int b = a + (int)Math.rint (yDensity);
      if (y >= a && y <= b) {index = i; break;}
    }
    return index;
  }
  
  /**
   * Return the state upon which the given x and y coordinates are placed
   */
  private StateInterval findState (int x, int y) {
    int dIndex = y / eachDispHt;
    
    if (((Integer)dtypeV.elementAt (dIndex)).intValue () != CONST.TIMELINES)
        return null;
    
    int index = findIndex (y % eachDispHt);
    
    y = y % eachDispHt;
    
    if (index != -1) {
      StateGroup currProc = (StateGroup)(all_states.visible.elementAt (index));
      if (currProc.dispStatus) {
        Enumeration e = currProc.elements();
        while ( e.hasMoreElements() ) {
          StateInterval currState = (StateInterval) e.nextElement();
          //Determining using position (less accurate)
          if ( currState.info.stateDef.checkbox.isSelected () &&
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
   * Method checks whether the given y coordinate is valid above the
   * given state  
   */
    // This check is not very efficient as for small nestFactors
    // differentiating between nested states may not yield correct 
    // results. Here we give preference to the boundry of the inner
    // state over the outer state --> y => currState.pt.y && y <= (curr.......
    // A choice had to be made and we chose this.
  private boolean checkLevel (StateInterval currState, int y) {
    if (y >= currState.y && y <= (currState.y + currState.h))
      return true;
    return false;
  }
  
  /**
   * Return the ArrowInfo object upon which the given x and y coordinates
   * are placed
   */
  private ArrowInfo findMsg( int x, int y )
  {
      RecDef        arrowdef;
      Enumeration   arrows;
      ArrowInfo     arrow;
      int           x1;
      int           y1;

      int dIndex = y / eachDispHt;
      if (    ( (Integer) dtypeV.elementAt(dIndex) ).intValue()
           != CONST.TIMELINES )
          return null;
      
      int index = findIndex( y % eachDispHt );
      
      y = y % eachDispHt;
      
      if ( index != -1 ) {
          StateGroup states = (StateGroup) all_states.visible.elementAt(index);
          y1 = states.y;
          Enumeration arrowDefs = parent.arrowDefs.elements();
          while ( arrowDefs.hasMoreElements() ) {
              arrowdef = ( RecDef ) arrowDefs.nextElement();
              if (    arrowdef.stateVector.size() > 0
                   && arrowdef.checkbox.isSelected() ) {
                  arrows = arrowdef.stateVector.elements();
                  while ( arrows.hasMoreElements() ) {
                      arrow = ( ArrowInfo ) arrows.nextElement();
                      if ( arrow.begGroupID.IsEqualTo( states.groupID ) ) {
                          x1 = getEvtXCord( arrow.begT - currImg.begT );
                          if (    ( (x1 - img[0].brad) <= (x - _xPix) )
                               && ( (x - _xPix) <= (x1 + img[0].brad) )
                               && ( (y1 - img[0].brad) <= y )
                               && ( y <= (y1 + img[0].brad) )
                             ) 
                             return arrow;
                      }
                  }
              }
          }
      }

      return null;
  }


  //Event Handler methods-------------------------------------------------------
  //  Events may be generated when the mouse is pressed, released or moved 
  //  and also if keyboard keys 'Z' or 'T' are pressed
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
  private void handleEventDlg( MouseEvent e ) {
    int x = e.getX(), y = e.getY();
    if (y >= allDispHt) return;
    StateInterval cur_state = findState( x, y );
    if (cur_state != null) {
      RecordDialog sd = new RecordDialog( parent, cur_state );
      
      Point p = getLocationOnScreen();
      int tx = x, ty = y;
      tx += p.x; ty += p.y;
      sd.setLocation(tx, ty); 
      
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
      RecordDialog md = new RecordDialog( parent, arrow );
      
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
      debug.println( "lockZoom() : zXTime = " + zXTime );
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

    // zDist = getW (getTime (tbegH), zXTime);
    // zDist = getW (currImg.begT + getTime (tbegH), zXTime);
    zDist = getW( getTime(tbegH), zXTime - FrameBegTime );
    debug.println( "zoomH(" + z + ") : currImg.begT = " + currImg.begT
                 + ",  begTime = " + begTime + ",  tbegH = " + tbegH );
    debug.println( "\t" + "zXTime = " + zXTime + ",  zDist = " + zDist );
    
    changeConst (z);
    
    //Get the scrollbar Position in the zoomed Image
    // int xcord = getEvtXCord (zXTime);
    // int xcord = getEvtXCord( zXTime - currImg.begT );
    int xcord = getEvtXCord( zXTime - FrameBegTime );
    sbPos = ( (tmaxH > widthCan || maxH > widthCan) && xcord > zDist ) ?
              xcord - zDist : 0;
     
    debug.println( "zoomH(" + z + ") : tmaxH, maxH, widthCan, xcord = "
                 + tmaxH + ", " + maxH + ", " + widthCan + ", " + xcord );
    debug.println( "zoomH(" + z + ") : A) sbPos = " + sbPos );

    if ( sbPos + parent.hbar.getVisibleAmount() >= maxH )
         sbPos =  maxH - parent.hbar.getVisibleAmount();
    if ( sbPos < 0 )
         sbPos = 0;
    debug.println( "zoomH(" + z + ") : B) sbPos = " + sbPos );
    
    //Get the appropriate image
    adjustZoomImg (sbPos);
  }
  
  private void changeConst (double z) {
    zoomH *= z; xDensity *= z;
    updateH ();
  }
  
  //  public void update (Graphics g) {paint (g);}

  private void adjustZoomImg (int pos) {
    int extra = 0, rem = 0;

    getImgH (getCurrGridH (pos)); 
    bflagH = false; fflagH = false; 
    tbegH = pos;
    if (pos > 0) {
        extra = getW(currImg.begT, zXTime) - zDist;
        // rem = getW (currImg.begT, maxT) - widthCan;
        // rem = getW(0, maxT) - widthCan;
        rem = getW (currImg.begT, FrameBegTime + maxT) - widthCan;
        // if (extra > rem) extra = rem;
        // panePosX = _xPix + extra;
        if ( extra > rem )
            panePosX = _xPix + rem;
        else
            panePosX = _xPix + extra;
    }  
    else
        panePosX = _xPix;
    debug.println( "adjustZoomImg(" + pos + ") : currImg.begT = "
                 + currImg.begT + ", currImg.endT = " + currImg.endT
                 + ",  maxT = " + maxT );
    debug.println( "adjustZoomImg(" + pos + ") : extra = " + extra + ", "
                 + "rem = " + rem + ",   panePosX = " + panePosX );
    
    debug.println( "adjustZoomImg(" + pos + ") : A) begTime = "
                 + begTime + ", currImg.begT = " + currImg.begT );
    begTime = FrameBegTime + getTime (tbegH);
    // begTime = getTime (tbegH);
    // begTime = currImg.begT + getTime (tbegH);
    debug.println( "adjustZoomImg(" + pos + ") : B) tbegH = " 
                 + tbegH + ",  " + "begTime = " + begTime );

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
    
    // int pos = getW (0, start);
    int pos = getW( FrameBegTime, start );
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
    
    // int pos = getW (0, begTime);
    int pos = getW( FrameBegTime, begTime );
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
    
    // int pos = getW (0, start);
    int pos = getW( FrameBegTime, start );
    parent.hbar.setValue (pos);  //Position the scrollbar at the start time
    parent.setPosition (pos);    //render the view with this start time

    //Now zoom in/out so that the end time corresponds with the given one.
    zoomH ((endTime - begTime) / diff); 
    parent.zoomH ();
  }

  /**
   * Method called whenver the canvas has to be resized
   */
  void Resize()
  {
    waitCursor ();

    Dimension dimVP = parent.vport.getExtentSize ();
    widthCan        = dimVP.width;
    heightCan       = dimVP.height;
    
    _xPix = 3 * widthCan;
    _yPix = heightCan;
    
    if (_yPix < 1) _yPix = 1;
    if (_xPix < 1) _xPix = 1;
    
    //  Calling both methods may not be needed
    //  setPreferredSize( getPreferredSize() );
    setSize( getPreferredSize() );
    
    maxT = tMaxT;
    xDensity = (widthCan / maxT) * zoomH;
    maxT += (endGap / xDensity);
    xDensity = (widthCan / maxT) * zoomH;
    
    //  setupImg() HAS to be called in every call of Resize(),
    //  So window redraws can be done correctly.   Resize() is called
    //  by componentResized() which is called when window is resized
    updateH ();
    setupImg ();
    
    if ( setupComplete ) {
      // sbPos = getEvtXCord (begTime);
      // sbPos = getW(currImg.begT, begTime);
      sbPos = getEvtXCord( begTime - FrameBegTime );
      currImg = img [0];
      adjustZoomImg (sbPos);
    }
    else 
        drawInitImg();

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
    waitCursor();
    all_states.visible.SetupNestedStates();
    normalCursor();
    img [prevH].drawStuff ();
    img [nextH].drawStuff ();
    currImg.drawStuff ();
    if (blink) timer.start ();
    repaint ();
    parent.vcanvas1.repaint (); parent.vcanvas2.repaint ();
    parent.vport.setViewPosition (new Point (panePosX, 0));
    debug.println( "Refresh() : .parent.vport.setViewPosition("
                 + panePosX + ", 0)" );
    normalCursor ();
  }

  //---------------------------------------------------------------------------
  
  //  private void pushArrows (StateGroup p, double time) {

  public void pushArrows( StateGroup p, double time )
  {
      StateGroupLabel   groupID = p.groupID;
      RecDef            arrowdef;
      Enumeration       arrows;
      ArrowInfo         arrow;

      Enumeration arrowDefs = parent.arrowDefs.elements();
      while ( arrowDefs.hasMoreElements() ) {
          arrowdef = ( RecDef ) arrowDefs.nextElement();
          if (    arrowdef.stateVector.size() > 0
               && arrowdef.checkbox.isSelected() ) {
              arrows = arrowdef.stateVector.elements();
              while ( arrows.hasMoreElements() ) {
                  arrow = ( ArrowInfo ) arrows.nextElement();
                  if ( arrow.begGroupID.IsEqualTo( groupID ) )
                      arrow.begT += time;
                  if ( arrow.endGroupID.IsEqualTo( groupID ) )
                      arrow.endT += time;
              }
          }
      }
  }
  
  /**
   * This method adjusts the scroll position of scrollPane container when
   * the time line is being moved
   */
  //  private void adjustPosition()
  public void adjustPosition()
  {
      StateGroup    group;
      StateInterval state;
      double        duration;
      double        max_duration = 0;

      Enumeration groups = all_states.visible.elements();
      while ( groups.hasMoreElements() ) {
          group = (StateGroup) groups.nextElement ();
          if ( group.size() > 0 ) {
              state = (StateInterval) group.lastElement();
              duration = state.info.endT - FrameBegTime;
              if ( duration > max_duration )
                  max_duration = duration;
          }
      }
    
      boolean end = false;
    
      if (max_duration < tMaxT || max_duration > tMaxT) {
          if (max_duration < tMaxT) end = true; 
          double diff = tMaxT - max_duration;
          tMaxT = max_duration;
          maxT -= diff;
          updateH ();
          parent.setHoriz();
          centralizeH(); 
      }

      if (end) {
          if (  ( parent.hbar.getValue() + parent.hbar.getVisibleAmount() ) 
              >= parent.hbar.getMaximum() ) {
              parent.hbar.setValue (maxH - widthCan);
              adjustImgH (parent.hbar.getValue ());
              parent.vport.setViewPosition (new Point (panePosX, 0));
              debug.println( "adjustPosition() : " );
              debug.println( "\t" + "parent.hbar.setValue("
                           + (maxH - widthCan) + ")" );
              debug.println( "\t" + "parent.vport.setViewPosition("
                           + panePosX + ", 0)" );
          }
      }
  }
  
  /**
   * This method returns the maximum value in pixels that the time line 
   * can be moved to the left before reaching the (begTime) time position
   * p = the process whose time line is to be moved
   */
    int getMaxDiff ( StateGroup group )
    {
        if ( group.size() > 0 ) {
            StateInterval state = (StateInterval) group.firstElement();
            // return getEvtXCord (state.info.begT - 0.0);
            return getEvtXCord ( state.info.begT - FrameBegTime );
         }
         return 0;
    }
  
  //end of methods dealing with process position manipulation------------------

  /** 
   * change nest factor to given value and update the display
   */
  void changeNestFactor (double n) {
    if ( all_states.visible.GetMaxNestingLevel() < 1) return;
    
    double uBound = 1 / (double) all_states.visible.GetMaxNestingLevel();
    
    if (n < 0 || n >= uBound) {
      new ErrorDiag (null, "Value must be > 0 and < "
                           + ( new Double(uBound) ).toString() );
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
    changeNestFactor (1 / ((double) all_states.visible.GetMaxNestingLevel() + 1));
    return nestFactor;
  }
  
  /**
   * returns the desired scroll position in the scrollpane
   */
  int getPanePosX() { return panePosX; }
  
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
    debug.println( "Before adjustStartEndTimes() : "
                 + "begTime = " + begTime + ",  endTime = " + endTime
                 + ",  widthCan = " + widthCan );
    endTime = begTime + getTime (widthCan);
    parent.optionsDlg.adjustTimes (begTime, endTime);
    debug.println( "After  adjustStartEndTimes() : "
                 + "begTime = " + begTime + ",  endTime = " + endTime 
                 + ",  widthCan = " + widthCan );
  }
  
  void addDisplay (int dtype) {
    waitCursor ();
    dtypeV.addElement (new Integer (dtype));
    setupImg ();
    // sbPos = getEvtXCord (begTime);
    // sbPos = getW( currImg.begT, begTime );
    sbPos = getEvtXCord( begTime - FrameBegTime );
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
    // sbPos = getEvtXCord (begTime);
    // sbPos = getW( currImg.begT, begTime );
    sbPos = getEvtXCord( begTime - FrameBegTime );
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
    debug.println( "*******" + "___componentResized()___ : start!" );
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
    // resetProcTimes ();
    all_states.ResetTimeLines();
    all_states = null;
    killImgs ();
  }
  
  protected void finalize() throws Throwable 
  {super.finalize();}
  
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
