#
# Main window control for Upshot
#
# Ed Karrels
# Argonne National Laboratory
#

#
# mainwin structure:
#
# index format: <id>,<fieldName>
#   id: the window name of the toplevel window
#   fieldName: one of the following:
#
#   log - logfile widget this window is associated with
#   needs_menus - whether this window needs menus to be created yet
#   empty - whether this window is currently empty; without a
#           logfile or display
#   is_root_win - whether this is the main window, which must remain
#                 open through the life of the program.  If it is, the
#                 last option in the <logfile> menu will be 'Exit Nupshot',
#                 as opposed to 'Close frame' for all other toplevel windows.
#   scrollers - list of all the windows that need to be sent
#               horizontal scroll information
#   displays - list of displays currently visible.  For example,
#              {timeline mtn {msgq recv 3}}
#   endTime, startTime - from the logfile
#   totalTime - legth of the logfile: endTime - startTime
#   span - time span of the current visible range
#   left - time displayed at the far left of the current visible range
#      * note *  these are referred to in 
#   totalUnits, windowUnits - saved for scrollbars, these depict the
#      ratio between the time span visible and the total length of the
#      logfile
#   procWidth - width of process names widget


proc NewWin {win is_root_win {logfile {}}} {
      # $logfile will be the default file open

   global mainwin color

      # initialize data for this window

   if [info exists mainwin($win,log)] {
      unset mainwin($win,log)
   }
   set mainwin($win,empty) 1
   set mainwin($win,needs_menus) 1
   set mainwin($win,is_root_win) $is_root_win

   if {$logfile == ""} {
      set title "Nupshot"
   } else {
      set title "Nupshot - $logfile"
   }

   if !$is_root_win {
      toplevel $win
      wm title $win $title
   }

      # create funky colored background frame
   frame $win.menu -relief raised -border 3 -back $color(menubar)

      # create main menus
   AddLogfileMenu $win $win.menu.logfile
   # AddOptionsMenu $win $win.menu.options

      # just to see the background color...
   # frame $win.menu.junk -width 50

   pack $win.menu -fill x
   # pack $win.menu.logfile $win.menu.options -side left
   pack $win.menu.logfile -side left

      # if a logfile was specified, load it
   if {$logfile != ""} {
      OpenFile $win $logfile [GuessFormat $logfile] \
	    [GetDefault displays Timelines]
   }

}


proc AddLogfileMenu {id win} {
   global mainwin
   # id is the index id for any neccessary data
   # win the the window to create

   menubutton $win -text "Logfile" -menu $win.menu

      # menu options
   set logmenu $win.menu
   menu $logmenu
   $logmenu add command -label "New frame" -command \
	 {NewWin .[GetUniqueWindowID] 0}

      # load a new logfile
   $logmenu add command -label "Open..." -command "SelectFile $id"

      # print the current view
   $logmenu add command \
	 -label "Print..." \
	 -state disabled

      # pick one of two exit options.  If this is the main window,
      # the user should know that destroying it will close the app
   if $mainwin($id,is_root_win) {
      $logmenu add command -label "Exit Nupshot" -command ExitUpshot
   } else {
      $logmenu add command -label "Close frame" -command "CloseFile $id"
   }
}


proc EnablePrintMenu {log w} {
   global mainwin

   # be sure to adjust this if the menus get moved
   $w.menu.logfile.menu entryconfigure 2 \
	 -state normal -command "Print_MainWin_Dialog $log $w"
}



proc AddOptionsMenu {id win} {
   # id is the index id for any neccessary data
   # win the the window to create

      # create menu button
   menubutton $win -text "Options" -menu $win.menu

      # add menu items
   menu $win.menu
   $win.menu add command -label "Colors..." -command ColorOptions
   $win.menu add command -label "Print..." -command PrintOptions

}


proc AddZoomMenu {id win} {
   # id is the index id for any neccessary data
   # win the the window to create

      # create menu button
   menubutton $win -text "Zoom" -menu $win.menu

      # add menu items
   menu $win.menu
   set menu $win.menu
   $win.menu add command -label "Zoom in horiz" -command \
	 "Zoom $id $id.displays horiz 2"
   $menu add command -label "Zoom out horiz" -command \
	 "Zoom $id $id.displays horiz .5"
   $menu add command -label "Reset view" -command \
	 "Zoom $id $id.displays reset"

   # These should not be here.  They should be on each widget.
   # $menu add command -label "Zoom in vert" -command \
	 "Zoom $id $id.displays vert 2"
   # $menu add command -label "Zoom out vert" -command \
	 "Zoom $id $id.displays vert .5"
}


