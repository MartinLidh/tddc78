import java.awt.*;
import java.util.*;

public class Statistics_Data
{
    private Vector dataVec;
    private String y_labels[],
	           title;
    private final Color     pre_defn_colors[] 
	= { Color.blue, Color.green, Color.orange, Color.darkGray,
	    Color.gray, Color.cyan, Color.magenta, Color.yellow, Color.pink,
	    Color.black };

    public Statistics_Data(String ttle, double data[][], String labels_on_y[],
			   String x_labels[])
    {
	int i, color_counter;
	dataVec = new Vector();    
	y_labels = new String[labels_on_y.length];
	title = "";
	Graph_data.resetUsedColors();
	if(data.length != 0){
	    if((data[0].length != 0) && (y_labels.length == data[0].length)) {
		for(i=0, color_counter=0;i<data.length;i++)
		    {
			if(color_counter < pre_defn_colors.length)
			    dataVec.addElement
				(new Graph_data
				 (x_labels[i],data[i],true,
				  pre_defn_colors[color_counter++]));
			else {
			    dataVec.addElement(new Graph_data
					       (x_labels[i],data[i]));
			}
		    }
		for(i=0;i<labels_on_y.length;i++)
		    y_labels[i] = new String(labels_on_y[i]);
		title = new String(ttle);
	}
	}
    }

    public Statistics_Data()
    {}

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
    public String getYLabel(int i)
    {
	return (new String(y_labels[i]));
    }
    
    public String getTitle()
    {
	return (new String(title));
    }
    public int getNumXValues()
    {
	return dataVec.size();
    }
    public double getElementAt(int i, int j)
    {
	return ((Graph_data)dataVec.elementAt(i)).getElementAt(j);
    }
    public int getNumYValues()
    {
	if(dataVec.size() > 0)
	    return ((Graph_data)dataVec.elementAt(0)).getLength();
	else
	    return 0;
    }
}
		
	
