import java.awt.*;
import javax.swing.*;

public class Graph_Title extends JPanel
{
    String title;

    public Graph_Title(String title)
    {
	super(new FlowLayout(FlowLayout.RIGHT));
	this.title = title;
    }
    
    public void paint(Graphics g)
    {
	super.paint(g);
	g.setColor(Color.black);
	g.drawString(title,
		     getSize().width/2 -
		     getFontMetrics(g.getFont()).stringWidth(title)/2,
		     getSize().height - 
		     (getFontMetrics(g.getFont()).getMaxDescent() +
		      getFontMetrics(g.getFont()).getMaxAscent()));
    }
}
