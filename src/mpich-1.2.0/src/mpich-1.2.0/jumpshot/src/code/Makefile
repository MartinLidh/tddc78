# Generated automatically from Makefile.in by configure.

SHELL = /bin/sh

JC         = /usr/java/bin/javac
JAR        = /usr/java/bin/jar
JAVA_LIBS  = /usr/java/lib/rt.jar:/usr/java/lib/classes.zip:/usr/java/classes
SWING_LIBS = /home/TDDB78/src/mpich-1.2.0/jumpshot/swing/swing.jar

GUI_HOME   = ../..
GUI_LIBDIR = $(GUI_HOME)/lib

MAIN_LIB = js_classes.jar

SRC = ApltFileDlg.java CanOptions.java ClogDisplay.java \
      ClogReader.java ColorUtil.java ColoredRect.java Dlgs.java \
      HistCanvas.java HistImage.java Histwin.java JProcess.java \
      Mainwin.java MyButton.java MyImage.java MyJPanel.java \
      MyTextField.java PHistCanvas.java PVertScaleCanvas.java \
      PrintCanvas.java PrintDlg.java ProcDlg.java ProcessState.java \
      ProgramCanvas.java ROUTINES.java RecordHandler.java \
      StateButtons.java Structs.java SwingWorker.java VertScaleCanvas.java \
      jumpshot.java

OBJ = $(SRC:.java=.class)

.SUFFIXES:	.java .class

.java.class:
	$(JC) $(JFLAGS) $<

CLASSPATH = $(JAVA_LIBS):$(SWING_LIBS):.

JFLAGS = -classpath $(CLASSPATH) 

ALL:	CLASSFILES $(MAIN_LIB)

CLASSFILES: $(SRC)
	$(JC) $(JFLAGS) $?

$(MAIN_LIB): $(OBJ)
	$(JAR) cvf $@ *.class
	mv -f $@ $(GUI_LIBDIR)

clean:
	rm -f $(OBJ) $(MAIN_LIB) *.class core

rmlib:
	rm -f $(GUI_LIBDIR)/$(MAIN_LIB)

distclean: clean rmlib
	rm -f *.java.* 
