import java.util.Vector;
import java.util.Enumeration;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Color;
import java.awt.PrintGraphics;

//This class draws the bins and the time scale onto a offscreen image
//associated with it.
public class HistImage {
  HistCanvas parent;//The canvas from which this offscreen image is got
 
  double begT;      //starting time in seconds of bins in this image
  double endT;      //ending time in seconds of bins in this image   
  int _xPix;        //width of the image 
  int _yPix;        //height of the image
  
  Image img;        //offscreen image object
  
  int statesDrawn;  //current number of states drawn
  int highestCt;    //highest number of states in any bin
  int max = 6;
  
  public HistImage (Image i, int x, int y, HistCanvas p) {
    img = i; _xPix = x; _yPix = y; parent = p;
  }
   
  public HistImage (int x, int y, HistCanvas p) {_xPix = x; _yPix = y; parent = p;}
  
  //change the starting and ending times and 
  //re draw all the bins
  void drawRegion (double b, double e) {
    begT = b; endT = e;
    drawStuff (img.getGraphics ());
  }
  
  void drawRegionOnGraphics (double b, double e, Graphics g) {
    begT = b; endT = e;
    drawStuff (g);
  }
  
  //methods to draw bins, time scale-------------------------------------------
  /**
   * This method draws the bins and the time scale to the specified graphics
   * context
   */
  void drawStuff (Graphics g) {
    if (g instanceof PrintGraphics) 
      g.setColor (parent.parent.parent.printImgBColor);
    else 
      g.setColor (parent.parent.parent.normImgBColor);
    
    g.fillRect (0, 0, _xPix, _yPix - 3 * parent.lineSize);
    
    //draw the stuff
    drawBins (g);
    
    //Draw the Time Line
    drawTimeLine (g);
  }
  
  /**
   * this method draws the bins and the time scale onto the offscreen image
   */
  void drawStuff () {drawStuff (img.getGraphics ());}

  /**
   * This function creates bins and causes them to be drawn
   */
  private void drawBins (Graphics g) {
    double dTime = (endT - begT) / parent.numBins;
    int j = 0; statesDrawn = 0; highestCt = 0;
    
    Vector v = parent.stateDef.stateVector;
    
    for (int i = 0; i < parent.numBins; i++) {
      //Limits of a bin
      double startTime = begT + (i * dTime);        
      double finishTime = begT + ((i + 1) * dTime);
      int ct = 0;
      
      Info currState;
      while (j < v.size () &&
             ((currState = (Info)v.elementAt (j)).lenT < finishTime ||
              (currState.lenT == finishTime && currState.lenT == endT))) {
        if (startTime <= currState.lenT) {
          ct++;
          statesDrawn ++;
        }
        j++;
      }
      if (highestCt < ct) highestCt = ct;
      
      //draw the bin
      if (ct > 0) drawBin (g, i, ct);
    }
  }
  
  /**
   * This function draws a histogram bar for a bin.
   */
  protected void drawBin (Graphics g, int binNo, int ct) {
    int height = parent.getHistHeight (ct);
    if (height == 0) height = 1;
    if (height > (_yPix - 3 * parent.lineSize)) height = _yPix - 3 * parent.lineSize;
    
    int d = (int)Math.rint ((binNo * _xPix) / ((double)parent.numBins));
    
    if (d >= _xPix) d = _xPix - 1;
    int x1 = d;
    
    int e = (int)Math.rint (((binNo + 1) * _xPix) / ((double)parent.numBins));

    int x2 = e;
    
    int width = x2 - x1;
    if (width == 0) width = 1;
    
    int y = _yPix - 3 * parent.lineSize - height;
    
    g.setColor (parent.stateDef.color);
    g.fillRect (x1, y, width, height);
  }
  
  /**
   * This function draws the timeLine for the image
   */
  private void drawTimeLine (Graphics g) {
    g.setColor (parent.parent.parent.rulerColor);
    g.fillRect (0, _yPix - 3 * parent.lineSize, _xPix, 3 * parent.lineSize);
    
    g.setColor (Color.black);
    g.drawLine (0, _yPix - 3 * parent.lineSize, _xPix, _yPix - 3 * parent.lineSize);
    double inchT = parent.getTime (parent.dpi); if (inchT <= 0) return;
    int i = (int)Math.rint (begT / inchT);
    double t = i * inchT;
    while (t < endT) {
      int xcord = i * parent.dpi - parent.getEvtXCord (begT) - 
        (int)Math.rint (parent.fm.charWidth ('|') / 2.0);
      
      g.drawString ("|", xcord, _yPix - 2 * parent.lineSize - parent.fDescent);
      
      String t1 = (new Float (t)).toString (), t2;
      
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
      
      g.drawString (t2,
                    xcord - (int)Math.rint (parent.fm.stringWidth (t2) / 2.0),
                    _yPix - (parent.lineSize + parent.fDescent));
      t = (++i * inchT);
    }
  }  
  //end of methods to draw bins, time scales----------------------------------------------
  
  void kill () {if (img != null) {img.flush (); img = null;}}
  
  protected void finalize () throws Throwable {super.finalize ();}
}
