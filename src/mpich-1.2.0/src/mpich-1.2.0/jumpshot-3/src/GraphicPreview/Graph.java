import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

/*********Class Graph***********/
/*
  This class defines the basic panel on which the actual graph is painted.
  There are no components contained in the panel. The graph is first painted on
  an off-screen image and then that image is drawn on the panel. There is a
  choice between a line graph and a bar graph.
*/


public class Graph extends JPanel
{
    /*----constant class variables----*/
    
    //----size of x-axis in pixels = graph_size.width
    //----size of y-axis in pixels = graph_size.height 
    public final static int
	GRAPH_MARGIN = 20,    //--constant pixel margin between graph and
	                      //--panel edge.
	LINE=1,BAR=2,         //--constants for graph type.
	MIN_WIDTH = 512,
	FIXED_GRAPH_HEIGHT = 256,
	MIN_XLABL_SIZE = 60;


    /*----instance variables----*/
    
    public  Dimension graph_size = new Dimension(MIN_WIDTH,FIXED_GRAPH_HEIGHT);
    private Vector    dataVec;
    private YAxisPanel YAxis;
    private XAxisEndPanel XAxisEnd;
    private int       num_data_pts;
    private Color     pre_defn_colors[] 
	= { Color.blue, Color.green, Color.orange, Color.darkGray,
	    Color.gray, Color.cyan, Color.magenta, Color.yellow, Color.pink,
	    Color.black };
    private double   
	x_range_min,      //--the lower time bound in time units.
	x_range_max,      //--the upper time bound in time units.
	x_bin_time,       //--the time allocated to each bin.
	y_range_min = 0,  //--min y-value assumed to be 0.
	y_range_max,
	y_range_max_line, //--max y-value. 
	y_range_max_bar;
	
    /*
      a bin is a pixel representation of the time interval between each value
      recorded.
    */

    private int      
	x_bin_gap,       //--size of bin in pixels. 
	graph_type = 2,  //--default is line graph.  
	x_labl_size = 60,//--min pixel gap between label on x-axis.
        time_flag = 0,   //-- 0=> time in seconds else time in hours
	y_bin_gap_min = 40,  //--min pixel gap between labels on the
                         //--y-axis.
	x_scale_index = 0,
	x_scale_string_width = 6,
	y_bin_gap=40,
	plotting_height = 256,
	num_in_group = 1, //-- number of data points to be grouped for display
	                  //-- default is one data point.
	zoom_level = 0,
	viewportWidth = 0;
    
    

    private boolean reScaleFlag = true,
	            drawAgain   = true;
    private Dimension display_size;   //--total size of panel.
    private Point     origin,         //--x,y pixel coordinates of origin on
                                      //--on panel.
	              markd_view = new Point(0,0);
    private Image     virtual_canvas;
    private Graphics  brush;
    private Font      font;           //--font type. 
    private int       markd;          //--flag if point selected on graph. It
                                      //--also contains the x coord for that 
                                      //--point.
    
