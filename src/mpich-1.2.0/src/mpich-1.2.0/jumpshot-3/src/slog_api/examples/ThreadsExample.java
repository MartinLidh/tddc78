/**
 * This file contains two examples that demonstrate how to code a pair 
 * of common threaded application idioms: using an interruptible worker 
 * thread to compute a value, blocking a worker thread and prompting
 * a user for input with a modal dialog.
 * 
 * setenv JAVA_HOME /usr/local/java/jdk1.1.5/solaris
 * setenv CLASSPATH /usr/local/java/swing-1.0.1/swing.jar:${JAVA_HOME}/lib/classes.zip
 * ${JAVA_HOME}/bin/javac ThreadsExample.java
 * ${JAVA_HOME}/bin/java -classpath .:${CLASSPATH} ThreadsExample
 */

import java.awt.*;
import java.awt.event.*;

import java.io.*;
import java.util.*;

import com.sun.java.swing.*;
import com.sun.java.swing.event.*;
import com.sun.java.swing.text.*;

import com.sun.java.swing.SwingUtilities;



/**
 * An abstract class that you subclass to perform
 * GUI-related work in a dedicated thread.
 * For instructions on using this class, see 
 * http://java.sun.com/products/jfc/swingdoc/threads.html
 * This version has an extra method called <code>interrupt()</code>,
 * see the Example1 class below for more information.
 */
abstract class SwingWorker 
{
    private Object value;
    private Thread thread;

    /** 
     * Compute the value to be returned by the <code>get</code> method. 
     */
    public abstract Object construct();


    /**
     * Called on the event dispatching thread (not on the worker thread)
     * after the <code>construct</code> method has returned.
     */
    public void finished() {
    }


    /**
     * A new method that interrupts the worker thread.  Call this method
     * to force the worker to abort what it's doing.
     */
    public void interrupt() {
	Thread t = thread;
	if (t != null) {
	    t.interrupt();
	}
	thread = null;
    }


    /**
     * Return the value created by the <code>construct</code> method.  
     */
    public Object get() {
        while (true) {  // keep trying if we're interrupted
            Thread t;
            synchronized (SwingWorker.this) {
                t = thread;
                if (t == null) {
                    return value;
                }
            }
            try {
                t.join();
            }
            catch (InterruptedException e) {
            }
        }
    }

    /**
     * Start a thread that will call the <code>construct</code> method
     * and then exit.
     */
    public SwingWorker() {
        final Runnable doFinished = new Runnable() {
           public void run() { finished(); }
        };

        Runnable doConstruct = new Runnable() { 
            public void run() {
                synchronized(SwingWorker.this) {
                    value = construct();
                    thread = null;
                }
                SwingUtilities.invokeLater(doFinished);
            }
        };

        thread = new Thread(doConstruct);
        thread.start();
    }
}



/**
 * Loads the begin,end bracketed text from a file and creates
 * a tab pane for each one:
 * 
 *   @begin ExampleClassName ExampleTitle
 *   Example HTML documentation
 *   @end
 * 
 * The Example1, Example2 classes are set up this way.
 */
class ExampleContainer extends JTabbedPane
{
    class ExampleInfo {
	String className;
	String title;
	String htmlText;
    }

    static void messageAndExit(String msg) {
	System.err.println("\n" + msg);
	System.exit(1);
    }


    Reader makeHTMLDoc(String text) {
	String header = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\"><html><body>\n";
	String footer = "</body>\n</html>\n";
	return new CharArrayReader((header + text + footer).toCharArray());
    }


