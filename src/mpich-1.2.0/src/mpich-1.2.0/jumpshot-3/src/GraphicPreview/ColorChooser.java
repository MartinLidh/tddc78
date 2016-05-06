import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class ColorChooser extends JDialog
    implements ChangeListener, ActionListener
{
    Color selected_color = Color.white;
    boolean selection_flag;
    JSlider scale[] = new JSlider[3];
    JTextField txtfld[] = new JTextField[3];
    JTextField preview_pane;
    public ColorChooser(JFrame frame, String title, Color init_color)
    {
	super(frame,title,true);
	selection_flag = true;
	if(init_color != null)
	    selected_color = new Color(init_color.getRed(),
				       init_color.getGreen(),
				       init_color.getBlue());
	Container panel = getContentPane();
	
       	panel.setLayout(new GridBagLayout());
	GridBagConstraints constraints = new GridBagConstraints();
	constraints.anchor = GridBagConstraints.CENTER;
	constraints.ipadx = 5;
	constraints.ipady = 5;
	
	JLabel label[] = new JLabel[3];
	label[0] = new JLabel("Red Value");
	label[1] = new JLabel("Green Value");
	label[2] = new JLabel("Blue Value");
	
	scale[0] = new JSlider(JSlider.HORIZONTAL,0,255,
				    init_color.getRed());
	scale[1] = new JSlider(JSlider.HORIZONTAL,0,255,
			    init_color.getGreen());
	scale[2] = new JSlider(JSlider.HORIZONTAL,0,255,
			    init_color.getBlue());

	txtfld[0] = new JTextField(init_color.getRed() + "",
					   4);
	txtfld[1] = new JTextField(init_color.getGreen() + "",
					   4);
	txtfld[2] = new JTextField(init_color.getBlue() + "",
					   4);


	int ii;
	for(ii = 0; ii<scale.length; ii++)
	    {
		buildConstraints(constraints, 0, ii, 1, 1, 
				 GridBagConstraints.HORIZONTAL);
		panel.add(label[ii],constraints);
		buildConstraints(constraints, 1, ii, 1, 1, 
				 GridBagConstraints.HORIZONTAL);
		panel.add(scale[ii],constraints);
		scale[ii].addChangeListener(this);

		buildConstraints(constraints, 2, ii, 1, 1, 
				 GridBagConstraints.HORIZONTAL);
		panel.add(txtfld[ii],constraints);
		txtfld[ii].setEnabled(false);
	    }

	preview_pane = 
	    new JTextField(10);
	preview_pane.setEnabled(false);
	preview_pane.setBackground(init_color);
	buildConstraints(constraints, 0, 3, 3, 1,
			 GridBagConstraints.NONE);
	panel.add(preview_pane,constraints);

	JPanel temp_panel = new JPanel(new FlowLayout(FlowLayout.CENTER));
	JButton button = new JButton("Select");
	button.addActionListener(this);
	temp_panel.add(button);

	button = new JButton("Cancel");
	button.addActionListener(this);
	temp_panel.add(button);

	button = new JButton("Reset");
	button.addActionListener(this);
	temp_panel.add(button);
	buildConstraints(constraints, 0, 4, 3, 1,
			 GridBagConstraints.NONE);
	panel.add(temp_panel,constraints);
	pack();
    }
    
    public void buildConstraints(GridBagConstraints constraints,
				 int gridx, int gridy,
				 int gridwidth, int gridheight,
				 int fill)
    {
	constraints.gridx = gridx;
	constraints.gridy = gridy;
	constraints.gridwidth = gridwidth;
	constraints.gridheight = gridheight;
	constraints.fill = fill;
    }
    /*
    public static void main(String args[])
    {
	JFrame frame = new JFrame("Testing");
	Color c = ColorChooser.showDialog(Color.red,frame);
	System.out.println(c.getRed()+","+c.getGreen()+","+c.getBlue());
	//c.setVisible(true);
    }
    */
    private void setComponentSize(JComponent comp, Dimension dim)
    {
	comp.setPreferredSize(dim);
	comp.setMaximumSize(dim);
	comp.setMinimumSize(dim);
    }
	
	
    public void stateChanged(ChangeEvent e)
    {
	JSlider slider = (JSlider)e.getSource();
	Color colr = preview_pane.getBackground();
	int colr_values[] = {colr.getRed(),colr.getGreen(),colr.getBlue()};
	int ii;
	for(ii = 0; ii<scale.length; ii++)
	    if(slider == scale[ii])
		{
		    colr_values[ii] = slider.getValue();
		    txtfld[ii].setText(slider.getValue()+"");
		    preview_pane.setBackground(new Color(colr_values[0],
							  colr_values[1],
							  colr_values[2]));
		    preview_pane.repaint();
		    break;
		}
	
	
    }
    public void actionPerformed(ActionEvent e)
    {
	Color colr;
	if(e.getActionCommand().equals("Select") == true)
	    {
		colr = preview_pane.getBackground();
		selected_color = new Color(colr.getRed(),
					   colr.getGreen(),
					   colr.getBlue());
		setVisible(false);
		selection_flag = false;
	    }
	else if(e.getActionCommand().equals("Cancel") == true)
	    {
		setVisible(false);
		selection_flag = false;
	    }
	else if(e.getActionCommand().equals("Reset") == true)
	    {
		colr = selected_color;
		int colr_values[] = {colr.getRed(),colr.getGreen(),
				     colr.getBlue()};
		int ii;
		for(ii = 0; ii<colr_values.length;ii++)
		    {
			scale[ii].setValue(colr_values[ii]);
			txtfld[ii].setText(colr_values[ii]+"");
			scale[ii].repaint();
		    }
		preview_pane.setBackground(selected_color);
		preview_pane.repaint();
	    }
    }
    
    public static Color showDialog(JFrame frame, String title, Color colr)
    {
	ColorChooser dialog = new ColorChooser(frame,title,colr);
	dialog.addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e){
		ColorChooser d = (ColorChooser)e.getSource();
		d.setVisible(false);
		d.selection_flag = false;
	    }
	});
	dialog.setVisible(true);
	return dialog.selected_color;
    }
}




