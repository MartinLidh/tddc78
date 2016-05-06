import com.sun.java.swing.JPanel;
import com.sun.java.swing.BorderFactory; 
import com.sun.java.swing.border.TitledBorder; 
import com.sun.java.swing.border.Border;

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
