import com.sun.java.swing.Icon;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Component;

/**
 * This class is used to make the colored rect representing a state 
 */
public class ColoredRect implements Icon {
  Color color;
  public ColoredRect (Color c) {
    this.color = c;
  }
  
  public void paintIcon(Component c, Graphics g, int x, int y) {
    Color oldColor = g.getColor();
    g.setColor(color);
    g.fill3DRect(x,y,getIconWidth(), getIconHeight(), true);
    g.setColor(oldColor);
  }
  public int getIconWidth() { return 25; }
  public int getIconHeight() { return 18; }
}