    Vector parseExampleFile(File file) {

	/* Load the file into a string.  We expand tabs here to work around
	 * a bug in the HTML viewer.
	 */
	String fileContents = "";
	try {
	    DataInputStream stream = new DataInputStream(new FileInputStream(file));
	    BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
	    StringBuffer buffer = new StringBuffer();
	    int c;
	    while((c = reader.read()) != -1) {
		if (c == '\t') {
		    buffer.append("    ");
		}
		else {
		    buffer.append((char)c);
		}
	    }
	    fileContents = buffer.toString();
	}
	catch (IOException e) {
	    messageAndExit("Couldn't load \"" + file + "\" " + e);
	}

	/* Extract the @begin,@end bracked text, create a ExampleInfo
	 * object for each one.
	 */
	Vector rv = new Vector();
	int i0 = 0;
	while ((i0 = fileContents.indexOf("\n@begin", i0)) != -1) {
	    int i1 = fileContents.indexOf("@end", i0);
	    if (i1 == -1) {
		messageAndExit("Can't find matching @end for " + fileContents.substring(i0));
	    }
	    String s = fileContents.substring(i0 + "\n@begin".length() + 1, i1);
	    StringTokenizer st = new StringTokenizer(s);
	    ExampleInfo info = new ExampleInfo();
	    info.className = st.nextToken(" ");
	    info.title = st.nextToken("\n").trim();
	    info.htmlText = st.nextToken("");
	    rv.addElement(info);
	    i0 = i1;
	}
	return rv;
    }


    ExampleContainer(File file) {
	super();
	Vector examples = parseExampleFile(file);
	for(int i = 0; i < examples.size(); i++) {
	    ExampleInfo exampleInfo = (ExampleInfo)(examples.elementAt(i));

	    /* Create an instance of the example class
	     */
	    Class exampleClass;
	    Component exampleComponent;
	    try {
		exampleClass = Class.forName(exampleInfo.className);
		exampleComponent = (Component)(exampleClass.newInstance());
	    }
	    catch (Exception e) {
		exampleComponent = new JLabel("Can't build example: " + e);
	    }

	    /* Make a JEditorPane that Displays the HTML documentation
	     * for the example.
	     */
	    String header = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\"><html><body>\n";
	    String title = "<h1>" + exampleInfo.title + "</h1>\n";
	    String text = exampleInfo.htmlText + "\n";
	    String footer = "\n</body>\n</html>\n";

	    JEditorPane docView = new JEditorPane();
	    docView.setEditable(false);
	    docView.setContentType("text/html");
            Document doc = docView.getEditorKit().createDefaultDocument();
	    Reader inr = new CharArrayReader((header + title + text + footer).toCharArray());
	    try {
		docView.getEditorKit().read(inr, doc, 0);
		docView.setDocument(doc);
	    }
	    catch (Exception e) {
		messageAndExit("Couldn't parse HTML doc: " + e);
	    }
	    
	    /* Add a tab that contains the example and its documentation
	     */
	    JPanel panel = new JPanel();
	    panel.setLayout(new BorderLayout());
	    panel.add(exampleComponent, BorderLayout.NORTH);
	    panel.add(new JScrollPane(docView), BorderLayout.CENTER);
	    panel.setPreferredSize(new Dimension(640, 480));
	    addTab(exampleInfo.className, panel);
	}
    }
}


