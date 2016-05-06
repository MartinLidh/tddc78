#!/usr/local/tcl/bin/wish -f
#
# Upshot - parallel program performance analysis tool
#
# Ed Karrels
# Argonne National Laboratory
# karrels@mcs.anl.gov
#

set defaultFile ~/.upshotrc


proc SetGlobalVars {} {
   global tcl_precision programName progName

   set programName "Nupshot"
   set progName nupshot
   set tcl_precision 17
}


proc SetDefaults {} {
   option add *Entry.Relief sunken
   option add *Scrollbar.Relief sunken
   option add *Menubutton.Relief raised
   if {[option get . font Font] == ""} {
      option add *font -Adobe-Helvetica-Bold-R-Normal--*-120-*
   }
   # wishful thinking
   # option add *Canvas.Font [option get . font Font] 30
}


proc ProcessCmdLineArgs {} {
   global argv bw logFileName

   foreach parameter $argv {
      if {$parameter == "-bw" } {
	 #black and white screen
	 set bw 1
      } elseif {$parameter == "-c" } {
	 #color screen
	 set bw 0
      } else {
	 if [file exists $parameter] {
	    set logFileName $parameter
	 }
      }
   }
}




proc SigDigits {num start end ninterest {factor 1}} {
   # ninterest is the number of interesting digits to leave
   if {!($end-$start)} {
      set ndigits 0
   } else {
      set ndigits [expr int($ninterest-log10($end*$factor-$start*$factor))]
   }
   if {$ndigits<0} {set ndigits 0}
   return [format [format "%%.%df" $ndigits] [expr $num*$factor]]
}



#proc LogFormatError {filename line lineNo} {
#   puts "Logfile format error in line $lineNo of $filename:\n$line\n\n"
#}


proc GuessFormat {filename} {

   if [regexp {.log$} $filename] {
      return alog
   } elseif [regexp {.trf$} $filename] {
      return picl_1
   } else {
      return [GetDefault logfileformat alog]
   }
}


proc RegisterDisplays {} {
   # In this procedure, register any displays that will be used with
   # Upshot.

   Display_Init
   Display_Register Timelines timeline
   Display_Register "Mountain Ranges" mtn
}



# progdir should have been defined by nupshot.c

source $progdir/common.tcl
source $progdir/entries.tcl
source $progdir/fileselect.tcl
source $progdir/zoom.tcl
source $progdir/mainwin.tcl
source $progdir/timelines.tcl
source $progdir/mtn.tcl
source $progdir/legend.tcl
source $progdir/procnums.tcl
source $progdir/hist.tcl
source $progdir/print.tcl
source $progdir/print_mainwin.tcl
source $progdir/print_hist.tcl
source $progdir/copy_canvas.tcl
source $progdir/trim_canvas.tcl
source $progdir/color.tcl
source $progdir/displays.tcl

if 0 {
source optionswin.tcl
source timelinewin.tcl
source lists.tcl
source readaloglog.tcl
source readpicllog.tcl
source gatherevt.tcl
source scales.tcl
source pctdone.tcl
}


SetDefaults
SetGlobalVars
ProcessCmdLineArgs
EntryBindings
RegisterDisplays


   # Creating windows within the main window is always a pain because
   # the main window is just ".", so you can't add windows named ".x",
   # ".y" to it, unlike any other window.  So, how's this for a fix:

frame .f
pack .f -expand 1 -fill both


   # open main window
if [info exists logFileName] {
   NewWin .f 1 $logFileName
} else {
   NewWin .f 1
}

