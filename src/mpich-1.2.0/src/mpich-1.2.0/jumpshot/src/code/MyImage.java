import java.awt.*;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.event.*;
import java.lang.Math;
import com.sun.java.swing.*;
import java.awt.image.ImageObserver;

/**
 * this is the class that contains the offscreen images on which the jumpshot data
 * is drawn. 2 offscreen images are used, one for states and arrows (img) and the
 * other for time line (timeImg). Functions for drawing rectangles and arrows are
 * present here. 
 */
class MyImage {
  ProgramCanvas parent;

  int imgIdH;     //id of this object 
  double begT;    //staring time of this object
  double endT;    //ending time of this object
  int _xPix;      //width of the entire image in pixels
  int _yPix;      //height of the entire image in pixels
  
  Image rulerImg;  //offscreen image for displaying the time ruler
  
  int tx;         //Used to prevent drawing thin states repeatedly over one another.
  
  //Configuration variables for the arrows.
  public Color printLineColor, normLineColor;
  public Color circleColor;             //Color of the circle of arrow
  public Color hiColor = Color.red;     //Arrow ring Color
  public Color normColor = Color.white; //Arrow Color
  public int lrad = 1;                  //Radius of the little cirle
  public int brad = 3;                  //Radius of the bigger circle
  public int angle = 40;                //central angle of the arrow head
  public int ht = 20;                   //height of the arrow head
  
  public int max = 8; //The maximum size of the time string in time ruler
  
  private int numDisp;
  private Display [] dispA;
  private Vector dtypeV;
  
  public MyImage (int idH, int x, int y, Vector d, ProgramCanvas p) {
    imgIdH = idH;
    _xPix = x; _yPix = y;
    parent = p;
    dtypeV = d;
    normLineColor = normColor; circleColor = hiColor; printLineColor = Color.black;
    setupDisps (); 
  }
  
  public MyImage (int x, int y, Vector d, ProgramCanvas p) {
    _xPix = x; _yPix = y;
    parent = p;
    dtypeV = d; numDisp = d.size ();
    normLineColor = normColor; circleColor = hiColor; printLineColor = Color.black;
  }
  
  void paintStuff1 (int x, Graphics g, ImageObserver io) {
    g.drawImage (rulerImg, x, parent.allDispHt, io);
    
    for (int i = 0; i < numDisp; i++) g.drawImage (dispA [i].img1, x, i * parent.eachDispHt, io);
  }
  
  void paintStuff2 (int x, Graphics g, ImageObserver io) {
    g.drawImage (rulerImg, x, parent.allDispHt, io);
    
    for (int i = 0; i < numDisp; i++) {
      if (dispA [i].dtype == CONST.TIMELINES) 
	g.drawImage (dispA [i].img2, x, i * parent.eachDispHt, io);
      else 
	g.drawImage (dispA [i].img1, x, i * parent.eachDispHt, io);
    }
  }
  
  private void setupDisps () {
    numDisp = dtypeV.size ();
    
    //Get offscreen images from parent
    rulerImg = parent.getOffScreenImage (_xPix, parent.rulerHt);
    
    dispA = new Display [numDisp];
    
    for (int i = 0; i < numDisp; i++) {
      int dtype = ((Integer)dtypeV.elementAt (i)).intValue ();
      
      Image im1 = parent.getOffScreenImage (_xPix, parent.eachDispHt), im2;
      
      if (dtype == CONST.TIMELINES) im2 = parent.getOffScreenImage (_xPix, parent.eachDispHt);
      else im2 = null;
      
      dispA [i] = new Display (dtype, im1, im2);
    }
  }

  /**
   * set time values and draw stuff
   */
  void drawRegion (double b, double e) {
    begT = b; endT = e;
    drawStuff ();
  }
  
  /**
   * set starting and ending time values
   */
  void setTimes (double b, double e) {begT = b; endT = e;}
  
  /**
   * draw all things in the offscreen images
   */
  void drawStuff () {
    for (int i = 0; i < numDisp; i++) 
      if (dispA [i].dtype == CONST.TIMELINES) drawTimeLines (dispA [i]);
      else drawMtnRanges (dispA [i]);
    
    Graphics g = rulerImg.getGraphics (); drawTimeRuler (g); g.dispose ();
  }
  
  void printStuff (Graphics g) {
    for (int i = 0; i < numDisp; i++) {
      if (((Integer)dtypeV.elementAt (i)).intValue () == CONST.TIMELINES) drawTimeLines (g);
      else drawMtnRanges (g);
      g.translate (0, parent.eachDispHt);
    }
    
    drawTimeRuler (g);
  }
  