proc AddDisplayMenu {id win} {
   # id is the index id in mainwin() for any neccessary data
   # win the the window to create

      # create menu button
   menubutton $win -text "Display" -menu $win.menu

      # add menu items
   menu $win.menu
   set menu $win.menu

   proc bill {name} {
      return Bill
   }
   proc joe {name} {
      return Joe
   }
   proc ed {name} {
      return Ed
   }
   $menu add command -label "Configure" -command \
	 "Display_Dialog $id.displays"
   # $menu add command -label "Add" -command \
	 "Display_Add $id"
   # $menu add command -label "Remove" -command \
	 "Display_Remove $id"
}



proc SelectFile {win} {
   # Call fileselect and select something.  On 'OK' call OpenFile.
   fileselect "SelectedFile $win" "Open Logfile:" $win.selectfile
}

proc SelectedFile {win filename format} {
   OpenFile $win $filename $format [GetDefault displays Timelines]
}


proc OpenFile {w logfile format displays} {
   global mainwin

   # puts "OpenFile $w $logfile $format $displays"

   # w is used to index into mainwin(), as well as the toplevel window name

   # display is a list of display widgets that should be used to display
   # this file.  For example:
   #   { {timeline} {msgq in 3} }
   # would specify that two displays are to be opened:  timeline and
   # msgq (input queue on process 3)

   # left_gap is the width of the gap to leave to leave on the left
   # side for whatever various widgets wish to do with it
   # Better way to do this?
   set left_gap [GetDefault display_left_gap 40]

      # reset zoom point
   if [info exists mainwin($w,zoom_time)] {unset mainwin($w,zoom_time)}

      # if a logfile hasn't already been loaded in this window

   if {$mainwin($w,needs_menus)} {
      AddLogManipulationButtons $w
      set mainwin($w,needs_menus) 0
   }

   if {!$mainwin($w,empty)} {
         # be sure to close the controls and stuff before closing the logfile

         # destroy the displays first, since if the other stuff is destroyed
         # first, the displays will notice they have more space, a
         # <Configure> event will be triggered, and they will waste time
         # recalculating to fill the new space.  Grrr...
      destroy $w.displays
      destroy $w.controls
      destroy $w.legend
      destroy $w.titlelegend
      destroy $w.bottom
      update idletasks
   }

      # close the old logfile
   if {[info exists mainwin($w,log)]} {
      $mainwin($w,log) close
      unset mainwin($w,log)
   }

      # turn on hourglass cursor
   LookBusy $w

      # open the new logfile
   set openStatus [catch "logfile $w.log $logfile $format" err]

   if {$openStatus} {
      # if there was an error, let the user know
      set errwin .[GetUniqueWindowID]
      toplevel $errwin
      wm title $errwin "Error"
      message $errwin.m -aspect 400 -text "Error opening logfile: \
	    $err"
      button $errwin.b -text "Cancel" -command "destroy $errwin"
      pack $errwin.m $errwin.b
      return -1
   }

   set mainwin($w,log) $w.log
   set log $mainwin($w,log)

   set mainwin($w,startTime) [$log starttime]
   set mainwin($w,endTime) [$log endtime]
   set mainwin($w,totalTime) [expr [$log endtime] - [$log starttime]]

      # set initial visible range
   set mainwin($w,left) $mainwin($w,startTime)
   set mainwin($w,span) $mainwin($w,totalTime)

      # set the title of the window to the name of the logfile
   if $mainwin($w,is_root_win) {
      wm title . "nupshot - $logfile"
   } else {
      wm title $w "nupshot - $logfile"
   }

   # set font [option get $w font Font]

   EnablePrintMenu $mainwin($w,log) $w

      # get the requested display widget width and height
   set widget_width [GetDefault "display widget width" 700]
   # set widget_height [GetDefault "display widget height" 300]

   Create_Controls $w.controls $w $mainwin($w,log)
   # frame $w.sep -height 4 -relief raised -borderwidth 2

   # The names of the windows used in this procedure are highly
   # depended on by stuff in print.tcl.  Change those if
   # you change these.

   Legend_Create $w.legend $mainwin($w,log) $widget_width
   set labeltitle [ $log creator ]
   #puts stdout "$labeltitle"
#   frame $w.titlelegend
   canvas $w.titlelegend 
   label $w.titlelegend.label -text "Logfile title:"
#   label $w.titlelegend.val -text "$labeltitle"
   text $w.titlelegend.val -height 1
   $w.titlelegend.val insert 1.0 "$labeltitle"
   
   pack $w.titlelegend.label $w.titlelegend.val -fill x -side left

   frame $w.bottom

      # these names are assumed int SetScrollsLeft and SetScrollsSpan
      # if you change the names here, change them there

      # attach time labels and a scrollbar to the whole mess
   time_lbl $w.bottom.tlbl [$log starttime] [$log endtime]
      # -font $font

   scrollbar $w.bottom.xscroll -command "SetXview $w" -orient horiz
      # create list of windows that are scrollable

   set mainwin($w,scrollers) [list $w.bottom.tlbl $w.bottom.xscroll]
   # set mainwin($w,scrollers) {}

   # Let's hope this doesn't change much
   set scroll_width 19
   set right_gap $scroll_width

   frame $w.bottom.left_gap -width $left_gap
   frame $w.bottom.right_gap -width $right_gap
   pack $w.bottom.left_gap -side left -fill y
   pack $w.bottom.right_gap -side right -fill y
   pack $w.bottom.tlbl $w.bottom.xscroll -fill x

   # might want to throw in a static scale for the scrollbar

   pack $w.controls -fill x
   # pack $w.sep -fill x -padx 15 -pady 4
   pack $w.legend -fill x
   pack $w.titlelegend -fill x
   pack $w.bottom -side bottom -fill x

      # why did I put this here?  Ah, so the reqheight's and stuff
      # will work.
   update idletasks

   set maxheight [expr ([winfo screenheight .] - \
	 [winfo reqheight $w.menu] - \
	 [winfo reqheight $w.controls] - \
	 [winfo reqheight $w.legend] - \
         [winfo reqheight $w.titlelegend] - \
	 [winfo reqheight $w.bottom] - 40) / [llength $displays]]
   # fudge factor of 40 for the window manager's border
   # What to do if this is really small?

   # xscrollcommand - command that everyone should call to update the scrollbar
   # -xscrollcommand "ScrollDisplays $w" 
   #   * removed *

   # timevar - variable that everyone should set to represent the current time
   #           the cursor is hovering over
   # setzoomptcmd - command that anyone can call if the user selects
   #                a new zoom point (in time)
   # maxheight - maximum height for any of the displays
   # scan - command that every should call to update the horizontal 
   #        position of other displays if the display gets scanned


      # Open a display widget
   display $w.displays $w $log \
	 -timevar "ptrtime($w)" \
	 -setzoomptcmd "Zoom_SetTime $w" \
	 -maxheight $maxheight \
	 -scan "SetLeft $w"

   # open up each display and pack 'em into the display widget

   foreach display $displays {
      # have each display send scroll commands to a central
      # point, attach to the same log, and tell a certain
      # 'ptrtime($w)' variable what time the pointer
      # is over

      $w.displays add $display end {}
   }

   pack $w.displays -fill both -expand 1

   if $mainwin($w,is_root_win) {
      set win .
   } else {
      set win $w
   }

   #wm minsize $win [winfo reqwidth $win] [winfo reqheight $win]
   
   wm minsize $win \
	 [maximum [list [winfo reqwidth $w.menu] \
	          [winfo reqwidth $w.controls]]] \
         [expr [winfo reqheight $w.menu] + \
	       [winfo reqheight $w.controls] + \
	       [winfo reqheight $w.legend] + \
	       [winfo reqheight $w.titlelegend] + \
	       [winfo reqheight $w.bottom]]

   set mainwin($w,totalTime) [expr [$log endtime] - [$log starttime]]
   set mainwin($w,span) $mainwin($w,totalTime)
   set mainwin($w,left) [$log starttime]
   set mainwin($w,windowUnits) [expr [winfo reqwidth $w.displays]/10+1]
   set mainwin($w,totalUnits) [expr $mainwin($w,windowUnits) * \
	 $mainwin($w,totalTime) / $mainwin($w,span)]

      # not empty anymore
   set mainwin($w,empty) 0

      # this command sets the horizontal scrollbar, time labels,
      # and the position of each of the displays, in units of
      # seconds
   SetView $w $mainwin($w,left) $mainwin($w,span)

      # go back to normal cursor
   LookBored $w
   update idletasks
}



