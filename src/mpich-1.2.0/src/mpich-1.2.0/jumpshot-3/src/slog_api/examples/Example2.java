import com.sun.java.swing.*;  //old package name
//import javax.swing.*;  

class Example2 extends Example1 {

    Example2() {
        super("Example 2");
    }

    /**
     * Popup a modal confirm dialog and block until the user responds.
     * Return true unless the user selects "NO".
     */
    boolean waitForUserConfirmation() throws InterruptedException {

	/* We're going to show the modal dialog on the event dispatching
	 * thread with SwingUtilities.invokeLater(), so we create a 
	 * Runnable class that shows the dialog and stores the user's
	 * response.
	 */
	class DoShowDialog implements Runnable {
	    boolean proceedConfirmed;
	    public void run() {
		Object[] options = {"Continue", "Cancel"};
	        int n = JOptionPane.showOptionDialog(Example2.this, 
					    "Example2: Continue?",
					    "Example2",
					    JOptionPane.YES_NO_OPTION,
					    JOptionPane.QUESTION_MESSAGE,
					    null,
					    options,
					    "Continue");
	        proceedConfirmed = (n == JOptionPane.YES_OPTION);
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
	    for(int i = 0; i < NUMLOOPS; i++) {
		updateStatus(i);
		if (i == 4) {
		    if (!waitForUserConfirmation()) {
			updateStatus(0);
			return "Not Confirmed";
		    }
		}
		if (Thread.interrupted()) {
		    throw new InterruptedException();
		}
		Thread.sleep(500);
	    }
	}
	catch (InterruptedException e) {
	    updateStatus(0);
	    return "Interrupted";  // SwingWorker.get() returns this
	}
	return "All Done";         // or this
    }
}