  private void drawTimeLines (Display d) {
    Graphics g1 = d.img1.getGraphics (), g2 = d.img2.getGraphics ();
    
    //Image Background
    g1.setColor (parent.parent.normImgBColor); g2.setColor (parent.parent.normImgBColor);
    g1.fillRect (0, 0, _xPix, parent.eachDispHt); g2.fillRect (0, 0, _xPix, parent.eachDispHt);
    
    //Top and bottom borders
    g1.setColor (parent.parent.rulerColor); g2.setColor (parent.parent.rulerColor);
    g1.fill3DRect (0, 0, _xPix, parent.hBWt, true); g2.fill3DRect (0, 0, _xPix, parent.hBWt, true);
    g1.fill3DRect (0, parent.eachDispHt - parent.hBWt, _xPix, parent.hBWt, true);
    g2.fill3DRect (0, parent.eachDispHt - parent.hBWt, _xPix, parent.hBWt, true);

    drawStates (g1, g2);
    drawArrows (g1, g2);
  
    g1.dispose (); g2.dispose ();
  }
  
  private void drawTimeLines (Graphics g) {
    //Image background
    g.setColor (parent.parent.printImgBColor);
    g.fillRect (0, 0, _xPix, parent.eachDispHt);
    
    //Top and bottom borders
    g.setColor (parent.parent.rulerColor);
    g.fill3DRect (0, 0, _xPix, parent.hBWt, true);
    g.fill3DRect (0, parent.eachDispHt - parent.hBWt, _xPix, parent.hBWt, true);

    drawStates (g, null);
    drawArrows (g, null);
  }
  
  private void drawMtnRanges (Display d) {
    Graphics g = d.img1.getGraphics ();
    drawMtnRanges (g);
    g.dispose ();
  }
  
  private void drawMtnRanges (Graphics g) {
    //Image background
    if (g instanceof PrintGraphics) g.setColor (parent.parent.printImgBColor);
    else g.setColor (parent.parent.normImgBColor);
    g.fillRect (0, 0, _xPix, parent.eachDispHt);
    
    //Top and bottom borders
    g.setColor (parent.parent.rulerColor);
    g.fill3DRect (0, 0, _xPix, parent.hBWt, true);
    g.fill3DRect (0, parent.eachDispHt - parent.hBWt, _xPix, parent.hBWt, true);
    
    drawMtns (g);
  }
  
  /**
   * draw all states
   */
  private void drawStates (Graphics g1, Graphics g2) {
    Vector procVector = parent.procVector;
    Enumeration enum = procVector.elements ();
    double py;
    
    while (enum.hasMoreElements ()) {
      JProcess currProc = (JProcess)enum.nextElement ();
      
      if (currProc.dispStatus) {
	py = parent.getProcYCord (parent.getIndex (parent.procVector, currProc.procId));
	draw (currProc, py, g1, g2); 
	
        Vector a = currProc.procStateVector;
	tx = -1;

	for (int i = a.size () - 1; i >= 0; i--) {
          ProcessState currEvt = (ProcessState)(a.elementAt (i));
          if (currEvt.info.stateDef.checkbox.isSelected ())
	    if (!(currEvt.info.endT < begT || currEvt.info.begT > endT)) {
	      if (currEvt.info.blink) parent.blink = true;
	      draw (currEvt, py, g1, g2);
	    }
	}
      } 
    }
  }

  /**
   * draw mountain ranges
   */
  private void drawMtns (Graphics g) {
    Vector mtns = parent.parent.mtns;
    Enumeration enum = mtns.elements ();
    tx = -1;
    
    while (enum.hasMoreElements ()) {
      MtnInfo currMtn = (MtnInfo)enum.nextElement ();
      
      if (!(currMtn.endT < begT || currMtn.begT > endT)) {
	double lenT = endT - begT;
	double timeDiff = currMtn.begT - begT;
	double effT = (timeDiff > 0)? timeDiff : 0.0;
	
	double t;
	if ((currMtn.endT - begT) > lenT) t = lenT;
	else t = (currMtn.endT - begT);
	
	int width = parent.getW (effT, t);
	
	int x = parent.getEvtXCord (effT);
	int l = parent.totalProc;
	
	//	if (!(x == tx && (width == 0 || width == 1))) {
	for (int i = 0; i < currMtn.s.length; i++) {
	  CLOG_STATE state = ((CLOG_STATE)parent.parent.stateDefs.elementAt (i));
	  if (currMtn.s [i] > 0 && state.checkbox.isSelected ()) {
	    if ((l - currMtn.s [i]) < 0) break;
	      
	    double ycord = parent.hBWt + ((parent.eachDispIHt / (double)parent.totalProc) * 
					  (l - currMtn.s [i]));
	    double limit = parent.hBWt + ((parent.eachDispIHt / (double)parent.totalProc) * l) - 1;
	    
	    double height = limit - ycord + 1;
	    if (height == 0) height = 1;
	      
	    double d = Math.rint (ycord);
	    double diff = d - ycord;
	      
	    int yc = (int)d; 
	    int ht = (int)Math.rint (height - diff); 
	      
	    if (width == 0) width = 1;
	      
	    g.setColor (state.color);
	    g.fillRect (x, yc, width, ht);
	    
	    l -= currMtn.s [i];
	  }
	}
	//	}
	tx = x;
      }
    }
  }