proc SetXview {win xview} {
   global mainwin

   # The scrollbars have this funny notion that every is speaking in terms
   # of 10000 units.  Change their words around to seconds before anyone
   # think the scrollbars are loony.  Then tell everyone else the new
   # left edge.

   if {$xview < 0} {set xview 0}
   if {$xview > 10000-$mainwin($win,windowUnits)} {
      set xview [expr 10000-$mainwin($win,windowUnits)]
   }
   set left [expr $xview*$mainwin($win,totalTime)/10000.0+ \
	 $mainwin($win,startTime)]
   SetLeft $win $left
}


proc SetLeft {win left} {
   global mainwin
   set mainwin($win,left) $left

   SetScrollsLeft $win $left
   foreach disp [$win.displays list] {
      $disp setleft $left
   }
   $win.bottom.tlbl setview $left $mainwin($win,span)
}


proc SetView {win left span} {
   global mainwin
   set mainwin($win,left) $left
   set mainwin($win,span) $span

   SetScrollsView $win $left $span
   foreach disp [$win.displays list] {
      $disp setview $left $span
   }
   $win.bottom.tlbl setview $left $span
}


proc SetScrollsLeft {win x} {
   global mainwin

   set log $mainwin($win,log)
   set l [expr int(10000.0*($x-$mainwin($win,startTime)) / \
	 ($mainwin($win,totalTime)))]
   set r [expr $l+$mainwin($win,windowUnits)]

   # puts "telling the scrollbars 10000 $mainwin($win,windowUnits) $l $r"

   $win.bottom.xscroll set 10000 $mainwin($win,windowUnits) $l $r
}


