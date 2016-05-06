import java.applet.Applet;

public class jumpshot extends Applet {
  public void init () {
    checkVersion ();
    String param = null, f = getParameter ("logfile");
    if (f != null) param = getParameter ("logfiledir") + "/" + f;
    new Mainwin (this, param, true, false);
  }
  static public void main (String [] args) {
    checkVersion ();
    String param = null;
    try {param = args [0];}
    catch (Exception x) {param = null;}
    new Mainwin (null, param, false, false);
  }
  
  static void checkVersion () {
    String vers = System.getProperty ("java.version");
    if (vers.compareTo ("1.1.6") < 0) {
      System.out.println ("WARNING: Jumpshot must be run with a 1.1.6 or higher version Java VM");
      System.out.println ("Questions: mpi-maint@mcs.anl.gov");
    }
  }
}
