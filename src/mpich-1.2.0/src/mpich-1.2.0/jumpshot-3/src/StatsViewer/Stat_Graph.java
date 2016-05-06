import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class Stat_Graph extends JPanel
{
    private Statistics_Data graph_data;
    private double x_range_max; 
    private int time_flag = 0,
	        x_scale_index,
                num_in_group = 1;

    public Stat_Graph()
    {
	super();
	graph_data = null;
    }

    public Stat_Graph(Statistics_Data data)
    {
	super();
	graph_data = data;
    }

    public void setGraphData(Statistics_Data data)
    {
	graph_data = data;
    }

    public void paint(Graphics g)
    {
	setBackground(Color.white);
	super.paint(g);
	int y_bin_gap;
	Dimension graph_size = new Dimension(0,0);
	Image virtual_canvas = createImage(getWidth(),getHeight());
	Graphics brush = virtual_canvas.getGraphics();
	Point origin = new Point(0,0);
	    
	super.paint(g);

	if(graph_data != null) {
	    y_bin_gap = drawYAxis(graph_size, brush, origin);
	    brush.setFont(g.getFont());
	    initNumInGroup(y_bin_gap, graph_size);
	    drawXAxis(graph_size,brush, origin);
	    drawGraph(graph_size,brush, origin, y_bin_gap);
	    g.drawImage(virtual_canvas,0,0,Color.white,this);
	}
    }
    
    private int drawYAxis(Dimension graph_size, Graphics brush, Point origin)
    {
	int max_y_label_length = 0,
	    margin             = 20,
	    bottom_margin      = 35,
	    y_bin_gap          = 0,
	    y_labl_position;
	boolean drawLabelsFlag = true;
	
	String string_buffer = "";
	Font y_label_font;
	FontMetrics font_metric;

	for(int ii = 0; ii < graph_data.getNumYValues(); ii++) 	    
	    if(graph_data.getYLabel(ii).length() > max_y_label_length)
		max_y_label_length = graph_data.getYLabel(ii).length();
	for(int ii = 0; ii <= max_y_label_length; ii++)
	    string_buffer += "0";
	
	if(graph_data.getNumYValues() != 0)
	    y_bin_gap = (getHeight() - (bottom_margin + margin)) /
		graph_data.getNumYValues();

	y_label_font = brush.getFont();
	
	if(y_bin_gap  < 4) {
	    drawLabelsFlag = false;
	    if(y_bin_gap < 1)
		y_bin_gap = 1;
	    y_label_font = new Font("TimesRoman", 
				    y_label_font.getStyle(), 
				    1);
	}
	
	font_metric = getFontMetrics(y_label_font);
	
	
	while((font_metric.getHeight() != 0) && 
	      (font_metric.getHeight() > y_bin_gap) &&
	      drawLabelsFlag) {
	    y_label_font = new Font("TimesRoman",
				    y_label_font.getStyle(), 
				    y_label_font.getSize()-1);
	    font_metric = getFontMetrics(y_label_font);
	}
	
	origin.setLocation(margin + font_metric.stringWidth(string_buffer),
			   getHeight() - bottom_margin);
	graph_size.setSize(getWidth() - (origin.x + margin),
			   getHeight() - (margin + bottom_margin));
	brush.setFont(y_label_font);
	brush.setColor(Color.white);
	brush.fillRect
	    (0,0,getWidth(),getHeight());
	brush.setColor(Color.black);
	
	if(origin.y > margin) {
	    //** drawing the y-axis
	    brush.drawLine(origin.x, origin.y, origin.x, margin - 5);
	    
	    //** drawing the arrows
	
	    brush.drawLine(origin.x, margin - 10, origin.x - 5, 
			   margin - 5);
	    brush.drawLine(origin.x, margin - 10, origin.x + 5, 
			   margin - 5);
	}
	
	//** draw labels on y-axis
	
	for(int ii = origin.y - y_bin_gap, jj = 0; 
		(jj < graph_data.getNumYValues()) &&
		(graph_size.height > 0);
	    ii -= y_bin_gap, jj++) {
	    
	    brush.drawLine(origin.x,ii,origin.x-4,ii);
	    y_labl_position = ii + y_bin_gap / 2;
	    if((y_labl_position - ii) < font_metric.getMaxAscent())
		y_labl_position = ii + font_metric.getMaxAscent();
	    if(drawLabelsFlag)
		brush.drawString(graph_data.getYLabel(jj),
				 origin.x - (5 +
					     font_metric.stringWidth
					     (graph_data.getYLabel(jj))),
				 y_labl_position);
	}
	return y_bin_gap;
    }

    private void drawXAxis(Dimension graph_size, Graphics brush, Point origin)
    {
	double cal_assist1,
	       increments,
	       small_increments;
	int plotting_height,
	    cal_assist2,
	    cal_assist3,
	    x_min_labl_size = 50,
	    x_scale_string_width = 6,
	    x_labl_position,
	    max_index,
	    margin = 20;

	x_range_max = 0;
	
	for(int ii = 0; ii < graph_data.getNumYValues(); ii += num_in_group) {
	    cal_assist1 = 0;
	    for(int jj = 0; jj < graph_data.getNumXValues(); jj++) 
		if(graph_data.isEnabled(jj))
		    for(int ll = 0; (ll < num_in_group) &&
			    ((ll + ii) < graph_data.getNumYValues()); ll++)
			cal_assist1 += graph_data.getElementAt(jj,ll + ii);
	    if(cal_assist1 > x_range_max)
		x_range_max = cal_assist1;
	}

	//** whenever all data sets are disabled y_range_max gets
	//**  calculated as zero and hence the following if statement.
	if(x_range_max == 0)
	    x_range_max = 1;
	
	if(x_range_max > 3600) {
	    time_flag = 1;
	    x_range_max=x_range_max/3600;
	}
	
	//** the following paragraph of code generates the scale of the graph
	//** in terms of an integer index of 10 
	//** for the case where max is either too large or too small.
	
	max_index = (int)(Math.log(x_range_max)/Math.log(10.0));
	if(x_range_max < Math.pow(10.0,max_index))
	    max_index--;
	    
	if(max_index > (x_scale_string_width-1))
	    x_scale_index = max_index - x_scale_string_width;
	else if( max_index < ((-1)*(x_scale_string_width-2)))
	    x_scale_index = (x_scale_string_width-2) + max_index;
	
	increments = Math.pow(10.0,max_index);
	small_increments = increments / 2;
	if((int)((increments * graph_size.width) / x_range_max) < 
	   x_min_labl_size) {
	    increments *= 5;
	    small_increments = increments/5;
	}
	else if ((int)((increments * graph_size.width) / (2 * x_range_max)) >=
		 x_min_labl_size) {
	    increments /= 2;
	    small_increments = increments / 5;
	}
		
	
	int tenRaisedTo = (int)Math.rint(Math.log(small_increments) / 
					 Math.log(10.0));

	brush.setColor(Color.black);

	if(origin.y > margin) {
	    brush.drawLine(origin.x, origin.y, getWidth()-10, origin.y);
	    brush.drawLine(getWidth()-5, origin.y, getWidth()-10, 
			   origin.y-5);
	    brush.drawLine(getWidth()-5, origin.y, getWidth()-10, 
			   origin.y+5);
	}
	
	//** draw labels on x-axis
	
	String temp;
	for(double ii = small_increments,jj = increments;
	    (ii <= x_range_max) && (graph_size.width > 0) &&
		(origin.y > margin);
	    ii += small_increments)
	    {
		x_labl_position =  (int)
		    Math.rint(ii * graph_size.width / x_range_max) + origin.x;
		if( Math.abs(jj - ii) < Math.pow(10,-6)) {
		    brush.fillRect(x_labl_position,origin.y,2,3);
		    ii = Math.rint(ii * Math.pow(10, (-1) * tenRaisedTo)) * 
			Math.pow(10, tenRaisedTo);
		    centerLabelOnX(ii, x_labl_position, origin, brush);
		    jj+=increments;
		}
		else 
		    brush.drawLine(x_labl_position,origin.y,x_labl_position,
				   origin.y + 2);
	    }
	
	temp = "time ( ";
	if(x_scale_index != 0)
	    temp += "x10    ";
	
	if(time_flag == 0)
	    temp += "seconds )";
	else
	    temp += "hours )";
	
	
	Font font = brush.getFont();

	if((x_scale_index !=  0)&&(x_scale_index != 1))
	    {
		brush.drawString(Integer.toString(x_scale_index),
				 origin.x + (graph_size.width/2) - 
				 (getFontMetrics(font).stringWidth(temp)/2)+
				 (getFontMetrics(font).
				  stringWidth("time ( x10")),
				 origin.y +
				 2*getFontMetrics(font).getHeight()-4);
	    }
	
	brush.drawString(temp,
			 origin.x + (graph_size.width/2) - 
			 (getFontMetrics(font).stringWidth(temp)/2),
			 origin.y + 2*getFontMetrics(font).getHeight());
	
	/* draw the marker to show the max x-value
	brush.setColor(Color.red);
	brush.drawLine(origin.x + (x_bin_gap * num_data_pts),origin.y,
		       origin.x + (x_bin_gap * num_data_pts),origin.y+8);
	
	temp = calculateXValueString((x_bin_gap*num_data_pts)+origin.x,6);
	
	brush.drawString(temp,
			 origin.x + (x_bin_gap * num_data_pts)- (int)
			 (0.75*getFontMetrics(font).stringWidth(temp)),
			 origin.y + 2*getFontMetrics(font).getMaxAscent()+8);
	*/
	    brush.setColor(Color.black);
    }

  
    private void centerLabelOnX(double value, int x_position, Point origin,
				Graphics brush)
    {
	Font font = brush.getFont();
	String value_s =  calculateString(value, 6);
	brush.drawString(value_s,
		      x_position-(getFontMetrics(font).stringWidth(value_s)/2),
		      (origin.y+3)+(getFontMetrics(font).getMaxAscent()));
    }

    private String calculateString(double value,int precision)
    {
	String value_s,temp_string;
	int indexValue,
	    min_displyd_char = 2;//-- example: "0.0.......1"
	                         //-- the "0." is 2 char long.

	if(time_flag == 1)
	    value /= 3600;

	//**scaling the value obtained from the above operation
	value /= Math.pow(10,x_scale_index);
	
	temp_string = Double.toString(value);

	//** if the value is very small or very large the string representation
	//** is in scientific notation which poses a tricky problem when
	//** the string needs to be displayed on the graph. the solution is 
	//** taken care of in the following if statement.

	if(temp_string.indexOf((int)'E') != -1)
	    {
		indexValue = Integer.parseInt(temp_string.substring
				    (temp_string.indexOf((int)'E')+1));

		//** since the max value has already been scaled down,
		//** only negative indices are possible in the string 
		//** representation of the time value. The following if 
		//** statement checks to make sure if the exponent is within
		//** the prescribed length of the final string (precision).
		if(indexValue >= ((-1)*(precision-min_displyd_char)))
		    //** 
		    {
			value_s ="0.";
			int zero_counter = (-1)*indexValue;
			while(zero_counter > 1)
			    {
				value_s += "0";
				zero_counter--;
			    }
			int i=0;
			value_s += temp_string.charAt(i);
			i=2;
			while((value_s.length() < precision)&&
			      (temp_string.charAt(i) != 'E'))
			    {
				value_s += temp_string.charAt(i);
				i++;
			    }
		    }
		else value_s = "0.0";
	    }
	else
	    {
		value_s = temp_string;
		if(value_s.length()>precision)
		    value_s = value_s.substring(0,precision);
		while(value_s.charAt(value_s.length() - 1) == '0')
		    value_s = value_s.substring(0,value_s.length() - 1);
		if(value_s.indexOf((int)'.') == (value_s.length()-1))
		    value_s = value_s.substring(0,value_s.length()-1);

	    }
	return new String(value_s);
    }	

    private void drawGraph(Dimension graph_size, Graphics brush, Point origin,
			   int y_bin_gap)
    {
	int x_coord1=0,
	    x_coord2=0,
	    bar_width=1,
	    margin = 20;
	double cal_assist1;
	brush.setColor(Color.blue);
	

	
	//** calculating bar width so as not to intersect any two bars 
	if(y_bin_gap > 3)
	    if(y_bin_gap % 2 == 0)
		bar_width = (y_bin_gap-1);
	    else bar_width = (y_bin_gap-2);
	

	for(int ii = 0, jj = origin.y - y_bin_gap;
	    (ii < graph_data.getNumYValues()) &&
		(jj > margin) &&
		(graph_size.width > 0) &&
		(origin.y > margin); 
	    ii+=num_in_group, jj -= y_bin_gap)
	    {
		x_coord1 = origin.x + 1;
		for(int kk = 0; kk < graph_data.getNumXValues(); 
		    kk++)
		    if(graph_data.isEnabled(kk))
			{
			    brush.setColor
				(graph_data.getColor(kk));
			    cal_assist1 = 0;
			    for(int ll = 0;(ll < num_in_group) &&
				    ((ll + ii) < graph_data.getNumYValues()); 
				ll++)
				cal_assist1 += 
				    graph_data.getElementAt(kk,ii + ll);
			    
			    x_coord2 = x_coord1 + 
				(int)((cal_assist1 * graph_size.width) /
				      x_range_max);
			    brush.fillRect(x_coord1, jj + 1, 
					   x_coord2 - x_coord1,
					   bar_width);
			    brush.setColor(Color.black);
			    if(bar_width > 1) 
				brush.drawRect(x_coord1 - 1, jj, 
					       x_coord2 - x_coord1,
					       bar_width + 1);
			    x_coord1 = x_coord2;

			}
	    }
	brush.setColor(Color.black);
    }
    
    private void initNumInGroup(int y_bin_gap, Dimension graph_size)
    {
	if(graph_size.height > 0) {
	    if(((y_bin_gap * graph_data.getNumYValues()) % 
		graph_size.height) == 0)
		num_in_group = (y_bin_gap * graph_data.getNumYValues()) / 
		    graph_size.height;
	    else if(((y_bin_gap * graph_data.getNumYValues()) % 
		     graph_size.height) != 0)
		num_in_group = ((y_bin_gap * graph_data.getNumYValues()) / 
				graph_size.height) + 1;
	    if(num_in_group == 0)
		{
		    num_in_group ++;
		}
	}
    }
}






