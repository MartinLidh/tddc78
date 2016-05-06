import java.awt.*;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.event.*;
import javax.swing.JMenuItem;
import javax.swing.JSlider;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JComponent;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
  On this canvas all drawing of the histograms takes place. An offscreen image
  is used as a double buffer. the bins and time scale are drawn onto this
  offscreen image which is then placed onto this canvas. The size of the image
  is that of this canvas.
  */
public class HistCanvas extends JComponent
implements ComponentListener, AdjustmentListener, ActionListener, ChangeListener {
  public Histwin parent;
  RecDef stateDef;
  Vector stateVector;

  double xDensity;        //horizontal pixels / second
  double yDensity;        //vertical pixels / state count
  double leastT;          //length of the shortest state
  double maxT;            //length of the largest state
  double begT;            //current starting length in the canvas
  double endT;            //current ending length in the canvas  
  int _xPix;              //width of the offscreen image
  int _yPix;              //height of the offscreen image
  int widthCan;           //width of the canvas
  int heightCan;          //height of the canvas 
  Color canBColor = Color.black; //Background color for the canvas
  public HistImage view;         //object containing the offscreen image
  
  boolean setupComplete = false;

  public int lineSize, dpi, fDescent;
  public FontMetrics fm;
  int numBins, maxNumBins;
  Font imgFont; 
  
  //Used for resizing the bars
  private int ty;                 //previous vertical position when the mouse was pressed
  private int eff_yPix;           
  public int topGap;      //gap to be subtracted from heightCan to get the appropriate
  //bin height when the command Resize to Fit is given Histwin
  
  private double zoomFac = 2.0;
  
  //parent.hbar values
  private int hbarMax, hbarVal, hbarVis;
  
  private int hPixMax;  
  private int regionsCt = 0;
  
  public HistCanvas (Histwin p) {super (); parent = p;}
  
  void init () {
    waitCursor ();
    setupData ();
    adjustCanvasStuff ();
    ResizeCanvas ();
    normalCursor ();
  }
  
  //setup methods---------------------------------------------------------------
  private void setupData () {
    stateDef = parent.stateDef; 
    stateVector = stateDef.stateVector;
    sortLen ();
    leastT = ((Info)stateVector.elementAt (0)).lenT;
    maxT = ((Info)stateVector.elementAt (stateVector.size () - 1)).lenT;
    
    if (leastT == maxT) maxT += (1.0 / 200); 
    begT = leastT; endT = maxT;
    numBins = 100; 
    maxNumBins = 200;
    this.addComponentListener (this);
    
    //Set Histwin's fields
    parent.cursorField.setText ("0.0");
    parent.stateNameLabel.setText ("State: " + stateDef.description);
    parent.numInstField.setText (Integer.toString (stateVector.size ()));
    parent.leastLenField.setText ((new Float (leastT)).toString ());
    parent.maxLenField.setText ((new Float (maxT)).toString ());
    parent.numBinsField.setText (Integer.toString (numBins));
    parent.maxNumBinsField.setText (Integer.toString (maxNumBins));
    parent.binSlider.setOrientation (JSlider.HORIZONTAL); 
    parent.binSlider.setMaximum (maxNumBins); parent.binSlider.setMinimum (1);
    parent.binSlider.setValue (numBins);
  }
 
  private void adjustCanvasStuff () {
    imgFont = new Font ("Serif", Font.PLAIN, 10);
    setFont (imgFont);
    fm = getToolkit ().getFontMetrics (imgFont);
    lineSize = fm.getHeight (); fDescent = fm.getDescent ();
    dpi = getToolkit ().getScreenResolution ();
    setBackground (canBColor);
    this.enableEvents (AWTEvent.MOUSE_MOTION_EVENT_MASK | 
                       AWTEvent.MOUSE_EVENT_MASK );
  }
  //end of setup methods----------------------------------------------------------
  
  public void paintComponent (Graphics g) {g.drawImage (view.img, 0, 0, this);}

  /**
   * Allocate memory for the offscreen image
   */
  private void setupImg () {
    if (view != null) view.kill ();
    Image im = createImage (_xPix, _yPix);
    view = new HistImage (im, _xPix, _yPix, this);
  }
  
  int getEvtXCord (double t) {
    double d = t * xDensity;
    return (int)(Math.rint (d));
  }

  double getTime (int pos) {
    double time = (1 / xDensity) * pos;
    return time;
  }
  
  int getW (double t1, double t2) {
    double d = (t2 - t1) * xDensity;
    return  (int)(Math.rint (d));
  }
  
  //Return the height in pixels of the given number of state count
  int getHistHeight(int ct) {return (int)Math.rint (yDensity * ct);}

  /**
   * This functions sorts the states in the vector in the order of their time lengths
   */
  private void sortLen () {
    ROUTINES.QuickSort (stateVector, 0, stateVector.size () - 1);
  }
  
  /**
   * reset the heights of the bins in display proportionately
   */
  private void reFit () {
    waitCursor ();
    yDensity = (_yPix - 3 * lineSize - topGap) / (double)view.highestCt;
    eff_yPix = (int)Math.rint (yDensity * stateVector.size ());
    view.drawRegion (begT, endT); repaint ();
    normalCursor ();
  }
   
  /**
   * change the number of bins in display
   */
  private void changeNumBins (int x) {
    numBins = x;
    view.drawStuff ();
    repaint ();
  }
  
  /**
   * Mark states of interest in Histogram display so that they blink
   */
  private void markBlink () {
    Enumeration enum = stateVector.elements ();
    
    while (enum.hasMoreElements ()) {
      Info currState = (Info)enum.nextElement ();
      
      if (currState.lenT >= begT && currState.lenT <= endT)
	currState.blink = true;
    }
  }
  
  /**
   * unmark previously marked states so that they do not blink
   */
  private void unmarkBlink (double b, double e) {
    Enumeration enum = stateVector.elements ();
    
    while (enum.hasMoreElements ()) {
      Info currState = (Info)enum.nextElement ();
      
      if (currState.lenT >= begT && currState.lenT <= endT)
	currState.blink = false;
    }
  }
  
  /**
   * unmark previously marked states so that they do not blink
   */
  void unmarkBlinkAll () {
    Enumeration enum = stateVector.elements ();
    
    while (enum.hasMoreElements ()) ((Info)enum.nextElement ()).blink = false;
  }
  
  //Event Handler methiods--------------------------------------------------------
   
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    
    if (command.equals ("Change numBins")) {
      int x;
      
      try {x = Integer.parseInt (parent.numBinsField.getText ());}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid integer required"); 
	parent.numBinsField.setText (Integer.toString (parent.binSlider.getValue ()));
	return;
      }
      
      if (x < 1 || x > maxNumBins) {
	new ErrorDiag (null, "Value must be >= 1 and =< " + maxNumBins); 
	parent.numBinsField.setText (Integer.toString (parent.binSlider.getValue ()));
	return;
      }
      parent.binSlider.setValue (x); changeNumBins (x);
    }
    else if (command.equals ("Change maxNumBins")) {
      int x;
      
      try {x = Integer.parseInt (parent.maxNumBinsField.getText ());}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid integer required"); 
	parent.maxNumBinsField.setText (Integer.toString (maxNumBins));
	return;
      }
      
      if (x < 1) {
	new ErrorDiag (this, "Value must be >= 1"); 
	parent.maxNumBinsField.setText (Integer.toString (maxNumBins));
	return;
      }
      maxNumBins = x; parent.binSlider.setMaximum (x); 
      int y = parent.binSlider.getValue (); if (y > x) y = x; parent.binSlider.setValue (y);
    }
    if (command.equals ("Change Start Len")) {
      double d;
      
      try {d = (Double.valueOf (parent.startLenField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required"); 
	parent.startLenField.setText ((new Float (begT)).toString ());
	return;
      }
      
      if (d < leastT || d >= endT) {
	new ErrorDiag (this, "Value must be >= " + (new Float (leastT)).toString () + 
		       " and < " + (new Float (endT)).toString ());
	parent.startLenField.setText ((new Float (begT)).toString ());
	return;
      }
      begT = d;
      
      xDensity = _xPix / (endT - begT);
      
      view.drawRegion (begT, endT); repaint ();
      
      calcHPixMax ();
      calcHbarMax ();
      calcHbarVal ();
      setHbarValues ();
    }
    else if (command.equals ("Change End Len")) {
      double d;
      try {d = (Double.valueOf (parent.endLenField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required"); 
	parent.endLenField.setText ((new Float (endT)).toString ());
	return;
      }
      
      if (d <= begT || d > maxT) {
	new ErrorDiag (this, "Value must be > " + (new Float (begT)).toString () + 
		       " and =< " + (new Float (maxT)).toString ());
	parent.endLenField.setText ((new Float (endT)).toString ());
	return;
      }
      endT = d;
      
      xDensity = _xPix / (endT - begT);
      
      view.drawRegion (begT, endT); repaint ();
      
      calcHPixMax ();
      calcHbarMax ();
      calcHbarVal ();
      setHbarValues ();
    }
    else if (command.equals ("Change Zoom Fac")) {
      double d;
      
      try {d = (Double.valueOf (parent.zFacField.getText ())).doubleValue ();}
      catch (NumberFormatException e) {
	new ErrorDiag (this, "Valid floating point value required"); 
	parent.zFacField.setText (Double.toString (zoomFac));
	return;
      }
      
      if (d < 1) {
	new ErrorDiag (this, "Value must be >= 1"); 
	parent.zFacField.setText (Double.toString (zoomFac));
	return;
      }
      zoomFac = d;
    }
    else if (command.equals ("In")) zoomInH ();
    else if (command.equals ("Out")) zoomOutH ();
    else if (command.equals ("Reset")) resetView ();
    else if (command.equals ("Resize to fit")) reFit ();
    else if (command.equals ("Blink states")) {
      regionsCt++;
      JRadioButtonMenuItem rb = new JRadioButtonMenuItem (begT + " to " + endT);
      rb.setSelected (true); rb.addActionListener (this); parent.regionsMenu.add (rb);
      markBlink (); parent.parent.canvas.Refresh ();
    }
    else if (evt.getSource () instanceof JRadioButtonMenuItem) {
      parent.regionsMenu.remove ((JMenuItem)evt.getSource ());
      String b = command.substring (0, command.indexOf (' '));
      String e = command.substring (command.lastIndexOf (' ') + 1, command.length ());
      
      unmarkBlink ((new Double (b)).doubleValue (), (new Double (e)).doubleValue ());
      parent.parent.canvas.Refresh ();
    }
  }
  
  private void obtainArea (int a, boolean top) {
    double zscore;
    
    switch (a) {
    case 1: zscore = 2.3268; break;
    case 5: zscore = 1.6452; break;
    case 10: zscore = 1.2817; break;
    case 20: zscore = 0.8; break;
    case 30: zscore = 0.524; break;
    case 50: zscore = 0; break;
    default: new ErrorDiag (null, "Region has not been implemented"); return;
    }
    
    if (!top) zscore = zscore * -1;
    
    double m  = getMeanOfStateLens ();
    double st = getStDevOfStateLens (m);
    
    //Using the st. normal dist. transformation z = (X - m) / st
    double X = zscore * st + m;
    
    if (X >= maxT && top) {
      new ErrorDiag (null, "No states with duration > mu(" + (new Float (m)).toString () + 
		     ") + " + zscore + "sigma(" + (new Float (st)).toString () + ")");
      return;
    }
    else if (X <= leastT && !top) {
      new ErrorDiag (null, "No states with duration < mu(" + (new Float (m)).toString () + 
		     ") + " + zscore + "sigma(" + (new Float (st)).toString () + ")");
      return;
    }

    endT = maxT; begT = leastT; 
    
    if (top) {
      if (stateVector.size () > 1 && X >= leastT) begT = X;
    }
    else
      if (stateVector.size () > 1 && X <= maxT) endT = X;
    
    xDensity = _xPix / (endT - begT);
     
    view.drawRegion (begT, endT); repaint ();
    
    calcHPixMax ();
    calcHbarMax ();
    calcHbarVal ();
    setHbarValues ();
    
    //set Field values
    parent.startLenField.setText ((new Float (begT)).toString ());
    parent.endLenField.setText ((new Float (endT)).toString ());
    parent.statesInViewField.setText (Integer.toString (view.statesDrawn));
    parent.pcInViewField.setText ((new Float (view.statesDrawn * 100 / 
					      stateVector.size ())).toString ());
  }
  
  private double getMeanOfStateLens () {
    Enumeration enum = stateVector.elements ();
    double sum = 0;
    
    while (enum.hasMoreElements ()) {
      Info currState = (Info)enum.nextElement ();
      sum += currState.lenT;
    }
    
    return (sum / stateVector.size ());
  }
  
  private double getStDevOfStateLens (double m) {
    Enumeration enum = stateVector.elements ();
    double sum = 0;
    
    while (enum.hasMoreElements ()) {
      Info currState = (Info)enum.nextElement ();
      sum += ((currState.lenT - m) * (currState.lenT - m));
    }
        
    return Math.sqrt (sum / stateVector.size ());
  }
  
  public void componentResized (ComponentEvent e) {ResizeCanvas ();}
  
  public void adjustmentValueChanged (AdjustmentEvent e) {
    int x = parent.hbar.getValue ();
    int v = parent.hbar.getVisibleAmount ();
    
    begT = leastT + (maxT - leastT) * (double)x / hbarMax;
    endT = leastT +  (maxT - leastT) * (double)(x + v) / hbarMax;
        
    view.drawRegion (begT, endT); repaint ();
    
    parent.startLenField.setText ((new Float (begT)).toString ());
    parent.endLenField.setText ((new Float (endT)).toString ());
    parent.statesInViewField.setText (Integer.toString (view.statesDrawn));
    parent.pcInViewField.setText ((new Float (view.statesDrawn * 100 / 
					      stateVector.size ())).toString ());
    parent.zFacField.setText ((new Float (zoomFac)).toString ());
  }
  
 
  public void stateChanged (ChangeEvent e) {
    JSlider source = (JSlider)e.getSource ();
    
    if (source == parent.binSlider) {
      int numBins = parent.binSlider.getValue ();
      parent.numBinsField.setText (Integer.toString (numBins));
      parent.maxNumBinsField.setText (Integer.toString (maxNumBins));
      changeNumBins (numBins);
    }
  }
  
  public void ResizeCanvas () {
    waitCursor ();
    
    Dimension dim = getSize ();

    widthCan = dim.width;
    topGap = parent.getInsets ().top;
    heightCan = dim.height;
    
    _xPix = widthCan;
    _yPix = heightCan;
    
    if (_yPix < 1) _yPix = 1;
    if (_xPix < 1) _xPix = 1;
    
    //will be moved from here
    if (!setupComplete) {
      setupComplete = true;
      eff_yPix = _yPix;
    }
    
    xDensity = widthCan / (endT - begT);
    yDensity = (_yPix - 3.0 * lineSize) / (double)stateVector.size ();
  
    setupImg ();
    view.drawRegion (begT, endT); repaint ();
    
    //Modify hbar values
    calcHPixMax ();
    calcHbarMax ();
    calcHbarVal ();
    setHbarValues ();
    
    normalCursor ();
  }
  
  private void calcHPixMax () {hPixMax = getW (leastT, maxT);}
  private void calcHbarMax () {hbarMax = hPixMax;}
  
  private void calcHbarVal () {  
    double d = (begT - leastT) * xDensity;
    hbarVal = (int)Math.rint (d);
  }
  
  private void setHbarValues () {
    parent.hbar.setValues (hbarVal, _xPix, 0, hbarMax);
  }
  
  private void resetView () {
    waitCursor ();
    
    begT = leastT; endT = maxT;
    
    xDensity = _xPix / (endT - begT);
    
    view.drawRegion (begT, endT); repaint ();
    
    calcHPixMax ();
    calcHbarMax ();
    calcHbarVal ();
    setHbarValues ();

    //set Field values
    parent.startLenField.setText ((new Float (begT)).toString ());
    parent.endLenField.setText ((new Float (endT)).toString ());
    parent.statesInViewField.setText (Integer.toString (view.statesDrawn));
    parent.pcInViewField.setText ((new Float (view.statesDrawn * 100 / 
							stateVector.size ())).toString ());
    normalCursor ();
  }
  
  private void zoomInH () {
    waitCursor ();
    
    double centerT = begT + (endT - begT) / 2;
    
    begT = centerT - (centerT - begT) / zoomFac;
    endT = centerT + (endT - centerT) / zoomFac;
    
    xDensity = _xPix / (endT - begT);
    
    view.drawRegion (begT, endT); repaint ();
    
    calcHPixMax ();
    calcHbarMax ();
    calcHbarVal ();
    setHbarValues ();

    //set Field values
    parent.startLenField.setText ((new Float (begT)).toString ());
    parent.endLenField.setText ((new Float (endT)).toString ());
    parent.statesInViewField.setText (Integer.toString (view.statesDrawn));
    parent.pcInViewField.setText ((new Float (view.statesDrawn * 100 / 
							stateVector.size ())).toString ());
    normalCursor ();
  }
  
  private void zoomOutH () {
    waitCursor ();

    double centerT = begT + (endT - begT) / 2;
    begT = centerT - (centerT - begT) * zoomFac;
    if (begT < leastT) begT = leastT;
    endT = centerT + (endT - centerT) * zoomFac;
    if (endT > maxT) endT = maxT;
    
    xDensity = _xPix / (endT - begT);
    
    view.drawRegion (begT, endT); repaint ();
    
    //set Hbar values
    calcHPixMax ();
    calcHbarMax ();
    calcHbarVal ();
    setHbarValues ();
    
    //set Field values
    parent.startLenField.setText ((new Float (begT)).toString ());
    parent.endLenField.setText ((new Float (endT)).toString ());
    parent.statesInViewField.setText (Integer.toString (view.statesDrawn));
    parent.pcInViewField.setText ((new Float (view.statesDrawn * 100 / 
							stateVector.size ())).toString ());
    
    normalCursor ();
  }
  
  /**
   * handles events when the mouse is moved or dragged
   */
  public void processMouseMotionEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_MOVED)
      adjustTimeField (e.getX ());
    if (e.getID () == MouseEvent.MOUSE_DRAGGED) {
      int y = e.getY ();
      int dy = ty - y;
      eff_yPix += dy;
      yDensity = (eff_yPix - 3.0 * lineSize) * 1.0/ (double)stateVector.size ();
      view.drawRegion (begT, endT); repaint ();
      ty = y;
    }
    else super.processMouseMotionEvent (e);
  }
  
  /**
   * handles events when the mouse is pressed
   */
  public void processMouseEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_PRESSED) ty = e.getY ();
    else if (e.getID () == MouseEvent.MOUSE_RELEASED) ty = e.getY ();
    else super.processMouseMotionEvent (e);
  }
  
  int print (Graphics g, int x, int y, int width, int height) {
    FontMetrics tfm = g.getFontMetrics ();
    
    g.drawString ("State '" + stateDef.description + "' length distribution",
		  x, y + tfm.getHeight () - tfm.getDescent ());
    
    g.drawString (Integer.toString (view.statesDrawn) + " drawn out of " + 
		  Integer.toString (stateVector.size ()) + " (" + 
		  (new Float (view.statesDrawn * 100.0 / (double)stateVector.size ())).toString ()
		  + "%)", 
		  x, y + 2 * tfm.getHeight () - tfm.getDescent ());
    
    return 3 * tfm.getHeight ();
  }
  
  //end of event handler methods---------------------------------------------------
  private void adjustTimeField (int x) {
    parent.cursorField.setText ((new Float (view.begT + getTime (x))).toString ());
  }
  
  /**
   * sets the current cursor to the WAIT_CURSOR type
   */
  void waitCursor () {setCursor (new Cursor (Cursor.WAIT_CURSOR));} 
  
  /**
   * sets the WAIT_CURSOR to cursor associated with this canvas
   */
  void normalCursor () {setCursor (new Cursor (Cursor.CROSSHAIR_CURSOR));}

  /** Unused methods of the ComponentListener interface. */
  public void componentHidden (ComponentEvent e) {;}
  public void componentMoved (ComponentEvent e) {;}
  public void componentShown (ComponentEvent e) {;}
  
  void kill () {
    if (view != null) {view.kill (); view = null;}
  }

  protected void finalize () throws Throwable {super.finalize ();}
  
  ActionListener tAL  = new ActionListener () {
    public void actionPerformed (ActionEvent e) { 
      String command = e.getActionCommand ();
      if (command.endsWith ("%")) {
	int val = Integer.parseInt (command.substring (0, command.length () - 1));
	parent.areaField.setText (command);
	obtainArea (val, true);
      }
    }
  };
  
  ActionListener bAL  = new ActionListener () {
    public void actionPerformed (ActionEvent e) {   
      String command = e.getActionCommand ();
      if (command.endsWith ("%")) {
	int val = Integer.parseInt (command.substring (0, command.length () - 1));
	parent.bAreaField.setText (command);
	obtainArea (val, false);
      }
    }
  };
}
