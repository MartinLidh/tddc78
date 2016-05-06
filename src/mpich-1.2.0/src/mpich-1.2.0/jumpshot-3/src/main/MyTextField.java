import java.awt.Color;
import javax.swing.JTextField;
import javax.swing.JLabel;

//This class extends TextField to put background and foreground
//colors to the TextField and make it editable according to the options.
public class MyTextField extends JTextField {
  public MyTextField (boolean b) {super (); setup (b);}
  public MyTextField (String text, boolean b) {super (text); setup (b);}
  public MyTextField (int columns, boolean b) {super (columns); setup (b);}
  public MyTextField (String text, int columns, boolean b) {
    super (text, columns);
    setup (b);
  }

  void setup (boolean b) {
    this.setBackground (Color.white);
    this.setForeground (Color.black);
    this.setEditable (b);
  }
}

class MyJLabel extends JLabel {
  public MyJLabel (String s) {super (s);}
  
  public MyJLabel (String s, int columns) {
    super ();
    StringBuffer t = new StringBuffer (s);
    int len = t.length ();
    
    if (len < columns)
      for (int i = 0; i < (columns - len); i++) t.append (' ');
    else if (len > columns) 
      t.setLength (columns);
    
    setText (t.toString ());
  }
  
  public MyJLabel (int columns) {
    super ();
    StringBuffer t = new StringBuffer ();
    
    for (int i = 0; i < columns; i++) t.append (' ');
    setText (t.toString ());
  }
}
