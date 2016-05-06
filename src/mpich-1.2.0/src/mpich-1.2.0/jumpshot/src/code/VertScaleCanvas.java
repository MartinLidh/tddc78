import java.awt.*;
import java.util.Enumeration;
import java.util.Vector;
import java.awt.event.*;
import com.sun.java.swing.*;

/**
 * This class is the canvas on which the process ids are displayed. It
 * is located left and right side of ProgramCanvas in mainPanel of
 * ClogDisplay object.
 */
public class VertScaleCanvas extends JComponent {
  protected ClogDisplay parent;
  protected ProgramCanvas canvas;

  protected int vgap, lineSize, _xPix;
  private Font vCanFont;
  
  private JProcess currProc;
  protected boolean setupComplete;
  
  public VertScaleCanvas (ClogDisplay p) {
    super ();
    parent = p;
    canvas = p.canvas;
    setup ();
  }
  
  public VertScaleCanvas () {super ();}
  
  private void setup () {
    setupComplete = false;
    adjustCanvasStuff ();
    this.enableEvents (AWTEvent.MOUSE_EVENT_MASK);
  }
  
  private void adjustCanvasStuff () {
    setCursor (new Cursor (Cursor.HAND_CURSOR));
    setToolTipText ("Double click on process # to get process controls");
    setFont (vCanFont = new Font ("Serif", Font.PLAIN, 12));
    FontMetrics fm = getToolkit ().getFontMetrics (vCanFont);
    lineSize = fm.getHeight ();
    _xPix = fm.stringWidth (new String ("0000"));
  }
  
  public void paintComponent (Graphics g) {
    if (setupComplete)
      drawStuff (g, 0, vgap, canvas._yPix);
  }
  
  public Dimension getPreferredSize () {return new Dimension (_xPix, canvas._yPix);}
  
  /**
   * This method is draws the vertical scale containing process Ids. This method
   * is used by paint () and also PrintDlg class to print.
   */
  public void drawStuff (Graphics g, int x, int y, int height) {
    int numDisp = canvas.dtypeV.size ();
    g.setColor (parent.rulerColor);
    g.fillRect (x, y, _xPix, height);
    
    for (int i = 0; i < numDisp; i++) {
      int dtype = ((Integer)canvas.dtypeV.elementAt (i)).intValue ();
      
      g.setColor (parent.rulerColor);
      g.fill3DRect (x, y + (i * canvas.eachDispHt), _xPix, canvas.hBWt, true);
      g.fill3DRect (x, y + ((i + 1) * canvas.eachDispHt) - canvas.hBWt, _xPix, canvas.hBWt, 
		    true);
      
      if (dtype == CONST.MTN_RANGES) continue;
      
      g.setColor (Color.black);
      
      for (int j = 0; j < canvas.procVector.size (); j++) {
	int xcord = x;
	int ycord = 
	  (int)Math.rint ((i * canvas.eachDispHt) + canvas.getProcYCord (j) + lineSize / 4.0 + y);
	int procId = ((JProcess)canvas.procVector.elementAt (j)).procId;
	g.drawString (getNumString (procId, 4), xcord, ycord);
      }
    }
  }
  
  /**
   * method used to format process Id
   */
  public String getNumString (int num, int ct) {
    String numStr = Integer.toString (num);
    int fill = ct - numStr.length ();
    
    for (int i = 0; i < fill; i++) numStr = " " + numStr;
    return numStr;
  }
  
  /**
   * Event handler method for MouseEvents
   */
  public void processMouseEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_CLICKED) {
      if (e.getClickCount () == 2) handleProcDlg (e);
    }
    else super.processMouseEvent (e);
  }
  
  /**
   * if the mouse is clicked upon some process Id then display the procDlg for
   * that process.
   */
  private void handleProcDlg (MouseEvent e) {
    int y = e.getY ();
    if (y < vgap || y >= (vgap + canvas.allDispHt)) return;
    int dIndex = (y - vgap) / canvas.eachDispHt;
    if (((Integer)canvas.dtypeV.elementAt (dIndex)).intValue () != CONST.TIMELINES) return;
    
    int index = canvas.findIndex ((y - vgap) % canvas.eachDispHt);
    if (index > -1) {
      currProc = (JProcess)(canvas.procVector.elementAt (index));
      parent.procDlg.reset (currProc);
      parent.procDlg.show ();
      parent.procDlg.toFront ();
    }
  }
  
  void setupComplete () {setupComplete = true; repaint ();}
  
  void setVerticalGap (int g) {vgap = g;}
}