/**
@begin Example1 Interrupting a Swing Worker Thread

One limitation of the SwingWorker class, 
defined in a previous issue of the Swing Connection,
<A HREF="http://java.sun.com/products/jfc/swingdoc/threads.html">Threads
Threads and Swing</A>, is that once a worker thread is started it can't
be interrupted.  It just runs until it's finished.

Here's a simple example that demonstrates how one can use a slightly modified version of the
SwingWorker class to support interrupting the worker thread.
In this example we're performing a slow operation 
on a separate thread to avoid locking up the event dispatching thread, and
thefore the entire GUI, while the operation is underway.  In our example
we're just using a loop that sleeps:
<pre>
for(int i = 0; i < progressBar.getMaximum(); i++) {
    updateStatus(i);
    Thread.sleep(500);
}
</pre>
In this case <code>progressBar.getMaximum()</code> returns 100, so the
loops takes nearly a minute to complete.  In a real application you'd
want to take this approach if you were doing something that was
computationally expensive, that might have to load a large number of
classes, or that might block for network or disk IO, or have to wait for
some other resource.  
<p>
To review: the SwingWorker class is a simple utility for computing a value on 
a new thread.  To use it, one creates a subclass of SwingWorker that
overrides the SwingWorker.construct() method to compute the value.  
For example this code fragment just spawns a thread that constructs
a string, and then waits for the thread to finish:
<pre>
SwingWorker worker = new SwingWorker() {
    public Object construct() {
        return "Hello" + " " + "World";
    }
};
System.out.println(worker.get().toString());
</pre>
The <code>SwingWorker.get()</code> method waits for the new thread to 
finish creating the value with the <code>SwingWorker.construct()</code> method. 
<p>
As originally defined the SwingWorker class doesn't provide a way for 
an application to interrupt the new worker thread.  It's rather bad style
for an interactive application to make the user wait while a worker thread
finishes.  If the user wants stop an operation that's in progress,
the worker thread that's busy performing the operation should be interrupted as 
quickly as possible.  To enable interrupting a SwingWorker we'll add one
method:
<pre>
public void interrupt() {
    Thread t = thread;
    if (t != null) {
        t.interrupt();
    }
    thread = null;
}
</pre>
Note: the conditional statement that interrupts the thread isn't synchronized
because we're referring to a local variable <code>t</code>, not the
thread field itself.  To demonstrate the <code>SwingWorker.interrupt()</code> 
method we've created an application that allows one to start a SwingWorker
and then either wait for it to complete or interrupt it.  The SwingWorkers
construct method just calls a method called <code>doWork()</code> which
was introduced earlier:
<pre>
Object doWork() {
    try {
        for(int i = 0; i < progressBar.getMaximum(); i++) {
            updateStatus(i);
            Thread.sleep(500);
        }
    }
    catch (InterruptedException e) {
        updateStatus(0);
        return "Interrupted";  
    }
    return "All Done"; 
}
</pre>
This method does what any time consuming operation should: it
periodically lets the user know that it's making progress.  The
<code>updateStatus()</code> method queues a Runnable for the event
dispatching thread (remember: don't do GUI work on other threads) which
updates a progress bar.  To start the worker thread running, the
<code>Action</code> for the "Start" menu item creates a SwingWorker that
calls <code>doWork()</code>:
<pre>
worker = new SwingWorker() {
    public Object construct() {
        return doWork();
    }
    public void finished() {
        startAction.setEnabled(true);
        abortAction.setEnabled(false);
        statusField.setText(get().toString());
    }
};
</pre>
This statement immediately forks a thread that runs
<code>SwingWorker.construct()</code>.  The <code>finished()</code>
method runs after the <code>construct()<code> method returns.  The
SwingWorker class runs the <code>finished()</code> method on the event
dispatching thread so it's safe to do GUI work, e.g. setting the status
text field.  In this case we're just reenabling the start
menu item and disabling the abort menu item when the worker
thread is finished.
<p>
If you try running the example you'll see that you can abort the
"Start" action at any time and that only one of the "Start" and
"Abort" menu items are enabled at a time so that it's not 
possible to abort when nothings running and similarly one can
only start one operation at a time.  

@end Example1
 */
class Example1 extends JPanel
{
    JProgressBar progressBar = new JProgressBar();
    JLabel statusField = new JLabel("Select 'Start' Action Menu Item to Begin");
    SwingWorker worker;


    /**
     * When the worker needs to update the GUI we do so by queuing
     * a Runnable for the event dispatching thread with 
     * SwingUtilities.invokeLater().  In this case we're just
     * changing the progress bars value.
     */
    void updateStatus(final int i) {
	Runnable doSetProgressBarValue = new Runnable() {
	    public void run() {
		progressBar.setValue(i);
	    }
	};
	SwingUtilities.invokeLater(doSetProgressBarValue);
    }

    
    /**
     * This method represents the application code that we'd like to 
     * run on a separate thread.  It simulates slowly computing 
     * a value, in this case just a string 'All Done'.  It updates the 
     * progress bar every half second to remind the user that
     * we're still busy.
     */
    Object doWork() {
	try {
	    for(int i = 0; i < progressBar.getMaximum(); i++) {
		updateStatus(i);
		Thread.sleep(500);
	    }
	}
	catch (InterruptedException e) {
	    updateStatus(0);
	    return "Interrupted";  // SwingWorker.get() returns this
	}
	return "All Done";         // or SwingWorker.get() returns this
    }


