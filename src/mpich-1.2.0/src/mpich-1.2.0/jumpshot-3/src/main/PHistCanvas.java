import java.awt.*;
import javax.swing.JOptionPane;

public class PHistCanvas extends HistCanvas {
  private Graphics pg;
  private HistCanvas canvas;
  private int xcord, ycord, width, height;
  
  public PHistCanvas (HistCanvas c, Graphics g, int x, int y, int w, int h, int d) {
    super (c.parent);
    canvas = c;
    pg = g;
    xcord = x; ycord = y;
    width = w; height = h;
    dpi = d;
    setupP ();
  }
    
  private void setupP () {
    fm = pg.getFontMetrics ();
    lineSize = fm.getHeight ();
    fDescent = fm.getDescent ();
    numBins = canvas.numBins;
    stateDef = canvas.stateDef;
    endT = canvas.endT;
    begT = canvas.begT;
    
    pg.translate (xcord, ycord);
    
    int n = JOptionPane.showConfirmDialog(null, 
					  "Should printed region have same proportions\n" + 
					  "as image?", "Print Option", JOptionPane.YES_NO_OPTION);
    if (n == JOptionPane.NO_OPTION) {_xPix = width; _yPix = height;}
    else if (n == JOptionPane.YES_OPTION) {
      _xPix = width;
      _yPix = (int)Math.floor ((canvas.heightCan / (double)canvas.widthCan) * _xPix);
      
      if (_yPix > height) {
	_yPix = height;
	_xPix = (int)Math.floor ((canvas.widthCan / (double)canvas.heightCan) * _yPix);
      }
    } 
    else return;

    pg.setClip (0, 0, _xPix, _yPix);
    
    xDensity = _xPix / (double)(endT - begT);
    yDensity = (_yPix / (double)canvas.heightCan) * canvas.yDensity;
    
    view  = new HistImage (_xPix, _yPix, this);
    view.drawRegionOnGraphics (begT, endT, pg);
    pg.setColor (Color.black);
    pg.drawRect (0, 0, _xPix - 1, _yPix - 1);
  }
}