    /*
      constructor -
      requires data set, the lower time bound and upper time bound
    */
    public Graph(double data[][], String data_labels[],
		 double x_range_min, double x_range_max)
    {
	int i, k, max_index, min_index, color_counter;
	double cal_assist1, cal_assist2;
	String temp_string;
	/*
	addComponentListener(new ComponentAdapter() {
	    public void componentResized(ComponentEvent e){
		((Graph)e.getSource()).repaint();
	    }
	});
	*/
	dataVec = new Vector();
	markd = 0;
	font = new Font("Dialog",Font.PLAIN,12);

	if(data.length < 1)
	    {
		System.out.print("No data values passed."+
				 " Terminating program.");
		System.exit(1);
	    }
	
    
	for(i=0, color_counter=0;i<data.length;i++)
	    {
		if(color_counter < pre_defn_colors.length)
		    dataVec.addElement(new Graph_data
				       (data_labels[i],data[i],true,
					pre_defn_colors[color_counter++]));
		else
		    dataVec.addElement(new Graph_data
				       (data_labels[i],data[i]));
	    }
				    
						  
	this.x_range_max = x_range_max;
	this.x_range_min = x_range_min;
	if(x_range_max > 3600)
	    {
		time_flag = 1;
		this.x_range_min=this.x_range_min/3600;
		this.x_range_max=this.x_range_max/3600;
	    }

	//** the following paragraph of code generates the scale of the graph
	//** in terms of an integer index of 10 
	//** for the case where max is either too large or too small.

	max_index = (int)(Math.log(this.x_range_max)/Math.log(10.0));
	if(max_index > (x_scale_string_width-1))
	    x_scale_index = max_index - x_scale_string_width;
	else if( max_index < ((-1)*(x_scale_string_width-2)))
	    x_scale_index = (x_scale_string_width-2)+max_index;

	reScale();
	temp_string = Double.toString(y_range_max_bar);
	temp_string = temp_string.substring(0,
					    temp_string.indexOf((int)'.') +
					    2);

	
	origin = new Point(0,
			   GRAPH_MARGIN + graph_size.height);
	display_size = new Dimension(origin.x + 
				     graph_size.width +
				     GRAPH_MARGIN,
				     origin.y + (3 * GRAPH_MARGIN));
	Dimension yaxis_dim = new Dimension
	    (GRAPH_MARGIN + 11 +
	     getFontMetrics(font).stringWidth(temp_string),
	     display_size.height);
	Point originY = new Point(yaxis_dim.width - 6,origin.y);

	YAxis = new YAxisPanel(this,originY,yaxis_dim);
	XAxisEnd = new XAxisEndPanel(this, new Point(origin));

	setPreferredSize(display_size);
    }

	
    /*
      this is where all the drawing takes place as well as the initialization 
      for the graphics context.
    */
    public void paint(Graphics g)
    {
	super.paint(g);
	g.setColor(Color.white);
	
	virtual_canvas = 
	    createImage(display_size.width,display_size.height);
	brush = virtual_canvas.getGraphics();

	//** standardizing the font for the axis labels.

	font = brush.getFont();

	reScale();

	//** initializing the label size so as to be a multiple of the bin size
	//** and also to make sure that the labels dont overlap.

	x_labl_size = getFontMetrics(font).stringWidth("digitsize");
	if(x_bin_gap >= MIN_XLABL_SIZE)
	    x_labl_size = x_bin_gap;
	else
	    x_labl_size = ((x_labl_size / x_bin_gap) + 1) * x_bin_gap;

	//** drawing the axis

	drawXAxis();

	if(graph_type == Graph.BAR)
	    drawBarGraph();
	else
	    drawLineGraph();

	g.drawImage(virtual_canvas,0,0,Color.white,this);

	YAxis.repaint();
	
	//** draw the marker if the point was selected

	if(markd != 0)
	    {
		g.setColor(Color.red);
		g.drawLine(markd,origin.y + 6,markd,
			   origin.y-graph_size.height-6);
	    }


	g.setColor(Color.black);
	g.drawLine(origin.x, origin.y,getWidth(),
		       origin.y);
	g.setColor(Color.white);
			
	brush.dispose();
    }

    /*
      draws both the x-axis and the y-axis with all the labels.
    */
    private void drawXAxis()
    {
	int cal_assist1,cal_assist2;

	brush.setColor(Color.white);
	brush.fillRect
	    (0,0,virtual_canvas.getWidth(this),virtual_canvas.getHeight(this));
	brush.setColor(Color.black);



	//** draw labels on x-axis

	String temp;
	int i;
	for(i=origin.x;
	    (((i-origin.x)/x_bin_gap)*x_bin_time+x_range_min) < x_range_max; 
	    i+=x_labl_size)
	    {
		brush.drawLine(i,origin.y,i,origin.y+4);
		if(i>origin.x)
		    centerLabelOnX(i);
		else
		    {
			temp = calculateXValueString(origin.x,7);
			temp = temp.substring(0,temp.indexOf('.'));
			brush.drawString(temp,
					 origin.x,
					 origin.y + 
					 getFontMetrics(font).getMaxAscent() +
					 6);
		    }
			
	    }

	temp = "time ( ";
	if(x_scale_index != 0)
	    temp += "x10    ";
	
	if(time_flag == 0)
	    temp += "seconds )";
	else
	    temp += "hours )";
	

  
	if((x_scale_index !=  0)&&(x_scale_index != 1))
	    {
		brush.setFont(new Font(brush.getFont().getName(),
				       brush.getFont().getStyle(),
				       9));
		brush.drawString(Integer.toString(x_scale_index),
				 origin.x + (graph_size.width/2) - 
				 (getFontMetrics(font).stringWidth(temp)/2)+
				 (getFontMetrics(font).
				  stringWidth("time ( x10")),
				 origin.y +
				 4*getFontMetrics(font).getMaxAscent()-4);
		brush.setFont(font);
	    }

	brush.drawString(temp,
			 origin.x + (graph_size.width/2) - 
			 (getFontMetrics(font).stringWidth(temp)/2),
			 origin.y + 4*getFontMetrics(font).getMaxAscent());

	//** draw the marker to show the max x-value
	brush.setColor(Color.red);
	brush.drawLine(origin.x + (x_bin_gap * num_data_pts),origin.y,
		       origin.x + (x_bin_gap * num_data_pts),origin.y+8);

	temp = calculateXValueString((x_bin_gap*num_data_pts)+origin.x,6);

	brush.drawString(temp,
			 origin.x + (x_bin_gap * num_data_pts)- (int)
			 (0.75*getFontMetrics(font).stringWidth(temp)),
			 origin.y + 2*getFontMetrics(font).getMaxAscent()+8);

	brush.setColor(Color.black);
    }