    /**
     * This action, called from the "Start" menu item, effectively 
     * forks the thread that does the work. We use Action.setEnabled()
     * to ensure that only the start item can't be selected until
     * the worker has either finished or been aborted.  Note also
     * that the SwingWorker.finished() method runs on the event dispatching
     * thread, so we don't have to bother with SwingUtilities.invokeLater().
     */
    Action startAction = new AbstractAction("Start") {
	public void actionPerformed(ActionEvent event) {
	    startAction.setEnabled(false);
	    abortAction.setEnabled(true);
	    statusField.setText("Working...");

	    /* Constructing the SwingWorker() causes a new Thread
	     * to be created that will call construct(), and then
	     * finished().  Note that finished() is called even if
	     * the worker is interrupted because we catch the
	     * InterruptedException in doWork().
	     */
	    worker = new SwingWorker() {
		public Object construct() {
		    return doWork();
		}
		public void finished() {
		    startAction.setEnabled(true);
		    abortAction.setEnabled(false);
		    statusField.setText(get().toString());
		}
	    };
	}
    };


    /**
     * This action, called from the "Abort" menu item, interrupts
     * the worker thread which is running this.doWork().  Note that
     * the doWork() method handles InterruptedExceptions cleanly.
     */
    Action abortAction = new AbstractAction("Abort") {
	public void actionPerformed(ActionEvent event) {
	    abortAction.setEnabled(false);
	    worker.interrupt();
	    startAction.setEnabled(true);
	}
    };


    /** 
     * And now for a little assembly.  Put together the menu items, progress 
     * bar and status text field.  
     */
    Example1() {
	super();
	statusField.setFont(new Font("Dialog", Font.BOLD, 14));
	startAction.setEnabled(true);
	abortAction.setEnabled(false);
	JMenuBar menuBar = new JMenuBar();
	JMenu menu = new JMenu("Worker Thread Actions");
	menu.add(startAction);
	menu.add(abortAction);
	menuBar.add(menu);
	setLayout(new BorderLayout());
	add(menuBar, BorderLayout.NORTH);
	add(progressBar, BorderLayout.CENTER);
	add(statusField, BorderLayout.SOUTH);
    }
}


/**
@begin Example2 Prompting the User from a Swing Worker Thread
   
This example is same as Example1 except that after the work method
has run for about two seconds it blocks until the user has 
responded to a YES/NO/CANCEL modal dialog.  If the user doesn't
confirm we short circuit the doWork() loop.  This demonstrates
an idiom common to many worker threads: if the worker runs into
an unexpected condition it may need to bock until it has
alerted the user or collected more information from the user
with a modal dialog.  Doing so is a little complex because the dialog 
needs to be shown on the event dispatching thread, and the
worker thread needs to be blocked until the user has 
dismissed the modal dialog.
<p>
We use <code>SwingUtilities.invokeAndWait()</code> to popup the 
dialog on the event dispatching thread.  Unlike 
<code>SwingUtilities.invokeLater()</code>, this method blocks
until it's Runnable returns.   In this case the Runnable will
not return until the dialog has been dismissed.  We create an 
named inner Runnable class, <code>DoShowDialog</code>, to handle 
popping up the dialog.  There's a field, 
<code>DoShowDialog.proceeedConfirmed</code>, to record the users
reponse:
<pre>
class DoShowDialog implements Runnable {
    boolean proceedConfirmed;
    public void run() {
	int n = JOptionPane.showConfirmDialog(Example2.this, "Proceed?");
	proceedConfirmed = (n != JOptionPane.NO_OPTION);
    }
}
</pre>
Remember that because <code>JOptionPane.showConfirmDialog()</code> 
pops up a modal dialog, the call will block until the user
has dismissed the dialog by selecting one of yes,no,cancel.
<p>
To show the dialog and block the calling thread (that's the worker
thread) until it's been dismissed, we just create an 
instance of <code>DoShowDialog</code> and call 
<code>SwingUtilities.invokeAndWait()</code>:
<pre>
DoShowDialog doShowDialog = new DoShowDialog();
try {
    SwingUtilities.invokeAndWait(doShowDialog);
}
catch (java.lang.reflect.InvocationTargetException e) {
    e.printStackTrace();
}
</pre>
We're catching the <code>InvocationTargetException</code>
here because it should only happen if there's a bug
in <code>DoShowDialog.run()</code>; and there shouldn't be.
Once the <code>invokeAndWait()</code> call has returned
from executing on the event dispatching thread,
we can return the users response, which is conveniently
stored in <code>doShowDialog.proceedConfirmed</code>

@end Example2
*/

