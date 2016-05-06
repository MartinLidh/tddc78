import java.awt.*;
import javax.swing.*;

public class Stat_Graph_Title extends JPanel
{
    String title;

    public Stat_Graph_Title()
    {
	this("");
    }

    public Stat_Graph_Title(String title)
    {
	super(new FlowLayout(FlowLayout.RIGHT));
	this.title = title;
	setPreferredSize
	    (new Dimension(getSize().width, 
			   getFontMetrics(getFont()).getHeight()+10));
    }
    
    public void setTitle(String title)
    {
	this.title = title;
    }

    public void paint(Graphics g)
    {
	super.paint(g);
	setBackground(Color.white);
	g.setColor(Color.black);
	g.drawString(title,
		     getSize().width/2 -
		     getFontMetrics(g.getFont()).stringWidth(title)/2,
		     getSize().height - 
		     getFontMetrics(g.getFont()).getMaxDescent());
    }
}
