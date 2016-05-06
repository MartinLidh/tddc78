import javax.swing.*;
import java.awt.*;

public class XAxisEndPanel extends JPanel
{
    Graph graph;
    Point origin;

    XAxisEndPanel(Graph graph,Point origin_val)
    {
	this.graph = graph;
	origin = origin_val;
	setBackground(Color.white);
    }

    public void paint(Graphics g)
    {
	super.paint(g);
	Image virtual_canvas = 
	    createImage(getWidth(),getHeight());
	Graphics brush = virtual_canvas.getGraphics();
	drawXAxisEnd(brush,virtual_canvas);
	g.drawImage(virtual_canvas,0,0,Color.white,this);
	brush.dispose();
    }
    

    private void drawXAxisEnd(Graphics brush, 
			   Image virtual_canvas)
    {
	brush.setColor(Color.white);
	brush.fillRect
	    (0,0,virtual_canvas.getWidth(this),virtual_canvas.getHeight(this));


	brush.setColor(Color.black);
	brush.drawLine(origin.x, origin.y, getWidth()-20,
		       origin.y);
	
	//** drawing the arrows
	
	brush.drawLine(origin.x + getWidth() - 20, origin.y,
		       origin.x + getWidth() - 25, origin.y - 5);
	brush.drawLine(origin.x + getWidth() - 20, origin.y,
		       origin.x + getWidth() - 25, origin.y + 5);
    }
    public void shiftOrigin(int x, int y)
    {
	origin.translate(x,y);
    }
}