class Example2 extends Example1
{
    /**
     * Popup a modal confirm dialog and block until the user responds.
     * Return true unless the user selects "NO".
     */
    boolean waitForUserConfirmation() throws InterruptedException {

	/* We're going to show the modal dialog on the event dispatching
	 * thread with SwingUtilities.invokeLater(), so we create a 
	 * Runnable class that shows the dialog and stores the users
	 * response.
	 */
	class DoShowDialog implements Runnable {
	    boolean proceedConfirmed;
	    public void run() {
		int n = JOptionPane.showConfirmDialog(Example2.this, "Example2: Proceed?");
		proceedConfirmed = (n != JOptionPane.NO_OPTION);
	    }
	}
	DoShowDialog doShowDialog = new DoShowDialog();

	/* The invokeAndWait utility can throw an InterruptedException,
	 * which we don't bother with, and an InvocationException.
	 * The latter occurs if our Runnable throws - which would indicate
	 * a bug in DoShowDialog.  The invokeAndWait() call will not return 
	 * until the user has dismissed the modal confirm dialog.
	 */
	try {
	    SwingUtilities.invokeAndWait(doShowDialog);
	}
	catch (java.lang.reflect.InvocationTargetException e) {
	    e.printStackTrace();
	}
	return doShowDialog.proceedConfirmed;
    }


    /**
     * This is just a copy of Example1.doWork() with one extra wrinkle.
     * After about two seconds we ask the user to confirm with
     * a modal dialog (see waitForUserConfirmation()); if the answer
     * is NO then we short circuit.
     */
    Object doWork() {
	try {
	    for(int i = 0; i < progressBar.getMaximum(); i++) {
		updateStatus(i);
		if (i == 4) {
		    if (!waitForUserConfirmation()) {
			updateStatus(0);
			return "Not Confirmed";
		    }
		}
		Thread.sleep(500);
	    }
	}
	catch (InterruptedException e) {
	    updateStatus(0);
	    return "Interrupted";  // SwingWorker.get() returns this
	}
	return "All Done";         // or SwingWorker.get() returns this
    }
}



public class ThreadsExample {

    /**
     * This is strictly boilerplate: set the look and feel, configure the
     * frame, pack(), show().
     */
    public static void main(String[] args) {
	String laf = UIManager.getCrossPlatformLookAndFeelClassName();
	try {
	    UIManager.setLookAndFeel(laf);
	}
	catch (Exception e) {
	    e.printStackTrace();
	    System.exit(1);
	}
	JFrame f = new JFrame("Threads Examples");
	WindowListener l = new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {System.exit(0);}
	};
	f.addWindowListener(l); 
	Container c = new ExampleContainer(new File("ThreadsExample.java"));
	f.getContentPane().add(c);
	f.pack();
	f.show();
    }

}
