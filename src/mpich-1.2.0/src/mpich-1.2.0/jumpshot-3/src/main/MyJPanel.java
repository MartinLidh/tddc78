import javax.swing.JPanel;
import javax.swing.BorderFactory; 
import javax.swing.border.TitledBorder; 
import javax.swing.border.Border;

public class MyJPanel extends JPanel {
  //Constructors
  public MyJPanel (java.awt.LayoutManager layout, String title) {
    super (layout); 
    setBorder (BorderFactory.createTitledBorder (title));
    TitledBorder b = (TitledBorder)getBorder ();
    b.setTitlePosition (TitledBorder.TOP); 
    b.setTitleJustification (TitledBorder.LEFT);
  }
  
  public MyJPanel (String title) {
    super (); 
    setBorder (BorderFactory.createTitledBorder (title));
    TitledBorder b = (TitledBorder)getBorder ();
    b.setTitlePosition (TitledBorder.TOP); 
    b.setTitleJustification (TitledBorder.LEFT);
  }
}
