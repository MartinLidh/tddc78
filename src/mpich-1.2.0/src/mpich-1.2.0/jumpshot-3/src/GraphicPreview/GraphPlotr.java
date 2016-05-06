import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class GraphPlotr extends JFrame
implements TimeInterface				
{
    static String label_const[] = {"abhi","whatever","loony","yahhooo","yoda"};
    Graph_Pane graph_pane;
    public GraphPlotr(String label,double values[][],String value_labels[], 
		      double start_time,double end_time)
    {
	super("Graph Plotter");
	getContentPane().setLayout(new BorderLayout(0,0));
	Dimension screen_size = Toolkit.getDefaultToolkit().getScreenSize();
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e){
		Window w = e.getWindow();
		w.setVisible(false);
		w.dispose();
		System.exit(0);
	    }
	});
	graph_pane = new Graph_Pane(this,label,values,value_labels,
				    start_time,end_time);
	getContentPane().add(graph_pane,BorderLayout.CENTER);
	graph_pane.setTimeListener(this);
	pack();
	getContentPane().setBackground(Color.black);
    }

    public static void main(String args[])
    {
	int i,j;    // for loop variables.
	int size1 =512;/*(int)(Math.random()*100);*/
	int size2 =20/*(int)(Math.random()*20)*/;
	System.out.println(size2);
	String value_labels[] = new String[size2];
	double random_vals[][] = new double[size2][size1];
	for(j=0;j<random_vals.length;j++)
	    for (i=0;i<random_vals[j].length;i++)
		random_vals[j][i]=Math.random()*20;
	for(j=0;j<value_labels.length;j++)
	    value_labels [j] = label_const[j%5];
	GraphPlotr main_frame =
	    new GraphPlotr("Who knows???",random_vals,value_labels,0.0025,20);
       	//main_frame.pack();
	main_frame.setVisible(true);
    }
    public void showTime(double time){}
    public void closePreview()
    {
	setVisible(false);
	dispose();
    }

}
    
        
	
	