    /*
      this returns the dimensions of the panel
    */
    public Dimension getDimension()
    {
	return (new Dimension(display_size.width + YAxis.getDimension().width,
			      display_size.height));
    }

    public int getPlotting_height()
    {
	return plotting_height;
    }

    public double getYRangeMax()
    {
	return y_range_max;
    }
    
    public int getYBinGap()
    {
	return y_bin_gap;
    }

    /*
      centers the label at the given position under the x-axis
    */
    private void centerLabelOnX(int x_position)
    {
	String value_s =  calculateXValueString(x_position,6);
	brush.drawString(value_s,
		      x_position-(getFontMetrics(font).stringWidth(value_s)/2),
		      (origin.y+6)+(getFontMetrics(font).getMaxAscent()));
    }

    /*
      draws the entire bar graph in one shot
    */
    private void drawBarGraph()
    {
	int i,j,y_coord1=0,y_coord2=0,bar_width=1;
	double cal_assist1;
	brush.setColor(Color.blue);
	
	//** calculating bar width so as not to intersect any two bars 
	if(x_bin_gap > 3)
	    if(x_bin_gap%2 == 0)
		bar_width = (x_bin_gap-1);
	    else bar_width = (x_bin_gap-2);

	for(i=0,j=origin.x+x_bin_gap;
	    i<(num_data_pts * num_in_group);i+=num_in_group,j+=x_bin_gap)
	    {
		int k;
		y_coord1 = origin.y;
		for(k=0;k<dataVec.size();k++)
		    if(((Graph_data)dataVec.elementAt(k)).isEnabled())
			{
			    brush.setColor
				(((Graph_data)dataVec.elementAt(k)).
				 getColor());
			    cal_assist1 = 0;
			    
			    int l;
			    for(l=0;l<num_in_group;l++)
				cal_assist1 +=  
				    ((Graph_data)dataVec.elementAt(k)).
				    getElementAt(i+l); 

			    y_coord2 = y_coord1 - 
				(int)((cal_assist1 / y_range_max) * 
				      plotting_height);

			    brush.fillRect(j-bar_width+1, y_coord2,
					   bar_width,y_coord1 - y_coord2);
			    y_coord1 = y_coord2;
			}
	    }
	brush.setColor(Color.black);
    }

    /*
      draws line graph in one shot
    */
    private void drawLineGraph()
    {
	int i,j,k,y_coord1=0,y_coord2;
	double cal_assist1;
	brush.setColor(Color.blue);
	
	//** making sure there is data

	for(k=0;k<dataVec.size();k++)
	    if(((Graph_data)dataVec.elementAt(k)).isEnabled())
		{
		    brush.setColor
			(((Graph_data)dataVec.elementAt(k)).getColor());
		    if(num_data_pts > 0)
			y_coord1 = origin.y;
		    for(i=0,j=origin.x;i<(num_data_pts * num_in_group);
					  i+= num_in_group,j+=x_bin_gap)
			{
			    cal_assist1 = 0;
			    
			    int l;
			    for(l=0;l < num_in_group;l++)
				cal_assist1 += 
				    ((Graph_data)dataVec.elementAt(k)).
				    getElementAt(i+l);

			    y_coord2 = origin.y - (int)((cal_assist1 /
				  (y_range_max-y_range_min))
				 * plotting_height);
				
			    brush.drawLine(j,y_coord1,j +
					   x_bin_gap,y_coord2);
			    
			    y_coord1 = y_coord2;
			}
		}
	brush.setColor(Color.black);
    }

