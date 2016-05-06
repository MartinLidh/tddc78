import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

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
public class StatisticsViewer extends JFrame
    implements /*MouseMotionListener,MouseListener,*/ ActionListener
{
    private Dimension dimensions;
    private Statistics_Data data_aray[];
    private int selected_index;
    private Stat_Graph_Title title;
    private JLabel title_label;
    private Stat_Graph graph;
    private StatTimeInterface time_listener;
    private JTextField track_time;
    private JMenu graph_menu;
    private JMenuItem graph_list[];
    private JList legend_list;
    private JDialog legend_dialog;
    private ImageIcon legend_images_selected[];
    private ImageIcon legend_images[];
    private boolean need_color_change;
    private static JFrame dummy_frame = new JFrame(); 
    private static Vector windows = new Vector();
    //JMenuItem zoomIn,zoomOut;
    private JMenuItem display_legend, hide_legend, all_data, no_data;
    private static boolean app_Enabled = false;


    Cursor default_cur = new Cursor(Cursor.DEFAULT_CURSOR),
	on_graph = new Cursor(Cursor.CROSSHAIR_CURSOR);
    
    
    /*
      Constructor
    */
    public StatisticsViewer()
	
    {
	super("Statistics Viewer");
	Container panel = getContentPane();
	JMenuBar menubar = new JMenuBar();
	panel.setLayout(new GridBagLayout());
	Dimension screen_size = Toolkit.getDefaultToolkit().getScreenSize();
	windows.addElement(this);
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e){
		Window w = e.getWindow();
		windows.removeElement(w);
		w.setVisible(false);
		w.dispose();
		if(app_Enabled	&& windows.isEmpty())
		    System.exit(0);
	    }
	});
	setSize(screen_size.width/2,screen_size.height/2);
	need_color_change = false;
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.anchor = GridBagConstraints.CENTER;
	constraints.weightx = 100;
	constraints.weighty = 0;

	menubar.setLayout(new FlowLayout(FlowLayout.LEFT,5,5));
	setJMenuBar(menubar);
	JMenu menu = new JMenu("File");
	menubar.add(menu);
	JMenuItem item = new JMenuItem("Open");
	item.addActionListener(this);
	menu.add(item);
	item = new JMenuItem("Close");
	item.addActionListener(this);
	menu.add(item);
	menu.addSeparator();
	item = new JMenuItem("New Window");
	item.addActionListener(this);
	menu.add(item);	
	menu.addSeparator();
	item = new JMenuItem("Exit");
	item.addActionListener(this);
	menu.add(item);

	
	graph_menu = new JMenu("Graph");
	menubar.add(graph_menu);
	
	menu = new JMenu("Data Sets");
	menubar.add(menu);
	all_data = new JMenuItem("All");
	all_data.addActionListener(this);
	all_data.setEnabled(false);
	menu.add(all_data);
	no_data = new JMenuItem("None");
	no_data.addActionListener(this);
	no_data.setEnabled(false);
	menu.add(no_data);

        menu = new JMenu("Legend");
	menubar.add(menu);
	display_legend = new JMenuItem("Display");
	display_legend.addActionListener(this);
	display_legend.setEnabled(false);
	menu.add(display_legend);
	hide_legend = new JMenuItem("Hide");
	hide_legend.addActionListener(this);
	hide_legend.setEnabled(false);
	menu.add(hide_legend);
		
	menubar.setBorder(new EtchedBorder());

	title = new Stat_Graph_Title();	
	buildConstraints(constraints, 0, 1, 1, 1, 
			 GridBagConstraints.HORIZONTAL);
	panel.add(title,constraints);

	graph = new Stat_Graph();
	constraints.anchor = GridBagConstraints.CENTER;
	constraints.weighty = 100;
	buildConstraints(constraints, 0, 2, 1, 1, 
			 GridBagConstraints.BOTH);
	panel.add(graph,constraints);    

	legend_dialog = new JDialog(dummy_frame,"Legend");
	setLocation(20*(windows.size()-1),20*(windows.size()-1));
	setVisible(true);
	
    }

    public StatisticsViewer(boolean app_enabler)
    {
	this();
	if(app_enabler) {
	    app_Enabled = true;
	}
	else
	    app_Enabled = false;
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
			renderer.setColor
			    (data_aray[selected_index].getColor(i));
			renderer.fillArc(2,2,8,8,0,360);
			legend_images_selected[i] = 
			    new ImageIcon(temp_image); 
			renderer.dispose();
			legend_list.repaint();
		    }
		need_color_change = false;
	    }
	super.paint(g);
    }

    public Dimension getDimension()
    {
	return new Dimension(dimensions);
    }

    public void setTimeListener(StatTimeInterface ti)
    {
	time_listener = ti;
    }
    /*
    public void mousePressed(MouseEvent e)
    {
    }
    public void mouseReleased(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
    
    public void mouseClicked(MouseEvent e)
    {

    if((time_listener != null)*/ /*&& (e.getClickCount()==2)*//*)
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
							      */
    
    public void actionPerformed(ActionEvent e)
    {
	if((e.getSource() == all_data) || 
	   (e.getActionCommand().compareTo("All") == 0))
	    {
		for(int i=0; (i < legend_images.length);i++)
		    {
			data_aray[selected_index].setState(i,true);
		    }
		legend_list.repaint();
		graph.repaint();
	    }
	else if((e.getSource() == no_data) ||
		(e.getActionCommand().compareTo("None") == 0))
	    {
		for(int i=0; (i < legend_images.length);i++)
		    {
			data_aray[selected_index].setState(i,false);
		    }
		legend_list.repaint();
		graph.repaint();
	    }
	else if(e.getActionCommand().compareTo("Open") == 0)
	    {
		initializeData(null);
	    }
	else if(e.getActionCommand().compareTo("Close") == 0)
	    {
		removeCurrentFile();
	    }
	else if(e.getActionCommand().compareTo("New Window") == 0)
	    {
		StatisticsViewer graph_pane = new StatisticsViewer(app_Enabled);
	    }
	else if(e.getActionCommand().compareTo("Exit") == 0)
	    {
		StatisticsViewer.closeAll();
	    }
	else if(e.getSource() == display_legend)
	    {
		legend_dialog.pack();
		legend_dialog.setVisible(true);
	    }
	else if(e.getSource() == hide_legend)
	    {
		legend_dialog.pack();
		legend_dialog.setVisible(false);
	    }
	else if(e.getActionCommand().compareTo("Select") == 0)
	    {
		int selected[] = legend_list.getSelectedIndices();
		for(int i=0; i < selected.length;i++)
		    data_aray[selected_index].setState
			(selected[i],
			 !data_aray[selected_index].isEnabled(selected[i]));
		graph.repaint();
		legend_list.repaint();
	    }
	else if(e.getActionCommand().compareTo("Change Color") == 0)
	    {
		Color color;
		int selected[] = legend_list.getSelectedIndices();
		for(int i=0; i < selected.length;i++)
		    {
			color = Stat_ColorChooser.showDialog
			    (this, "Color Chooser",
			     data_aray[selected_index].getColor(selected[i]));
			data_aray[selected_index].changeColor
			    (selected[i],color);
		    }
		need_color_change = true;
		repaint();
		graph.repaint();
	    }
	/*
	else if(e.getActionCommand().compareTo("On") == 0)
	    {
		graph.setReScale(true);
		graph.repaint();
	    }
	else if(e.getActionCommand().compareTo("Off") == 0)
	    graph.setReScale(false);
	*/
	else if((time_listener != null) &&
		(e.getActionCommand().compareTo("Close") == 0))
	    time_listener.closePreview();
	else {
	    for(int ii=0; ii < graph_list.length; ii++) {
		if(e.getSource() == graph_list[ii]) {
		    if(selected_index != ii) {
			selected_index = ii;
			makeGraph(data_aray[selected_index]);
		    }
		    ii = graph_list.length;
		}
	    }
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
	    if (index == -1) { 
		int selected = list.getSelectedIndex();
		if (selected == -1)
		    return this;
		else
		    index = selected;
	    }

	    setText("  " + data_aray[selected_index].getLabel(index));

	    if(data_aray[selected_index].isEnabled(index))
		setIcon(legend_images_selected[index]);
	    else
		setIcon(legend_images[index]);

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
	    repaint();
	    return this;
	}
    }

    private void removeCurrentFile()
    {
 	title.setTitle("");
	graph.setGraphData(null);
	legend_images = new ImageIcon[0];
	legend_images_selected = new ImageIcon[0];
	all_data.setEnabled(false);
	no_data.setEnabled(false);
	display_legend.setEnabled(false);
	hide_legend.setEnabled(false);
	graph_menu.removeAll();
	
	need_color_change = true;
	repaint();
	
	legend_dialog.setVisible(false);
	legend_dialog.getContentPane().removeAll();
	String value_labels[] = new String[0];
	makeLegendDialog(value_labels);
	graph.repaint();
	title.repaint();
	legend_list.repaint();
	legend_dialog.repaint();
    }

    public void makeGraph(Statistics_Data data)
    {
        String value_labels[];
	title.setTitle(data.getTitle());
	graph.setGraphData(data);
	value_labels = new String[data.getNumXValues()];
	for(int ii = 0; ii < data.getNumXValues(); ii++) {
	    value_labels[ii] = data.getLabel(ii);
	}
	legend_images = new ImageIcon[value_labels.length];
	legend_images_selected = new ImageIcon[value_labels.length];
	all_data.setEnabled(true);
	no_data.setEnabled(true);
	display_legend.setEnabled(true);
	hide_legend.setEnabled(true);
	
	need_color_change = true;
	
	legend_dialog.getContentPane().removeAll();
	makeLegendDialog(value_labels);
	graph.repaint();
	title.repaint();
    
	/*
	JLabel temp_label = new JLabel("Time:",JLabel.CENTER);
	title.add(temp_label);
	track_time = new JTextField("0.0",7);
	track_time.setEnabled(false);
	title.add(track_time);
	buildConstraints(constraints, 0, 1, 3, 1, 
			 GridBagConstraints.HORIZONTAL);
	add(title,constraints);
	*/
    }
	
    private void makeLegendDialog(String value_labels[])
    {
	GridBagConstraints constraints = new GridBagConstraints();
	Dimension dim;
	legend_dialog.getContentPane().setLayout(new GridBagLayout());

        legend_list = new JList(value_labels);
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
	legend_dialog.setLocation(getLocation().x + getSize().width,
				  getLocation().y + getSize().height/2);
    }

    private static void closeAll() {
	while(!windows.isEmpty()) {
	    Window w = (Window)windows.firstElement();
	    w.setVisible(false);
	    windows.removeElement(w);
	    w.dispose();
	}
	if(app_Enabled)
	    System.exit(0);
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
    
    
    private void initializeData(String file_name)
    {
	boolean readable_file = true;
	int option = JFileChooser.APPROVE_OPTION;
	String exts[] = {"tsv"};
	Statistics_Data data;
	JFileChooser file_chooser = 
	    new JFileChooser(System.getProperty("user.dir"));

	file_chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	StatView_FileFilter filter = 
	    new StatView_FileFilter(exts,"tab separated values (*.tsv)");
	file_chooser.addChoosableFileFilter(filter);

	if(file_name == null) {
	    option = file_chooser.showOpenDialog(this);
	    file_name = new String(file_chooser.getSelectedFile().getPath());
	}

	if(option == JFileChooser.APPROVE_OPTION) {
	    Stat_File_Parser filin = null;
	    try {
		filin = 
		    new Stat_File_Parser(file_name);
	    } catch(Exception e) {
		JOptionPane.showMessageDialog
		    (this, "Error: Unable to open selected file.", "Message",
		     JOptionPane.INFORMATION_MESSAGE); 
		readable_file = false;
	    }
	    
	    if(readable_file) {
		Vector temp_data_aray = new Vector();
		try {
		    data = filin.readGraphData();
		    while( data != null ) {
			temp_data_aray.addElement(data);
			data = filin.readGraphData();
		    }
		}
		catch(Exception e) {
		    JOptionPane.showMessageDialog
			(this, "Error reading file.", "Message",
			 JOptionPane.INFORMATION_MESSAGE); 
		    readable_file = false;
		}
		
		if(!temp_data_aray.isEmpty() &&
		   readable_file) {
		    data_aray = new Statistics_Data[temp_data_aray.size()];
		    graph_list = new JMenuItem[data_aray.length];
		    for(int ii = 0; ii < data_aray.length; ii++) {
			data_aray[ii] = 
			    (Statistics_Data)temp_data_aray.elementAt(ii);
		    graph_list[ii] = new JMenuItem(data_aray[ii].getTitle());
		    graph_menu.add(graph_list[ii]);
		    graph_list[ii].addActionListener(this);
		    }
		    selected_index = 0;
		    makeGraph(data_aray[selected_index]);
		}
		else 
		    JOptionPane.showMessageDialog
			(this, "Error reading file.", "Message",
			 JOptionPane.INFORMATION_MESSAGE); 
		    
	    }
	}
    }
    
    public static void main(String args[])
    {
	StatisticsViewer graph_pane = new StatisticsViewer(true);

	if(args.length > 0) 
	    graph_pane.initializeData(args[0]);
    }
}











