import java.util.Vector;
import java.awt.Container;
import java.awt.Cursor;
import java.awt.Component;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import java.lang.NoSuchMethodException;

public class ROUTINES {
  public static void QuickSort (Vector source, int lBound, int uBound) {
    int lCount, uCount;
    Info pivot, a, b;
    
    if (lBound >= uBound) return;                  /* trap door for recursion */

    a = (Info)source.elementAt (uBound);
    b = (Info)source.elementAt (lBound);
    
    if (a.lenT < b.lenT) {
      source.setElementAt (a, lBound);
      source.setElementAt (b, uBound);
    }
    
    pivot = (Info)source.elementAt (lBound);  /* comparisons made with this one */
    lCount = lBound;                               /* forward count starts at the pivot */
    uCount = uBound;                               /* backward count starts at end */
  
    while (lCount <= uCount) {
      
      do {                                    /* scan the list from the front + 1 until */
	lCount++;                             /* the first number greater than */
      }                                       /* or equal to pivot is found */
      while (((Info)source.elementAt (lCount)).lenT < pivot.lenT);
      
      do {                                    /* scan the list from the end - 1 until */
	uCount--;                             /* the first number less than */
      }                                       /* or equal to pivot is found */
      while (((Info)source.elementAt (uCount)).lenT > pivot.lenT);
	
      
      if (lCount < uCount) {    
	// counts have not passed each other yet so trade between upper and lower partitions
	a = (Info)source.elementAt (lCount);
	b = (Info)source.elementAt (uCount);
      	source.setElementAt (a, uCount);
	source.setElementAt (b, lCount);
      }
    }                                               /* while lCount <= uCount */
  
    /* now swap the pivot into the correct position */
    source.setElementAt ((Info)source.elementAt (uCount), lBound);
    source.setElementAt (pivot, uCount);

    /* and do the left and right sub-lists */
    if (uCount > lBound) /* otherwise no need to swap & no left list */
      QuickSort (source, lBound, uCount - 1);
    
    if (uCount < uBound) /* otherwise there is no right list */
      QuickSort (source, uCount + 1, uBound);
  }
  
  public static void makeCursor (Component comp, Cursor c) {
    comp.setCursor (c);
    if (comp instanceof Container) {
      Component [] a = ((Container)comp).getComponents();
      for (int i = 0; i < a.length; i++) makeCursor (a [i], c);
    }
  }

  public static Object invokeMethod (Object target, String methodname, Object arg, Class cla) {
    //    System.out.println ("Entering invokeMethod:" + methodname);
    
    Class c, parameters [];
    Method m;
    Object returnVal = null;
    
    c = target.getClass ();
    if (cla == null) parameters = new Class [0];
    else parameters = new Class [] { cla };
    
    //    for (int k = 0; k < parameters.length; k++) System.out.println (parameters [k].getName ());
    
    try {m = c.getMethod (methodname, parameters);}
    catch (NoSuchMethodException e) { 
      System.err.println (e.toString ());
      return returnVal; 
    }
  
    Object [] arguments;
    
    if (arg == null) arguments = new Object [0];
    else arguments = new Object [] { arg };
    try {returnVal = m.invoke (target, arguments); }
    catch (IllegalAccessException e) {System.err.println (e.toString ());}
    catch (InvocationTargetException e) {System.err.println (e.toString ());}
    
//    System.out.println ("Leaving invokeMethod");
    return returnVal;
  }
}