  /**
   * draw arrows
   */
  private void drawArrows (Graphics g1, Graphics g2) {
    if (!parent.arrowDispStatus) return;
    Enumeration enum = parent.parent.quiver.arrowVector.elements ();
    while (enum.hasMoreElements ()) {
      ArrowInfo arrow = (ArrowInfo)enum.nextElement ();
      if (check (arrow, begT, endT)) {
	draw (arrow, g1);
	if (arrow.blink) parent.blink = true;
	if (g2 != null && !arrow.blink) draw (arrow, g2);
      }
    }  
  }
  
  /**
   * Draws the given process state in this image.
   */
  private void draw (ProcessState ps, double py, Graphics g1, Graphics g2) {
    double timeDiff = ps.info.begT - begT;
    double effT = (timeDiff > 0)? timeDiff : 0.0;
    double lenT = endT - begT;
    int maxW = parent.getW (effT, lenT);
    int w = parent.getW (effT, ps.info.endT - begT);
    if (w > maxW) w = maxW;
    int x = parent.getEvtXCord (effT);
    
    if (!(x == tx && (w == 0 || w == 1))) {
      boolean startflag = (ps.info.begT >= begT)? true : false;
      boolean endflag = (ps.info.endT <= endT)? true : false;
      
      double diff = ps.info.level * parent.yDensity * parent.nestFactor;
      double height = parent.yDensity - diff; 
      int y = (int)Math.rint (py - height / 2.0);
      int h = (int)Math.rint (height); 
      
      //Done so that when state dialog has to pop up it uses these values
      ps.y = y; ps.h = h; if (ps.h == 0) ps.h = 1;
      
      Color c = ps.info.stateDef.color;
      
      displayRect (g1, c, x, y, w, h, startflag, endflag);
      if (g2 != null && !ps.info.blink)
	displayRect (g2, c, x, y, w, h, startflag, endflag);
    }
    tx = x;
  }
  
  private void displayRect (Graphics g, Color c, int x, int y, int w, int h, 
			    boolean startflag, boolean endflag) {
    g.setColor (c); 
    if (w == 0) w = 1; if (h == 0) h = 1;
    g.fillRect (x, y, w, h);
    
    g.setColor (Color.white);
    g.drawLine (x, y, x + w - 1, y);
    g.drawLine (x, y + h - 1, x + w - 1, y + h - 1);
    
    if (startflag) g.drawLine (x, y, x, y + h - 1);
    if (endflag) g.drawLine (x + w - 1, y, x + w - 1, y + h - 1);
  }
  
  /**
   * Draws Process time Line
   */
  private void draw (JProcess p, double py, Graphics g1, Graphics g2) {
    int y = (int)Math.rint (py); 
    
    //Used for locating arrows
    p.y = y;
    
    g1.setColor (Color.red);
    g1.drawLine (0, y, _xPix - 1, y); 

    if (g2 != null) {
      g2.setColor (Color.red);
      g2.drawLine (0, y, _xPix - 1, y); 
    }
  }
  
  /**
   * Check whether the given arrow should be drawn in this image or not.
   */
  private boolean check (ArrowInfo arrow, double b, double e) {
    if (arrow.endT >= b && arrow.begT <= e) {
      int startIndex = parent.getIndex (parent.procVector, arrow.begProcId);
      int endIndex = parent.getIndex (parent.procVector, arrow.endProcId);
      int size = parent.procVector.size ();
      if (startIndex < size && endIndex < size) {
        if (((JProcess)parent.procVector.elementAt (startIndex)).dispStatus &&
            ((JProcess)parent.procVector.elementAt (endIndex)).dispStatus) 
          return true;
      }
    }
    return false;
  }
  