    public double getXBinTime()
    {
	return x_bin_time;
    }

    public int getXBinGap()
    {
	return x_bin_gap;
    }

    public Point getOrigin()
    {
	return new Point(origin.x,origin.y);
    }

    public double getMinTimeVal()
    {
	return x_range_min;
    }
    
    /*
      changes the graph representation
    */
    public void changeGraphType(int type)
    {
	this.graph_type = type;
	repaint();
    }
    
    /*
      returns the range of the graph in pixels from x_min to x_max
    */
    public int getGraphRange()
    {
	return (x_bin_gap * num_data_pts);
    }
    
    public Color getColor(int data_id)
    {
	return ((Graph_data)dataVec.elementAt(data_id)).getColor();
    }
    
    public void changeColor(int data_id, Color c)
    {
	((Graph_data)dataVec.elementAt(data_id)).changeColor(c);
    }

    public void setState(int data_id,boolean state)
    {
	((Graph_data)dataVec.elementAt(data_id)).setEnabled(state);
    }
    public boolean isEnabled(int i)
    {
	return ((Graph_data)dataVec.elementAt(i)).isEnabled();
    }
    public String getLabel(int i)
    {
	return ((Graph_data)dataVec.elementAt(i)).getLabel();
    }

    /*
      This function returns a string representing the scaled value.
      -precision is a positive number defining the max number of characters
      in the returned string.
      -x_pos is the position on the graph for which a string representation
      of the time is required.
    */
    public String calculateXValueString(int x_pos,int precision)
    {
	String value_s,temp_string;
	double value;
	int indexValue,
	    min_displyd_char = 2;//-- example: "0.0.......1"
	                         //-- the "0." is 2 char long.

	value = (((double)(x_pos-origin.x)/x_bin_gap)*x_bin_time)+
	    x_range_min;

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
		if(value_s.indexOf((int)'.') == (value_s.length()-1))
		    value_s = value_s.substring(0,value_s.length()-1);
	    }
	return new String(value_s);
    }	
    
    /*
      This function accepts a time value and moves the marker on the graph
      to the cooresponding value on the graph.
    */
    public void moveMarkerTo(double time)
    {
	int cal_assist1 = (int)((time/this.x_bin_time)*x_bin_gap)+origin.x;
	if(markd != 0)
	    markd_view.setLocation(markd_view.x + (cal_assist1 - markd),0);
	this.markd = cal_assist1;
	repaint();
    }
    
    private void reScale()
    {
	double cal_assist1, 
	       cal_assist2;
	int cal_assist3,
	    cal_assist4;

	//** the following code performs the task of grouping data points 
	//** together if the number of data points is greater than the minimum
	//** display width. We know there is at least one data set in
	//** dataVec and hence the index 0. We have already checked for this 
	//** in an if statement at the beginning of the constructor.
	
	if((((Graph_data)dataVec.elementAt(0)).getLength() %
	               graph_size.width) == 0)
	    num_in_group = (((Graph_data)dataVec.elementAt(0)).getLength() / 
			    graph_size.width);
	else
	    num_in_group = (((Graph_data)dataVec.elementAt(0)).getLength() / 
	               graph_size.width) + 1;
	num_data_pts = ((Graph_data)dataVec.elementAt(0)).getLength() /
	               num_in_group;



	this.x_bin_gap = graph_size.width / (num_data_pts);

	if(this.x_bin_gap == 0)
	    {
		System.out.println("Number of values to be graphed exceeds" + 
				   " the maximum display size of 500 pixels");
		System.exit(1);
	    }

	x_bin_time = ((double)x_bin_gap / ((num_data_pts) * x_bin_gap)) *
	    (this.x_range_max - this.x_range_min);

	if(reScaleFlag)
	    {
		y_range_min = 0;
		y_range_max_line = 0;
		y_range_max_bar = 0;
		
		//** finding the largest value in the data set to initialize
		//**  the max y value.

		int i,k;
		for (i=0;i < (num_data_pts * num_in_group);i+=num_in_group)
		    {
			int j;
			cal_assist1 = 0;
			for(j=0;j < dataVec.size();j++)
			    {
				if(((Graph_data)dataVec.elementAt(j)).
				   isEnabled())
				    {
					cal_assist2 = 0;
					for(k=0;k < num_in_group;k++)
					    cal_assist2 += 
						((Graph_data)dataVec.
						 elementAt(j)).
						getElementAt(i+k);
					cal_assist1 += cal_assist2;
					if(cal_assist2 > y_range_max_line)
					    y_range_max_line = cal_assist2;
				    }
			    }
			if(cal_assist1 > y_range_max_bar)
			    y_range_max_bar = (double)cal_assist1;
		    }
	
		//** the max y value is always a bit higher than the actual
		//**  max data value by 20 pixels.
	
		y_range_max_bar += (((double)20 / graph_size.height) * 
				    (y_range_max_bar-y_range_min));
		y_range_max_line += (((double)20 / graph_size.height) * 
				     (y_range_max_line-y_range_min));
		
		if(graph_type == Graph.BAR)
		    y_range_max = y_range_max_bar;
		else
		    y_range_max = y_range_max_line;
	
		//** whenever all data sets are disabled y_range_max gets
		//**  calculated as zero and hence the following if statement.
		if(y_range_max < 1)
		    y_range_max = 1;
		
		
		y_range_max = Math.rint(y_range_max + 0.5); 
		
		if(y_range_max < graph_size.height)
		    {
			plotting_height = (int)
			    ((int)(graph_size.height/y_range_max) *
				       y_range_max);
			cal_assist3 = (int)(plotting_height / y_range_max);
			cal_assist4 = (y_bin_gap_min/cal_assist3) * 
			    cal_assist3;
			if(cal_assist4 < y_bin_gap_min)
			    y_bin_gap = cal_assist4 + cal_assist3;
			else
			    y_bin_gap = y_bin_gap_min;
		    }
	    }
    }

    /*
      marks the point selected in the graph by setting the value of 
      marker to the time value corresponding to that point.
    */
    public double mark_time(Point marker, Point viewPosition)
    {
	int cal_assist1;
	markd_view.setLocation(viewPosition.x,0);
	markd = marker.x;
	repaint();
	if(x_range_max > 3600)
	    return ((((double)(marker.x-origin.x)/x_bin_gap)*x_bin_time)+
		    x_range_min)*3600;
	else
	    return (((double)(marker.x-origin.x)/x_bin_gap)*x_bin_time)+
		x_range_min;	
    }
    
    public boolean zoomIn()
    {
	int cal_assist1;
	if(graph_size.width < Math.pow(2,14))
	    {
		zoom_level++;
		cal_assist1 = markd - markd_view.x;
		markd*=2;
		markd_view.setLocation(markd - cal_assist1,0);
		setViewportWidth(viewportWidth);
	        repaint();
	    }
        if(graph_size.width > Math.pow(2,13))
	    return false;
	else
	    return true;
    }
    
    public boolean zoomOut()
    {
	int cal_assist1;
	if(zoom_level > 0)
	    {
		zoom_level--;
		cal_assist1 = markd - markd_view.x;
		markd/=2;
		if(zoom_level != 0)
		    markd_view.setLocation(markd - cal_assist1,0);
		else
		    markd_view.setLocation(0,0);
		setViewportWidth(viewportWidth);
		repaint();
	    }
	if(zoom_level == 0)
	    return false;
	else 
	    return true;
    }

    public void reset()
    {
	markd/=Math.pow(2,zoom_level);
	zoom_level = 0;
	markd_view.setLocation(0,0);
	setViewportWidth(viewportWidth);
	repaint();
    }
    
    public YAxisPanel getYAxis()
    {
	return YAxis;
    }
    public Dimension getXAxisDimension()
    {
	return (new Dimension(display_size));
    }
    public Point getMarkdView()
    {
	return new Point(markd_view);
    }
    public void setReScale(boolean value)
    {
	reScaleFlag = value;
    }
    public void setViewportWidth(int width)
    {
	int graph_width   = MIN_WIDTH * (int)Math.pow(2,zoom_level),
	    display_width = origin.x + graph_width + GRAPH_MARGIN;

	viewportWidth = width;

	if(display_width < viewportWidth)
	    {
		graph_width   = graph_width * viewportWidth / display_width;
		display_width = viewportWidth;
	    }
	display_size = new Dimension(display_width,
				     origin.y + (3 * GRAPH_MARGIN));
	graph_size = new Dimension(graph_width,
				   FIXED_GRAPH_HEIGHT);  
	setSize(display_size);
	setPreferredSize(display_size);
	repaint();

    }
    public XAxisEndPanel getXAxisEnd()
    {
	return XAxisEnd;
    }
}
	



