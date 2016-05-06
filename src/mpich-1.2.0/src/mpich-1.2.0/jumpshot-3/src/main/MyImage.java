import java.awt.*;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.event.*;
import java.lang.Math;
import java.awt.image.ImageObserver;
import javax.swing.*;

/**
 * this is the class that contains the offscreen images on which the 
 * jumpshot data is drawn. 2 offscreen images are used, one for states
 * and arrows (img) and the other for time line (timeImg). Functions 
 * for drawing rectangles and arrows are
 * present here. 
 */
class MyImage {
  ProgramCanvas canvas;

  int imgIdH;     //id of this object 
  double begT;    //staring time of this object
  double endT;    //ending time of this object
  int _xPix;      //width of the entire image in pixels
  int _yPix;      //height of the entire image in pixels
  
  Image rulerImg; //offscreen image for displaying the time ruler
  
  // prevent drawing thin states or arrows repeatedly over one another.
  private int prev_state_beg_pos;
  private int prev_arrow_beg_pos;
  
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
    canvas = p;
    dtypeV = d;
    normLineColor = normColor;
    circleColor = hiColor;
    printLineColor = Color.black;
    setupDisps (); 
  }
  
  public MyImage (int x, int y, Vector d, ProgramCanvas p) {
    _xPix = x; _yPix = y;
    canvas = p;
    dtypeV = d; numDisp = d.size ();
    normLineColor = normColor;
    circleColor = hiColor;
    printLineColor = Color.black;
  }
  
  void paintStuff1 (int x, Graphics g, ImageObserver io) {
    g.drawImage (rulerImg, x, canvas.allDispHt, io);
    
    for (int i = 0; i < numDisp; i++)
        g.drawImage (dispA [i].img1, x, i * canvas.eachDispHt, io);
  }
  
  void paintStuff2 (int x, Graphics g, ImageObserver io) {
    g.drawImage (rulerImg, x, canvas.allDispHt, io);
    
    for (int i = 0; i < numDisp; i++) {
      if (dispA [i].dtype == CONST.TIMELINES) 
	g.drawImage (dispA [i].img2, x, i * canvas.eachDispHt, io);
      else 
	g.drawImage (dispA [i].img1, x, i * canvas.eachDispHt, io);
    }
  }
  
  private void setupDisps () {
    numDisp = dtypeV.size ();
    
    //Get offscreen images from canvas
    rulerImg = canvas.getOffScreenImage (_xPix, canvas.rulerHt);
    
    dispA = new Display [numDisp];
    
    for (int i = 0; i < numDisp; i++) {
      int dtype = ((Integer)dtypeV.elementAt (i)).intValue ();
      
      Image im1 = canvas.getOffScreenImage (_xPix, canvas.eachDispHt), im2;
      
      if (dtype == CONST.TIMELINES)
          im2 = canvas.getOffScreenImage (_xPix, canvas.eachDispHt);
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
        for ( int i = 0; i < numDisp; i++ ) 
            if ( dispA [i].dtype == CONST.TIMELINES )
                drawTimeLines (dispA [i]);
            else drawMtnRanges (dispA [i]);
    
        Graphics g = rulerImg.getGraphics();
        drawTimeRuler(g);
        g.dispose ();
    }
  
  void printStuff (Graphics g) {
    for (int i = 0; i < numDisp; i++) {
      if (((Integer)dtypeV.elementAt (i)).intValue () == CONST.TIMELINES)
          drawTimeLines (g);
      else drawMtnRanges (g);
      g.translate (0, canvas.eachDispHt);
    }
    
    drawTimeRuler (g);
  }
  
  private void drawTimeLines (Display d) {
    Graphics g1 = d.img1.getGraphics (), g2 = d.img2.getGraphics ();
    
    //Image Background
    g1.setColor (canvas.parent.normImgBColor);
    g2.setColor (canvas.parent.normImgBColor);
    g1.fillRect (0, 0, _xPix, canvas.eachDispHt);
    g2.fillRect (0, 0, _xPix, canvas.eachDispHt);
    
    //Top and bottom borders
    g1.setColor (canvas.parent.rulerColor);
    g2.setColor (canvas.parent.rulerColor);
    g1.fill3DRect (0, 0, _xPix, canvas.hBWt, true);
    g2.fill3DRect (0, 0, _xPix, canvas.hBWt, true);
    g1.fill3DRect (0, canvas.eachDispHt - canvas.hBWt,
                   _xPix, canvas.hBWt, true);
    g2.fill3DRect (0, canvas.eachDispHt - canvas.hBWt,
                   _xPix, canvas.hBWt, true);

    drawStates (g1, g2);
    drawArrows (g1, g2);
  
    g1.dispose ();
    g2.dispose ();
  }
  
  private void drawTimeLines (Graphics g) {
    //Image background
    g.setColor (canvas.parent.printImgBColor);
    g.fillRect (0, 0, _xPix, canvas.eachDispHt);
    
    //Top and bottom borders
    g.setColor (canvas.parent.rulerColor);
    g.fill3DRect (0, 0, _xPix, canvas.hBWt, true);
    g.fill3DRect (0, canvas.eachDispHt - canvas.hBWt, _xPix, canvas.hBWt, true);

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
    if (g instanceof PrintGraphics) g.setColor (canvas.parent.printImgBColor);
    else g.setColor (canvas.parent.normImgBColor);
    g.fillRect (0, 0, _xPix, canvas.eachDispHt);
    
    //Top and bottom borders
    g.setColor (canvas.parent.rulerColor);
    g.fill3DRect (0, 0, _xPix, canvas.hBWt, true);
    g.fill3DRect (0, canvas.eachDispHt - canvas.hBWt, _xPix, canvas.hBWt, true);
    
    drawMtns (g);
  }
  
  /**
   * draw all states
   */
    private void drawStates (Graphics g1, Graphics g2)
    {
        StateGroupList visible_groups = canvas.all_states.visible;
        int            grp_seq_idx;
        double         py;
        int            idx;
    
        Enumeration groups = visible_groups.elements();
        while ( groups.hasMoreElements() ) {
            StateGroup cur_grp = (StateGroup) groups.nextElement();
      
            if ( cur_grp.dispStatus ) {
                grp_seq_idx = visible_groups.GetSeqIndexAt( cur_grp.groupID );
                py = canvas.GetStateGroupYCord( grp_seq_idx );
                draw(cur_grp, py, g1, g2); 
	
                prev_state_beg_pos = -1;

                for ( idx = cur_grp.size() - 1; idx >= 0; idx-- ) {
                    StateInterval cur_state = (StateInterval)
                                              cur_grp.elementAt( idx );
                    if ( cur_state.info.stateDef.checkbox.isSelected() )
                        if ( !(    cur_state.info.endT < begT
                                || cur_state.info.begT > endT)) {
                            if ( cur_state.info.blink )
                                 canvas.blink = true;
                            draw(cur_state, py, g1, g2);
	                }
	        }
            } 
        }
     }

  /**
   * draw mountain ranges
   */
  private void drawMtns (Graphics g) {
    Vector mtns = canvas.parent.mtns;
    Enumeration enum = mtns.elements ();
    prev_state_beg_pos = -1;
    
    while (enum.hasMoreElements ()) {
      MtnInfo currMtn = (MtnInfo)enum.nextElement ();
      
      if (!(currMtn.endT < begT || currMtn.begT > endT)) {
	double lenT = endT - begT;
	double timeDiff = currMtn.begT - begT;
	double effT = (timeDiff > 0)? timeDiff : 0.0;
	
	double t;
	if ((currMtn.endT - begT) > lenT) t = lenT;
	else t = (currMtn.endT - begT);
	
	int width = canvas.getW (effT, t);
	
	int x = canvas.getEvtXCord (effT);
	int l = canvas.totalProc;
	
	//	if (!(x == prev_state_beg_pos && (width == 0 || width == 1))) {
	for (int i = 0; i < currMtn.s.length; i++) {
	  RecDef statedef = (RecDef) canvas.parent.stateDefs.elementAt(i);
	  if (currMtn.s [i] > 0 && statedef.checkbox.isSelected ()) {
	    if ((l - currMtn.s [i]) < 0) break;
	      
	    double ycord = canvas.hBWt
                         + ((canvas.eachDispIHt / (double)canvas.totalProc)
                         * (l - currMtn.s [i]));
	    double limit = canvas.hBWt
                         + ((canvas.eachDispIHt / (double)canvas.totalProc) * l)
                         - 1;
	    
	    double height = limit - ycord + 1;
	    if (height == 0) height = 1;
	      
	    double d = Math.rint (ycord);
	    double diff = d - ycord;
	      
	    int yc = (int)d; 
	    int ht = (int)Math.rint (height - diff); 
	      
	    if (width == 0) width = 1;
	      
	    g.setColor (statedef.color);
	    g.fillRect (x, yc, width, ht);
	    
	    l -= currMtn.s [i];
	  }
	}
	//	}
	prev_state_beg_pos = x;
      }
    }
  }

  /**
   * draw arrows
   */
  private void drawArrows( Graphics g1, Graphics g2 )
  {
      RecDef        arrowdef;
      Enumeration   arrows;
      ArrowInfo     arrow;
      Color         arrow_color;

      Enumeration arrowDefs = canvas.parent.arrowDefs.elements();
      while ( arrowDefs.hasMoreElements() ) {
          arrowdef = ( RecDef ) arrowDefs.nextElement();
          if (    arrowdef.stateVector.size() > 0
               && arrowdef.checkbox.isSelected() ) {
              arrows = arrowdef.stateVector.elements();
              arrow_color = arrowdef.color;
              while ( arrows.hasMoreElements() ) {
                  arrow = ( ArrowInfo ) arrows.nextElement();
                  if ( check( arrow, begT, endT ) ) {
                      draw( arrow, g1, arrow_color );
                      if ( arrow.blink )
                          canvas.blink = true;
                      if ( g2 != null && !arrow.blink )
                          draw( arrow, g2, arrow_color );
                  }
              }
          }
      }
  }
  
  /**
   * Draws the given process state in this image.
   */
  private void draw( StateInterval state, double py, Graphics g1, Graphics g2 )
  {
    /*
    double timeDiff = state.info.begT - begT;
    double effT = (timeDiff > 0)? timeDiff : 0.0;
    double lenT = endT - begT;
    int maxW = canvas.getW (effT, lenT);
    int w = canvas.getW (effT, state.info.endT - begT);
    if (w > maxW) w = maxW;
    int x = canvas.getEvtXCord (effT);
    */

    boolean startflag = (state.info.begT >= begT) ? true : false;
    boolean endflag   = (state.info.endT <= endT) ? true : false;
      
    // The origin is set at the X = begT
    int x1 = canvas.getEvtXCord( state.info.begT - begT );
    int x2 = canvas.getEvtXCord( state.info.endT - begT );

    // Define the interval segment to be drawn within the range of image buffer
    int x = startflag ? x1     : 0;
    int w = endflag   ? x2 - x : canvas.getEvtXCord( endT - begT ) - x;
    
    if (!(x == prev_state_beg_pos && (w == 0 || w == 1))) {
      double diff = state.info.level * canvas.yDensity * canvas.nestFactor;
      double height = canvas.yDensity - diff; 
      int y = (int)Math.rint (py - height / 2.0);
      int h = (int)Math.rint (height); 
      
      //Done so that when state dialog has to pop up it uses these values
      state.y = y; state.h = h; if (state.h == 0) state.h = 1;
      
      Color c = state.info.stateDef.color;
      
      displayRect (g1, c, x, y, w, h, startflag, endflag);
      if (g2 != null && !state.info.blink)
	displayRect (g2, c, x, y, w, h, startflag, endflag);
    }
    prev_state_beg_pos = x;
  }
  
  private void displayRect (Graphics g, Color c, int x, int y, int w, int h, 
			    boolean startflag, boolean endflag) {
    g.setColor (c); 
    if (w == 0) w = 1;
    if (h == 0) h = 1;
    g.fillRect (x, y, w, h);
    
    g.setColor (Color.white);
    g.drawLine (x, y, x + w - 1, y);
    g.drawLine (x, y + h - 1, x + w - 1, y + h - 1);
    
    if (startflag) g.drawLine (x, y, x, y + h - 1);
    if (endflag)   g.drawLine (x + w - 1, y, x + w - 1, y + h - 1);
  }
  
  /**
   * Draws Process time Line
   */
  private void draw (StateGroup p, double py, Graphics g1, Graphics g2) {
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
  private boolean check( ArrowInfo arrow, double b, double e )
  {
    int start_idx, end_idx;

    if (    ! ( arrow.begT < begT && arrow.endT < begT )
         && ! ( arrow.begT > endT && arrow.endT > endT ) ) {
        StateGroupList visible_groups = canvas.all_states.visible;
        try {
            start_idx = visible_groups.GetSeqIndexAt( arrow.begGroupID );
            end_idx   = visible_groups.GetSeqIndexAt( arrow.endGroupID );
        } catch ( IndexOutOfBoundsException err ) {
            return false;
        }
        int group_size = visible_groups.size();
        if ( start_idx < group_size && end_idx < group_size ) {
            if ( ( (StateGroup) visible_groups.elementAt(start_idx) ).dispStatus
                 &&
                 ( (StateGroup) visible_groups.elementAt(end_idx) ).dispStatus )
          return true;
        }
    }
    return false;
  }
  
  /**
   * Draw the arrow representing a message in the given image.
   */
  private void draw( ArrowInfo arrow, Graphics g, Color arrow_color )
  {
    // plot coordinates to be passed to displayArrow()
    int x1_p, x2_p, y1_p, y2_p; 

    boolean startflag = (arrow.begT >= begT) ? true : false;
    boolean endflag   = (arrow.endT <= endT) ? true : false;

    // The origin is set at the X = begT
    int x1 = canvas.getEvtXCord( arrow.begT - begT );
    int x2 = canvas.getEvtXCord( arrow.endT - begT );

    // Define the arrow segment to be drawn within the range of image buffer
    x1_p = startflag ? x1 : 0;
    x2_p = endflag   ? x2 : canvas.getEvtXCord( endT - begT );

    if ( ! (    arrow.begGroupID == arrow.endGroupID
             && ( Math.abs( x2_p - x1_p ) <= 1 )
             && x1_p == prev_arrow_beg_pos ) ) {
        double slope = 0.0;

        StateGroupList visible_groups = canvas.all_states.visible;
        int start_idx = visible_groups.GetSeqIndexAt( arrow.begGroupID );
        int end_idx   = visible_groups.GetSeqIndexAt( arrow.endGroupID );

        double y1 = canvas.GetStateGroupYCord( start_idx );
        double y2 = canvas.GetStateGroupYCord( end_idx );

        if ( !startflag || !endflag )
            slope = ( y2 - y1 ) / ( x2 - x1 );

        y1_p = startflag ? (int)Math.rint( y1 ) :
                           (int)Math.rint( y1 - slope * x1 );
        y2_p = endflag   ? (int)Math.rint( y2 ) :
                           (int)Math.rint( y1 + slope * ( x2_p - x1 ) );

        debug.println( "draw_Arrow( [" + begT + "+" + (arrow.begT - begT) + ","
                                       + start_idx
                             + "] = {" +   x1    + "," +   y1    + "}, "
                             +   "+ {" + (x2-x1) + "," + (y2-y1) + "}" );

        displayArrow( g, arrow_color, x1_p, y1_p, x2_p, y2_p,
                      startflag, endflag );
    }
    prev_arrow_beg_pos = x1_p;

  }   

  private void displayArrow( Graphics g, Color arrow_color,
                             int x1, int y1, int x2, int y2,
                             boolean startflag, boolean endflag)
  {
    //Drawing arrow line
    if (g instanceof PrintGraphics) g.setColor (printLineColor);
    // else g.setColor (normLineColor);
    else g.setColor (arrow_color);
    
    g.drawLine (x1, y1, x2, y2);
    
    //Draw circle which gives info. about message
    if ( startflag ) {
        g.setColor (circleColor);
        g.fillOval (x1 - lrad, y1 - lrad, lrad * 2, lrad * 2);

        brad = (int)Math.rint (canvas.yDensity / 2);
        if (brad > 10) brad = 10;
        if (brad < 3) brad = 3;
        g.drawOval (x1 - brad, y1 - brad, brad * 2, brad * 2);
    }

    //Draw arrowhead
    if ( endflag ) {
        double halfangle = (Math.PI / 360.0) * angle;
        double dx = (double) Math.abs(x2 - x1);
        double dy = (double) Math.abs(y2 - y1);
        double a1 = Math.atan (dy / dx);
        double radtodeg = 180.0 / Math.PI;
        int startangle = 0;
    
        if (y1 < y2 && x2 > x1)      // Forward : Downward
            startangle = (int)Math.rint (radtodeg * (Math.PI - a1 - halfangle));
        else if (y1 > y2 && x2 > x1) // Forward : Upward
            startangle = (int)Math.rint (radtodeg * (Math.PI + a1 - halfangle));
        else if (y1 > y2 && x1 > x2) // Backward : Upward
            startangle = (int)Math.rint (radtodeg * ((2.0 * Math.PI) - a1 - halfangle));
        else if (y1 < y2 && x1 > x2) // Backward : Downward
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
        // else g.setColor (normLineColor);
        else g.setColor (arrow_color);
    
        ht = (int)Math.rint (canvas.yDensity);
        if (ht > 20) ht = 20;
        g.fillArc (x2 - ht, y2 - ht, ht * 2, ht * 2, startangle, angle);
    }
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
    g.setColor (canvas.parent.rulerColor);
    
    g.fillRect (0, 0, _xPix, canvas.rulerHt);
    g.setColor (Color.black); g.drawLine (0, 0, _xPix, 0);
    
    double inchT = canvas.getTime (canvas.dpi); if (inchT <= 0) return;
    int i = (int)Math.rint (begT / inchT);
    
    //Start time
    double t = i * inchT;

    //    while (t < endT && t < canvas.maxT) {
    while ( t < endT ) {
      int xcord = i * canvas.dpi - canvas.getEvtXCord (begT);
     
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

      g.drawString ("|", xcord - 2, canvas.lineSize - canvas.fDescent);
      g.drawString (t2,
		    xcord - (int)Math.rint ((double)canvas.fm.stringWidth (t2)
                          / 2.0),
		    2 * canvas.lineSize - canvas.fDescent);
      t = (++i * inchT);
    }
  }  
  
  void kill () {
    for (int i = 0; i < dispA.length; i++) {
      if (dispA [i].img1 != null) {
          dispA [i].img1.flush ();
          dispA [i].img1 = null;
      }
      if (dispA [i].img2 != null) {
          dispA [i].img2.flush ();
          dispA [i].img2 = null;
      }
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
