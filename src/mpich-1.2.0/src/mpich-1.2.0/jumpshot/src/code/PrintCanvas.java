import com.sun.java.swing.JOptionPane;
import java.awt.*;

public class PrintCanvas extends ProgramCanvas {
  private Graphics pg;
  private ProgramCanvas canvas;
  private int xcord, ycord, width, height, vcanWt;
  
  public PrintCanvas (ProgramCanvas c, Graphics g, int x, int y, int w, int h, int d) {
    super ();
    canvas = c;
    pg = g;
    xcord = x; ycord = y;
    width = w; height = h;
    dpi = d;
    setupP ();
  }
    
  private void setupP () {
    dtypeV = canvas.dtypeV;
    procVector = canvas.procVector;
    maxT = canvas.maxT;
    begTime = canvas.begTime;
    endTime = canvas.endTime;
    hBWt = canvas.hBWt;
    parent = canvas.parent;
    proct = canvas.proct;
    nestFactor = canvas.nestFactor;
    arrowDispStatus = canvas.arrowDispStatus;
    totalProc = canvas.totalProc;
    
    fm = pg.getFontMetrics ();
    lineSize = fm.getHeight (); rulerHt = 3 * lineSize; fDescent = fm.getDescent ();
    vcanWt = fm.stringWidth (new String ("0000"));
    pg.translate (xcord + vcanWt, ycord);
    
    int n = JOptionPane.showConfirmDialog(null, 
					  "Should printed region have same proportions\n" + 
					  "as image?", "Print Option", JOptionPane.YES_NO_OPTION);
    if (n == JOptionPane.NO_OPTION) {_xPix = width - vcanWt; _yPix = height;}
    else if (n == JOptionPane.YES_OPTION) {
      _xPix = width - vcanWt;
      _yPix = (int)Math.floor ((canvas.heightCan / (double)canvas.widthCan) * _xPix);
      
      if (_yPix > height) {
	_yPix = height;
	_xPix = (int)Math.floor ((canvas.widthCan / (double)canvas.heightCan) * _yPix);
      }
    } 
    else return;
    
    pg.setClip (0, 0, _xPix, _yPix);
    
    setValues ();
    xDensity = canvas.xDensity * (_xPix / (double)canvas.widthCan);
    
    currImg  = new MyImage (_xPix, _yPix, dtypeV, this);
    currImg.setTimes (begTime, endTime);
    currImg.printStuff (pg);
    pg.translate (-vcanWt, -allDispHt);
    pg.setClip (0, 0, vcanWt, _yPix);
    new PVertScaleCanvas (this, pg);
    pg.translate (vcanWt, 0);
    pg.setClip (0, 0, _xPix, allDispHt);
    pg.setColor (Color.black);
    pg.drawRect (0, 0, _xPix - 1, allDispHt - 1);
    canvas.Refresh ();
  }
}


