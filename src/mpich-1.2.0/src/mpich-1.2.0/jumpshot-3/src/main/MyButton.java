import java.awt.Button;
import java.awt.event.ActionListener;
import java.awt.TextField;
import java.awt.Color;
import javax.swing.*;

//This class extends Button to add the ActionListener
public class MyButton extends JButton {
  //Constructors
  public MyButton (String label) {
    super (label);
  }
  
  public MyButton (String label, ActionListener a) {
    super (label);
    this.addActionListener (a);
  }

  public MyButton (String label, String text, ActionListener a) {
    super (label);
    this.addActionListener (a);
    this.setToolTipText (text);
  }
  
  public MyButton (String label, String text, Icon icon, ActionListener a) {
    super (label, icon);
    this.addActionListener (a);
    this.setToolTipText (text);
  }
}
