import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;

public class ScrollBarBorder extends AbstractBorder
{
    Color color = Color.darkGray;
    public ScrollBarBorder(Color colr)
    {
	color = colr;
    }

    public ScrollBarBorder(){}
    
    public void paintBorder(Component c, Graphics g, int x, int y,
			    int width, int height)
    {
	Insets insets = getBorderInsets(c);
	g.setColor(Color.darkGray);
	g.fillRect(x, y, insets.left, height);
	g.fillRect(x, y + height - insets.bottom, width, insets.bottom);
	g.fillRect(x + width - insets.right, y, insets.right, height);
    }
    public boolean isBorderOpaque(){ return true;}
    public Insets getBorderInsets(Component c)
    {
	return new Insets(0,1,1,1);
    }
}

