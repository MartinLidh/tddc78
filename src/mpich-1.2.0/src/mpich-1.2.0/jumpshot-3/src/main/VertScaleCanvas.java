import java.awt.*;
import java.util.Enumeration;
import java.util.Vector;
import java.awt.event.*;
import javax.swing.*;

/**
 * This class is the canvas on which the process ids are displayed. It
 * is located left and right side of ProgramCanvas in mainPanel of
 * FrameDisplay object.
 */
public class VertScaleCanvas extends JComponent {
  //  NdigitsDisplayed specifies the No. of digits of integer allowed
  private   int           NdigitsDisplayed = 8; 
  protected FrameDisplay  parent;
  protected ProgramCanvas canvas;
  private String view_indexes_label;

  protected int vgap, lineSize, _xPix;
  private Font vCanFont;
  
  private StateGroup currProc;
  protected boolean setupComplete;
  
  public VertScaleCanvas( FrameDisplay p, String in_view_label )
  {
    super ();
    parent = p;
    canvas = p.canvas;
    view_indexes_label = in_view_label;
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
    setToolTipText( "Double click on State Group Label, "
                  + view_indexes_label
                  + ", to get State Group manipulation panel" );
    setFont (vCanFont = new Font ("Serif", Font.PLAIN, 12));
    FontMetrics fm = getToolkit ().getFontMetrics (vCanFont);
    lineSize = fm.getHeight ();
    StringBuffer str_buf = new StringBuffer();
    for ( int ii = 0; ii < NdigitsDisplayed; ii ++ )
        str_buf.append( "0" ); 
    _xPix = fm.stringWidth ( str_buf.toString() );
  }
  
  public void paintComponent (Graphics g) {
    if (setupComplete)
      drawStuff (g, 0, vgap, canvas._yPix);
  }
  
  public Dimension getPreferredSize () {
      return new Dimension (_xPix, canvas._yPix);
  }
  
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
      g.fill3DRect (x, y + ((i + 1) * canvas.eachDispHt) - canvas.hBWt,
                    _xPix, canvas.hBWt, true);
      
      if (dtype == CONST.MTN_RANGES) continue;
      
      g.setColor (Color.black);
      
      StateGroupLabel groupID;
      StateGroupList visible_groups = canvas.all_states.visible;
      int groups_size = canvas.all_states.visible.size();
      for ( int j = 0; j < groups_size; j++ ) {
          int xcord = x;
          int ycord = (int)Math.rint( (i * canvas.eachDispHt)
                                    + canvas.GetStateGroupYCord( j )
                                    + lineSize / 4.0 + y );
          groupID = ( (StateGroup) visible_groups.elementAt( j ) ).groupID;
          g.drawString( getNumString( groupID, NdigitsDisplayed ),
                        xcord, ycord );
      }
    }
  }
  
  /**
   * method used to format process Id
   */
  public String getNumString ( StateGroupLabel label, int ct) {
    String numStr = label.toString();
    int fill = ct - numStr.length ();
    
    for (int i = 0; i < fill; i++) numStr = " " + numStr;
    return numStr;
  }
  
  /**
   * Event handler method for MouseEvents
   */
  public void processMouseEvent (MouseEvent e) {
    if (e.getID () == MouseEvent.MOUSE_CLICKED) {
      if (e.getClickCount () == 2) handleStateGroupDialog (e);
    }
    else super.processMouseEvent (e);
  }
  
  /**
   * if the mouse is clicked upon some process Id then display the procDlg for
   * that process.
   */
  private void handleStateGroupDialog (MouseEvent e) {
    int y = e.getY ();
    if (y < vgap || y >= (vgap + canvas.allDispHt)) return;
    int dIndex = (y - vgap) / canvas.eachDispHt;
    if ( ( (Integer) canvas.dtypeV.elementAt(dIndex) ).intValue()
         != CONST.TIMELINES )
        return;
    
    int index = canvas.findIndex ((y - vgap) % canvas.eachDispHt);
    if (index > -1) {
      currProc = (StateGroup) canvas.all_states.visible.elementAt(index);
      parent.procDlg.reset (currProc);
      parent.procDlg.show ();
      parent.procDlg.toFront ();
    }
  }
  
  void setupComplete () {setupComplete = true; repaint ();}
  
  void setVerticalGap (int g) {vgap = g;}
}
