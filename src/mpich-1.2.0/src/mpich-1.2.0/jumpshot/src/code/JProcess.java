import java.util.Vector;

/**
 * This class specifies each Process. 
 */
public class JProcess
{
  Vector procStateVector;    //this vector stores all states belonging to this process in 
                             //ascending order of their end timestamps
  int procId;                //procId of this particular JProcess
  boolean dispStatus;        //tells whether the process should be drawn or not
  double begT;               //starting time for the first state of this process
  int y;                     //y coordinate of this process's line
  
  public JProcess (int pid) { 
    procId = pid;
    dispStatus = true;
    procStateVector = new Vector ();
  }
}
