import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;	      //Old package name
import com.sun.java.swing.border.*; //Old package name
//import javax.swing.*;	      //New package name
//import javax.swing.border.*;  //New package name

class Example1 extends JPanel {
    JProgressBar progressBar = new JProgressBar();
    static int NUMLOOPS = 100;
    JLabel statusField = new JLabel("Click Start to begin", JLabel.CENTER);
    SwingWorker worker;
    JButton startButton;
    JButton interruptButton;
    Border spaceBelow = BorderFactory.createEmptyBorder(0, 0, 5, 0);

    JButton getStartButton() {
        return startButton;
    }

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
            for(int i = 0; i < NUMLOOPS; i++) {
                updateStatus(i);
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


    /**
     * This action listener, called by the "Start" button, effectively 
     * forks the thread that does the work.
     */
    ActionListener startListener = new ActionListener() {
        public void actionPerformed(ActionEvent event) {
            startButton.setEnabled(false);
            interruptButton.setEnabled(true);
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
                    startButton.setEnabled(true);
                    interruptButton.setEnabled(false);
                    statusField.setText(get().toString());
                }
            };
        }
    };


    /**
     * This action listener, called by the "Cancel" button, interrupts
     * the worker thread which is running this.doWork().  Note that
     * the doWork() method handles InterruptedExceptions cleanly.
     */
    ActionListener interruptListener = new ActionListener() {
        public void actionPerformed(ActionEvent event) {
            interruptButton.setEnabled(false);
            worker.interrupt();
            startButton.setEnabled(true);
        }
    };

    /** 
     * And now for a little assembly.  Put together the buttons, progress 
     * bar and status text field.  
     */
    Example1(String name) {
        setBorder(BorderFactory.createTitledBorder(
                      BorderFactory.createLineBorder(Color.black),
                      name));

        progressBar.setMaximum(NUMLOOPS);

        startButton = new JButton("Start");
        startButton.addActionListener(startListener);
        startButton.setEnabled(true);

        interruptButton = new JButton("Cancel");
        interruptButton.addActionListener(interruptListener);
        interruptButton.setEnabled(false);

        JComponent buttonBox = new JPanel();
        buttonBox.add(startButton);
        buttonBox.add(interruptButton);

        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        add(buttonBox);
        add(progressBar);
        add(statusField);
	statusField.setAlignmentX(CENTER_ALIGNMENT);

        buttonBox.setBorder(spaceBelow);
        Border pbBorder = progressBar.getBorder();
        progressBar.setBorder(BorderFactory.createCompoundBorder(
                                        spaceBelow,
                                        pbBorder));
    }

    Example1() {
        this("Example 1");
    }
}