proc SetScrollsView {win left span} {
   global mainwin

   # always set totalUnits to 10000
   if 0 {
      set w [expr int([winfo width $win]/10)+1]
      set t [expr int($w*$mainwin($win,totalTime)/$span)]
   }
   
   set mainwin($win,windowUnits) \
	 [expr int(10000.0*$span/$mainwin($win,totalTime))]
   set l [expr int(10000.0*($left-$mainwin($win,startTime)) / \
	 ($mainwin($win,totalTime)))]
   set r [expr $l+$mainwin($win,windowUnits)]
   

   # puts "telling the scrollbars 10000 $mainwin($win,windowUnits) $l $r"

   $win.bottom.xscroll set 10000 $mainwin($win,windowUnits) $l $r
}



#
# send an 'xview' command to each of the display widgets, keeping
# them all in sync
#
# mainwin($id,ndisplays) should be set to the number of displays
# The displays should be the windows $id.display.[0 -> (n-1)]
#
proc XviewAll {id xview} {
   global mainwin

   foreach disp [$id.displays list] {
      # puts "$disp xview $xview"
      $disp xview $xview
   }
}



   # send a 'set' command to all the x-scrolling widgets on this window

proc ScrollDisplays {id t w l r} {
   global mainwin

   foreach scroller $mainwin($id,scrollers) {
      # puts "$scroller set $t $w $l $r"
      $scroller set $t $w $l $r
   }
}


proc OpenDisplay {frame cmd log} {
      # separate out the widget command and the arguments
   set args [lrange $cmd 1 end]
   set widget [lindex $cmd 0]

   # puts "OpenDisplay $frame $cmd $log"

   eval $widget $frame $log $args
}


proc ExitUpshot {} {
   exit
}

proc CloseFile win {
   # Where did these come from?!
   # displays, display_left_gap, display widget width

   destroy $win
}


proc AddLogManipulationButtons {win} {
   AddZoomMenu $win $win.menu.zoom
   AddDisplayMenu $win $win.menu.display
   pack $win.menu.display $win.menu.zoom -side left
}


proc Create_Controls {frame win log} {
   frame $frame

      # display of the time the pointer is currently at
   frame $frame.time
   label $frame.time.lbl -text "Pointer: "
   entry $frame.time.val -relief sunken -textvariable ptrtime($win)
   label $frame.units -text "(in seconds)"

   pack $frame.time.lbl $frame.time.val -side left

      # zoom buttons
   frame $frame.z
   button $frame.z.ih -bitmap zoom_in_horiz  -command \
	 "Zoom $win $win.displays horiz 2"
   button $frame.z.oh -bitmap zoom_out_horiz -command \
	 "Zoom $win $win.displays horiz .5"
   # button $frame.z.iv -bitmap zoom_in_vert   -command \
	 "Zoom $win $win.displays vert 2"
   # button $frame.z.ov -bitmap zoom_out_vert  -command \
	 "Zoom $win $win.displays vert .5"
   # pack $frame.z.ih $frame.z.oh $frame.z.iv $frame.z.ov -side left

   button $frame.z.r -text "Reset view" -command \
	 "Zoom $win $win.displays reset"
   pack $frame.z.ih $frame.z.oh -side left -padx 5
   pack $frame.z.r -side left -padx 5

   pack $frame.time $frame.units $frame.z -side left -padx 5 -pady 3
}
