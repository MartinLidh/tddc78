import com.sun.java.swing.*;
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

/**
 * Dialog showing information about the state
 */
class StateDlg extends JDialog 
implements ActionListener
{
  /**
   * Constructor
   */
  public StateDlg (Frame f, ProcessState ps) {
    super (f, "State Info");
    
    StateInfo info = ps.info;
    
    GridBagConstraints con = new GridBagConstraints ();
    con.anchor = GridBagConstraints.SOUTHWEST;
    
    JButton b = new JButton ();
    b.addActionListener (this);
    b.setLayout (new GridBagLayout ());
    b.setToolTipText ("Click to close");
    b.setCursor (new Cursor (Cursor.HAND_CURSOR));
    
    b.add (new JLabel ("State: " + info.stateDef.description.desc + " from " + 
				       (new Float (info.begT)).toString () + " to " + 
				       (new Float (info.endT)).toString () + " "), 
			   con); 
    
    con.gridy = 1;
    b.add (new JLabel ("Length: " + (new Float (info.lenT)).toString () + 
				       " sec, Process: " + Integer.toString (ps.info.procId)), 
			   con);
   
    
    getContentPane ().setLayout (new BorderLayout ());
    getContentPane ().add (b, BorderLayout.CENTER);
       
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {dispose ();}
    });
    
    pack ();
    setSize (getMinimumSize ()); setResizable (false);
  }
  
  public void actionPerformed (ActionEvent e) {dispose ();}
}

/**
 * Dialog showing information about a message arrow
 */
class MsgDlg extends JDialog 
implements ActionListener {
  
  /**
   * Constructor
   */
  public MsgDlg (Frame f, ArrowInfo info) {
    super (f, "Message Info");
    
    GridBagConstraints con = new GridBagConstraints ();
    con.anchor = GridBagConstraints.SOUTHWEST;
    
    JButton b = new JButton ();
    b.addActionListener (this);
    b.setLayout (new GridBagLayout ());
    b.setToolTipText ("Double click to destroy");
    b.setCursor (new Cursor (Cursor.HAND_CURSOR));
    
    JLabel x = new JLabel ();
    x.setText ("Message: Size " + Integer.toString (info.size) + ", Tag " + 
	       Integer.toString (info.tag) + ", " +
	       (new Float (info.size / (double)(info.endT - info.begT))).toString () + 
	       " (bytes/sec)");
    b.add (x, con);

    JLabel y = new JLabel ();
    y.setText ("from Process " + Integer.toString (info.begProcId) + " at " +
	       (new Float (info.begT)).toString () + " sec, to Process " + 
	       Integer.toString (info.endProcId) + " at " + (new Float (info.endT)).toString () +
	       " sec");
    
    con.gridy = 1;
    b.add (y, con);
    
    getContentPane ().setLayout (new BorderLayout ()); 
    getContentPane ().add (b, BorderLayout.CENTER);
    
    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {dispose ();}
    });
    
    pack ();
    setSize (getMinimumSize ()); setResizable (false);
  }
 
  public void actionPerformed (ActionEvent e) {dispose ();}
}












