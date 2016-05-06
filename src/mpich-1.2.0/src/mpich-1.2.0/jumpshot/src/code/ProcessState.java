/**
 * This class specifies each state
 */
public class ProcessState {
  StateInfo info;            //object storing all relevent info. on a state
  JProcess parent;           //JProcess that this state belongs to
  int y;                     //y coordinate of this state instance's rectangle
  int h;                     //height of this state instance's rectangle
    
  public ProcessState (StateInfo evt, JProcess p) { 
    info = evt;
    parent = p;
  }
} 


