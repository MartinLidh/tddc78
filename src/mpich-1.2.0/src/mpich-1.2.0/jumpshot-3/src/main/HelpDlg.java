import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * This class implements a help dialog. The lines of text to be displayed
 * in this dialog are passed on as a single string, lines seperated by '/n's
 */
class HelpDlg extends JDialog 
implements ActionListener {
  private String text;
  private JTextArea area;
  
  /**
   * Constructor
   */
  public HelpDlg (Frame p, String s) {
    super (p, "Help", false);
    text = s;
    setup ();
  }
  
  private void setup () {
    setupData ();
    setupPanel ();
    
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {setVisible (false);}
    });
  }

  private void setupData () {
    area = new JTextArea (text, 8, 40);
    area.setEditable (false);
    area.setBackground (Color.white);
    area.setForeground (Color.black);
    area.setCursor (new Cursor (Frame.DEFAULT_CURSOR));
  }
  
  private void setupPanel () {
    JScrollPane scroller = new JScrollPane();
    scroller.getViewport().add(area);
    getContentPane ().add (scroller, "Center");
    getContentPane ().add (new MyButton ("Close", this), "South");
    pack ();
  }
  
  public void actionPerformed (ActionEvent evt) {
    String command = evt.getActionCommand ();
    if (command.equals ("Close")) setVisible (false);
  }
}  

class ErrorDiag {
  public ErrorDiag(Component p, String txt) {
    JOptionPane.showMessageDialog(p, txt, "Error", JOptionPane.ERROR_MESSAGE);
  }
}
