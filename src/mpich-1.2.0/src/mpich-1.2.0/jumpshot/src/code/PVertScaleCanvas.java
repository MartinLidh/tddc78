import java.awt.*;

public class PVertScaleCanvas extends VertScaleCanvas {
  private Graphics pg;
  
  public PVertScaleCanvas (PrintCanvas p, Graphics g) {
    super ();
    pg = g;
    parent = p.parent;
    canvas = p;
    setupComplete = true;
    lineSize = canvas.lineSize;
    _xPix = canvas.fm.stringWidth (new String ("0000"));
    vgap = 0;
    drawStuff (pg, 0, 0, canvas._yPix);
  }
}