  /**
   * Draw the arrow representing a message in the given image.
   */
  private void draw (ArrowInfo arrow, Graphics g) {
    int x1 = parent.getEvtXCord (arrow.begT - begT);
    int y1 = (int)Math.rint 
      (parent.getProcYCord (parent.getIndex (parent.procVector, arrow.begProcId)));
    int x2 = parent.getEvtXCord (arrow.endT - begT);
    int y2 = (int)Math.rint 
      (parent.getProcYCord (parent.getIndex (parent.procVector, arrow.endProcId)));
    
    //Drawing arrow line
    if (g instanceof PrintGraphics) g.setColor (printLineColor);
    else g.setColor (normLineColor);
    
    g.drawLine (x1, y1, x2, y2);
    
    //Draw circle which gives info. about message
    g.setColor (circleColor);
    g.fillOval (x1 - lrad, y1 - lrad, lrad * 2, lrad * 2);

    brad = (int)Math.rint (parent.yDensity / 2);
    if (brad > 10) brad = 10;
    if (brad < 3) brad = 3;
    g.drawOval (x1 - brad, y1 - brad, brad * 2, brad * 2);

    //Draw arrowhead
    double halfangle = (Math.PI / 360.0) * angle;
    double dx = (double)Math.abs(x2 - x1);
    double dy = (double)Math.abs (y2 - y1);
    double a1 = Math.atan (dy / dx);
    double radtodeg = 180.0 / Math.PI;
    int startangle = 0;
    
    if (y1 < y2 && x2 > x1)      //Forward : Downward
      startangle = (int)Math.rint (radtodeg * (Math.PI  - a1 - halfangle));
    else if (y1 > y2 && x2 > x1) //Forward : Upward
      startangle = (int)Math.rint (radtodeg * (Math.PI + a1 - halfangle));
    else if (y1 > y2 && x1 > x2) //Backward : Upward
      startangle = (int)Math.rint (radtodeg * ((2.0 * Math.PI) - a1 - halfangle));
    else if (y2 > y1 && x1 > x2) //Backward : Downward
      startangle = (int)Math.rint (radtodeg * (a1 - halfangle));
    else if (y1 == y2) {
      startangle = (int)Math.rint (radtodeg * (Math.PI - halfangle));
      if (x1 > x2) startangle += 180;
    }
    else if (x1 == x2) {
      startangle = (int)Math.rint (radtodeg * (Math.PI / 2.0 - halfangle));
      if (y1 > y2) startangle += 180;
    }
    
    if (g instanceof PrintGraphics) g.setColor (printLineColor);
    else g.setColor (normLineColor);
    
    ht = (int)Math.rint (parent.yDensity);
    if (ht > 20) ht = 20;
    g.fillArc (x2 - ht, y2 - ht, ht * 2, ht * 2, startangle, angle);
  }
  
  /**
   * checks whether the given time is valid for this image
   */
  private boolean check (double time, double b, double e) 
  {return (time >= b && time <= e)? true : false;}
  
  /**
   * Draw ruler in specified graphics context
   */
  void drawTimeRuler (Graphics g) {
    g.setColor (parent.parent.rulerColor);
    
    g.fillRect (0, 0, _xPix, parent.rulerHt);
    g.setColor (Color.black); g.drawLine (0, 0, _xPix, 0);
    
    double inchT = parent.getTime (parent.dpi); if (inchT <= 0) return;
    int i = (int)Math.rint (begT / inchT);
    
    //Start time
    double t = i * inchT;

    while (t < endT && t < parent.maxT) {
      int xcord = i * parent.dpi - parent.getEvtXCord (begT);
     
      String t1 = (new Float (t)).toString (), t2 = null;
      
      if (t1.indexOf ('E') == -1) {
	int index = max;
	if (index > t1.length ()) index = t1.length ();
	t2 = t1.substring (0, index);
      }
      else {
	int exp = t1.indexOf ('E');
	String e = t1.substring (exp, t1.length ());
	
	int si = 5; if (exp < si) si = exp;
	String a = t1.substring (0, si);
	
	t2 = a + e;
      }

      g.drawString ("|", xcord - 2, parent.lineSize - parent.fDescent);
      g.drawString (t2,
		    xcord - (int)Math.rint ((double)parent.fm.stringWidth (t2) / 2.0),
		    2 * parent.lineSize - parent.fDescent);
      t = (++i * inchT);
    }
  }  
  
  void kill () {
    for (int i = 0; i < dispA.length; i++) {
      if (dispA [i].img1 != null) {dispA [i].img1.flush (); dispA [i].img1 = null;}
      if (dispA [i].img2 != null) {dispA [i].img2.flush (); dispA [i].img2 = null;}
    }
    
    if (rulerImg != null) rulerImg.flush (); rulerImg = null;
  }
  
  protected void finalize() throws Throwable {super.finalize();}
}

class Display {
  Image img1;
  Image img2;
  int dtype;

  public Display (int d, Image i1, Image i2) {
    dtype = d;
    img1 = i1;
    img2 = i2;
  }
}
