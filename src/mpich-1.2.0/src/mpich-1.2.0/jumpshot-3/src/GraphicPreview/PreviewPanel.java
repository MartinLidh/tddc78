import javax.swing.*;
import java.awt.*;

public class PreviewPanel extends JPanel
{
    Color displayed_color;
    
    public PreviewPanel(Color color)
    {
	displayed_color = color;
	setPreferredSize(new Dimension(30,30));
	//setBackground(Color.white);
    }
    
    public void paint(Graphics g)
    {
	super.paint(g);
	setBackground(displayed_color);
	/*
	g.setColor(displayed_color);
	g.fillArc(10,10,getWidth()-10,getHeight()-10,0,360);
	*/
    }
    
    public void setDisplaydColor(Color new_color)
    {
	displayed_color = new_color;
	repaint();
    }
}
