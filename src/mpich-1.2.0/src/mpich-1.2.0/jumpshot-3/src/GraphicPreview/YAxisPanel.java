import javax.swing.*;
import java.awt.*;

public class YAxisPanel extends JPanel
{
    Graph graph;
    double y_range_max,
	   y_range_min = 0;
    Point originY;
    Dimension yaxis_dim;
    YAxisPanel(Graph graph,Point origin_val,Dimension size)
    {
	this.graph = graph;
	originY = origin_val;
	yaxis_dim = size;
	setBackground(Color.white);
    }

    

    public void paint(Graphics g)
    {
	super.paint(g);
	Image virtual_canvas = 
	    createImage(getWidth(),getHeight());
	Graphics brush = virtual_canvas.getGraphics();
	drawYAxis(brush,virtual_canvas);
	g.drawImage(virtual_canvas,0,0,Color.white,this);
	brush.dispose();
    }
    
    public Dimension getDimension()
    {
	return (new Dimension(yaxis_dim));
    }

    private void drawYAxis(Graphics brush, 
			   Image virtual_canvas)
    {
	int y_bin_gap       = graph.getYBinGap();
	Point origin = new Point(getWidth() - (yaxis_dim.width - originY.x),
				 originY.y);
	brush.setColor(Color.white);
	brush.fillRect
	    (0,0,virtual_canvas.getWidth(this),virtual_canvas.getHeight(this));
	brush.setColor(Color.black);
	
	//** drawing the axes

	brush.drawLine(origin.x, origin.y, origin.x, graph.GRAPH_MARGIN-5);
	brush.drawLine(origin.x, origin.y, virtual_canvas.getWidth(this) ,
		       origin.y);
	
	//** drawing the arrows

	brush.drawLine(origin.x, graph.GRAPH_MARGIN - 5, origin.x - 5, 
		       graph.GRAPH_MARGIN);
	brush.drawLine(origin.x, graph.GRAPH_MARGIN - 5, origin.x + 5, 
		       graph.GRAPH_MARGIN);
	
	int i;

	//** draw labels on y-axis

	for(i=origin.y; i>(origin.y-(graph.graph_size.height-4));
	    i-=y_bin_gap)
	    {
		brush.drawLine(origin.x,i,origin.x-4,i);
		centerLabelOnY(i,brush,origin);
	    }
	brush.setColor(Color.black);
    }
    /*
      centers the label at the given position by the y-axis
    */
    private void centerLabelOnY(int y_position,Graphics brush,Point origin)
    {
	int    plotting_height = graph.getPlotting_height();
	double y_range_max = graph.getYRangeMax(),
	       value;
	Font font = brush.getFont();
	value = ((((double)(origin.y-y_position)/plotting_height)*
		  y_range_max)+(double)y_range_min);
	String value_s = Double.toString(value);
	value_s = value_s.substring(0, value_s.indexOf((int)'.'));
	brush.drawString(value_s,
		     (origin.x-6)-(getFontMetrics(font).stringWidth(value_s)),
			 y_position);
    }
    public void shiftOrigin(int x, int y)
    {
	originY.translate(x,y);
    }

}


