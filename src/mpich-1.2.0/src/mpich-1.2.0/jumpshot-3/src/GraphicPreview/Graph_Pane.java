import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;

/********Class Graph_Pane******/
/*
  this class contains the graph as well as the other components
  that either communicate with the graph or alter the graph.
  Components:
  1 Menu Bar 
    -- Graph representation menu
  2 Title on a panel
  3 Time Value display
  
  This class is also an event listener for these components
  To make Graph_Pane communicate the marked time to the top-level container,
  the container needs to be passed as an object to this class using the second
  constructor, the event code in the mouseClicked function needs to be changed 
  - add the necessary function to the container and utillize it in the if 
    statement, replace the (Graph_Pane) explicit typecasting with the
    (container class) type cast. which basically means that you need to also
    provide a double variable in the container called markd_time.
*/
public class Graph_Pane extends JPanel
implements MouseMotionListener,MouseListener,ActionListener
{
    private JFrame owner;
    private Dimension dimensions;
    private JPanel title;
    private JLabel title_label;
    private Graph graph;
    private TimeInterface time_listener;
    private JTextField track_time;
    private JMenu legend_menu;
    private JCheckBoxMenuItem legend[];
    private JScrollPane graph_scroller;
    private JList legend_list;
    JDialog legend_dialog;
    private ImageIcon legend_images_selected[];
    private ImageIcon legend_images[];
    private boolean need_color_change;
    JMenuItem zoomIn,zoomOut;

    Cursor default_cur = new Cursor(Cursor.DEFAULT_CURSOR),
	on_graph = new Cursor(Cursor.CROSSHAIR_CURSOR);

    
    /*
      Constructor
    */
    public Graph_Pane
	(JFrame parent,String label,double values[][],String value_labels[],
	 double start_time,double end_time)
	
    {
	super(new GridBagLayout());

	Graph_data.resetUsedColors();

	owner = parent;
	need_color_change = true;
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.anchor = GridBagConstraints.CENTER;
	constraints.weightx = 100;

	legend_images = new ImageIcon[value_labels.length];
	legend_images_selected = new ImageIcon[value_labels.length];

	JMenuBar menubar = new JMenuBar();
	menubar.setLayout(new FlowLayout(FlowLayout.LEFT,5,5));
	JMenu menu = new JMenu("File");
	menubar.add(menu);
	JMenuItem item = new JMenuItem("View Statistics");
	item.addActionListener(this);
	menu.add(item);
	item = new JMenuItem("Close");
	item.addActionListener(this);
	menu.add(item);

	menu = new JMenu("Graph");
	menubar.add(menu);
	JMenu submenu = new JMenu("Type");
	item = new JMenuItem("Bar");
	item.addActionListener(this);
	submenu.add(item);
	item = new JMenuItem("Line");
	item.addActionListener(this);
	submenu.add(item);
	menu.add(submenu);
	submenu = new JMenu("ReScale");
	ButtonGroup group = new ButtonGroup();
	JRadioButtonMenuItem ritem = new JRadioButtonMenuItem("On");
	ritem.addActionListener(this);
	ritem.setSelected(true);
	group.add(ritem);
	submenu.add(ritem);
	ritem = new JRadioButtonMenuItem("Off");
	ritem.addActionListener(this);
	group.add(ritem);
	submenu.add(ritem);
	menu.add(submenu);

	menu = new JMenu("Data Sets");
	menubar.add(menu);
	item = new JMenuItem("All");
	item.addActionListener(this);
	menu.add(item);
	item = new JMenuItem("None");
	item.addActionListener(this);
	menu.add(item);

	menu = new JMenu("Zoom");
	menubar.add(menu);
	zoomIn = new JMenuItem("In");
	zoomIn.addActionListener(this);
	menu.add(zoomIn);
	zoomOut = new JMenuItem("Out");
	zoomOut.addActionListener(this);
	zoomOut.setEnabled(false);
	menu.add(zoomOut);
	item = new JMenuItem("Reset");
	item.addActionListener(this);
	menu.add(item);

        menu = new JMenu("Legend");
	menubar.add(menu);
	item = new JMenuItem("Display");
	item.addActionListener(this);
	menu.add(item);
	item = new JMenuItem("Hide");
	item.addActionListener(this);
	menu.add(item);
		
	menubar.setBorder(new EtchedBorder());
	buildConstraints(constraints, 0, 0, 3, 1, 
			 GridBagConstraints.HORIZONTAL);
	add(menubar,constraints);
	
	title = new Graph_Title(label);
	JLabel temp_label = new JLabel("Time:",JLabel.CENTER);
	title.add(temp_label);
	track_time = new JTextField("0.0",7);
	track_time.setEnabled(false);
	title.add(track_time);
	buildConstraints(constraints, 0, 1, 3, 1, 
			 GridBagConstraints.HORIZONTAL);
	add(title,constraints);

	graph = new Graph(values,value_labels,start_time, end_time);
	dimensions = graph.getDimension();	
	graph_scroller = 
	    new JScrollPane(graph, JScrollPane.VERTICAL_SCROLLBAR_NEVER,
			    JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
	YAxisPanel YAxis = graph.getYAxis();
	constraints.anchor = GridBagConstraints.NORTHEAST;
	buildConstraints(constraints, 0, 2, 1, 1, 
			 GridBagConstraints.HORIZONTAL);
	constraints.weightx = 0;
	add(YAxis,constraints);	

	graph_scroller.setBorder(new LineBorder(Color.white));
	graph_scroller.getHorizontalScrollBar().
	    setBorder(new ScrollBarBorder());
	graph_scroller.addComponentListener
	    ( new ComponentAdapter() {
		public void componentResized(ComponentEvent e) {
		    graph.setViewportWidth
			(((JScrollPane)e.getSource()).getWidth());
		}
	    });

	YAxis.shiftOrigin(0,1);
	constraints.anchor = GridBagConstraints.CENTER;
	constraints.weightx = 100;
	buildConstraints(constraints, 1, 2, 1, 1, 
			 GridBagConstraints.HORIZONTAL);
	add(graph_scroller,constraints);    
	graph.addMouseMotionListener(this);
	graph.addMouseListener(this);

	XAxisEndPanel XAxisEnd = graph.getXAxisEnd();
	XAxisEnd.shiftOrigin(0,1);
	constraints.anchor = GridBagConstraints.NORTHEAST;
	buildConstraints(constraints, 2, 2, 1, 1, 
			 GridBagConstraints.HORIZONTAL);
	constraints.weightx = 0;
	add(XAxisEnd,constraints);    

	makeLegendDialog(value_labels);
	
	temp_label.setBackground(Color.white);
	title.setBackground(Color.white);

	Dimension temp_dimension = 
	    new Dimension(graph.getXAxisDimension().width,
			  dimensions.height+15);
	setObjectSize(graph_scroller, temp_dimension);

	temp_dimension = new Dimension(YAxis.getDimension().width,
				       dimensions.height+15);
	setObjectSize(YAxis, temp_dimension);
	setObjectSize(XAxisEnd, temp_dimension);
	
	setBackground(Color.white);
    }
    public void init()
    {
    }
    
    public void paint(Graphics g)
    {
	int i;
	if(need_color_change)
	    {
		Image temp_image;
		Graphics renderer;
		
		for(i=0;i<legend_images.length;i++)
		    {
			temp_image = createImage(15,15);  
			renderer = temp_image.getGraphics();
			renderer.setColor(Color.white);
			renderer.fillRect(0,0,15,15);		
			renderer.setColor(Color.black);
			renderer.drawArc(0,0,12,12,0,360);
			legend_images[i] = new ImageIcon(temp_image); 
			renderer.dispose();
			temp_image = createImage(15,15);
			renderer = temp_image.getGraphics();
			renderer.setColor(Color.white);
			renderer.fillRect(0,0,15,15);	
			renderer.setColor(Color.black);
			renderer.drawArc(0,0,12,12,0,360);	
			renderer.setColor(graph.getColor(i));
			renderer.fillArc(2,2,8,8,0,360);
			legend_images_selected[i] = 
			    new ImageIcon(temp_image); 
			renderer.dispose();
		    }
		need_color_change = false;
	    }
	super.paint(g);
    }

    public Dimension getDimension()
    {
	return new Dimension(dimensions);
    }

    /*
      This function accepts a time value and moves the marker on the graph
      to the cooresponding value on the graph.
    */
    void moveMarkerTo(double time)
    {
	graph.moveMarkerTo(time);
    }

    public void setTimeListener(TimeInterface ti)
    {
	time_listener = ti;
    }

    public void mousePressed(MouseEvent e)
    {
    }
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}

    public void mouseClicked(MouseEvent e)
    {

	if((time_listener != null) /*&& (e.getClickCount()==2)*/)
	    {
		Point coords = e.getPoint(),
		    graph_origin = graph.getOrigin();
		if((coords.x >= graph_origin.x) && 
		   (coords.x <= (graph_origin.x + graph.getGraphRange())) &&
		   (coords.y <= graph_origin.y) &&
		   (coords.y > graph.GRAPH_MARGIN))
		    time_listener.showTime
			(graph.mark_time
			 (coords,
			  graph_scroller.getViewport().getViewPosition())); 
	    }
    }

    public void mouseMoved(MouseEvent e)
    {
	Point coords = e.getPoint(),
	    graph_origin = graph.getOrigin();
	String time_value_s;

        if((coords.x >= graph_origin.x) && 
	   (coords.x <= (graph_origin.x + graph.getGraphRange())) &&
	   (coords.y <= graph_origin.y) &&
	   (coords.y > graph.GRAPH_MARGIN))
	    {
		graph.setCursor(on_graph);
		time_value_s = graph.calculateXValueString(coords.x,8);
		track_time.setText(time_value_s);
	    }
	else
	    graph.setCursor(default_cur);
		
    }
    public void mouseDragged(MouseEvent e){}
    public void actionPerformed(ActionEvent e)
    {
	if(e.getActionCommand().compareTo("Line") == 0)
	    graph.changeGraphType(Graph.LINE);
	else if(e.getActionCommand().compareTo("Bar") == 0)
	    graph.changeGraphType(Graph.BAR);
	else if(e.getActionCommand().compareTo("All") == 0)
	    {
		for(int i=0; (i < legend_images.length);i++)
		    {
			graph.setState(i,true);
		    }
		legend_list.repaint();
		graph.repaint();
	    }
	else if(e.getActionCommand().compareTo("None") == 0)
	    {
		for(int i=0; (i < legend_images.length);i++)
		    {
			graph.setState(i,false);
		    }
		legend_list.repaint();
		graph.repaint();
	    }
	else if(e.getActionCommand().compareTo("View Statistics") == 0)
	    {
                /*
		String args[]={};
		UTEStatsViewer.Main.main(args);
                */
                StatisticsViewer stats_viewer = new StatisticsViewer();
	    }
	else if(e.getActionCommand().compareTo("In") == 0)
	    {
		if(graph.zoomIn() == true)
		    {			
			((JMenuItem)e.getSource()).setEnabled(true);
			zoomOut.setEnabled(true);
		    }
		else
		    ((JMenuItem)e.getSource()).setEnabled(false);
		graph_scroller.getViewport().setViewPosition
		    (graph.getMarkdView());
		repaint();
	    }
	else if(e.getActionCommand().compareTo("Out") == 0)
	    {
		if(graph.zoomOut() == true)
		    {
			((JMenuItem)e.getSource()).setEnabled(true);
			zoomIn.setEnabled(true);
		    }
		else
		    ((JMenuItem)e.getSource()).setEnabled(false);
		graph_scroller.getViewport().setViewPosition
		    (graph.getMarkdView());
		repaint();
	    }
	else if(e.getActionCommand().compareTo("Reset") == 0)
	    {
		zoomIn.setEnabled(true);
		zoomOut.setEnabled(false);
		graph.reset();
		repaint();
	    }
	else if(e.getActionCommand().compareTo("Display") == 0)
	    {
		legend_dialog.pack();
		legend_list.setSelectedIndex(-1);
		legend_dialog.setVisible(true);
	    }
	else if(e.getActionCommand().compareTo("Hide") == 0)
	    {
		legend_dialog.pack();
		legend_list.setSelectedIndex(-1);
		legend_dialog.setVisible(false);
	    }
	else if(e.getActionCommand().compareTo("Select") == 0)
	    {
		int selected[] = legend_list.getSelectedIndices();
		for(int i=0; i < selected.length;i++)
		    graph.setState(selected[i],!graph.isEnabled(selected[i]));
		graph.repaint();
		legend_list.repaint();
	    }
	else if(e.getActionCommand().compareTo("Change Color") == 0)
	    {
		Color color;
		int selected[] = legend_list.getSelectedIndices();
		for(int i=0; i < selected.length;i++)
		    {
			color = ColorChooser.showDialog
			    (null, "Color Chooser",
			     graph.getColor(selected[i]));
			graph.changeColor(selected[i],color);
		    }
		need_color_change = true;
		repaint();
		graph.repaint();
		legend_list.repaint();
	    }
	else if(e.getActionCommand().compareTo("On") == 0)
	    {
		graph.setReScale(true);
		graph.repaint();
	    }
	else if(e.getActionCommand().compareTo("Off") == 0)
	    graph.setReScale(false);
	else if((time_listener != null) &&
		(e.getActionCommand().compareTo("Close") == 0)) {
	    legend_dialog.setVisible(false);
	    legend_dialog.dispose();
	    time_listener.closePreview();
	}
    }
    class LegendListCellRenderer extends JLabel implements ListCellRenderer
    {
	public LegendListCellRenderer()
	{
	    super();
	    setBorder(new LineBorder(Color.white,2));
	    setOpaque(true);
	}
	public Component getListCellRendererComponent
	(JList list, Object value, int index, boolean isselected, 
	 boolean cellHasFocus)
	{
	    setText("  " + graph.getLabel(index));
	    if(isselected)
		{
		    setBackground(Color.black);
		    setForeground(Color.white);
		}
	    else
		{
		    setForeground(Color.black);
		    setBackground(Color.white);
		}
	    if(graph.isEnabled(index))
		setIcon(legend_images_selected[index]);
	    else
		setIcon(legend_images[index]);
 
	    repaint();
	    return this;
	}
    }
    private void makeLegendDialog(String value_labels[])
    {
	GridBagConstraints constraints = new GridBagConstraints();
	Dimension dim;
	legend_dialog = new JDialog(owner,"Legend");
	legend_dialog.getContentPane().setLayout(new GridBagLayout());

        legend_list = new JList(value_labels);
	legend_list.setSelectionBackground(Color.black);
	legend_list.setCellRenderer(new LegendListCellRenderer());
	constraints.weightx = 100;
	constraints.weighty = 100;
	buildConstraints(constraints, 0,0,2,1,GridBagConstraints.BOTH); 
	legend_dialog.getContentPane().add(new JScrollPane(legend_list),
					   constraints);
	constraints.weightx = 0;
	constraints.weighty = 0;

	JButton button = new JButton("Select");
	button.addActionListener(this);
	buildConstraints(constraints, 0,1,2,1,GridBagConstraints.BOTH); 
	legend_dialog.getContentPane().add(button,constraints);
	legend_dialog.getContentPane().add("Center",button);
  
	button = new JButton("All");
	button.addActionListener(this);
	buildConstraints(constraints, 0,2,1,1,GridBagConstraints.BOTH); 
	legend_dialog.getContentPane().add(button,constraints);

	button = new JButton("None");
	button.addActionListener(this);
	buildConstraints(constraints, 1,2,1,1,GridBagConstraints.BOTH); 
	legend_dialog.getContentPane().add(button,constraints);

	button = new JButton("Change Color");
	button.addActionListener(this);
	buildConstraints(constraints, 0,3,2,1,GridBagConstraints.BOTH); 
	legend_dialog.getContentPane().add(button,constraints);

				     
	legend_dialog.pack();
	
    }
    
    private void buildConstraints(GridBagConstraints constraints,
				  int gridx, int gridy,
				  int gridwidth, int gridheight,
				  int fill)
    {
	constraints.gridx      = gridx;
	constraints.gridy      = gridy;
	constraints.gridwidth  = gridwidth;
	constraints.gridheight = gridheight;
	constraints.fill       = fill;
    }
    private void setObjectSize(JComponent obj, Dimension dim)
    {
       	obj.setPreferredSize(dim);
	obj.setMaximumSize(dim);
	obj.setMinimumSize(dim);
    }	

}








