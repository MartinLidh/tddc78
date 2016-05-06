#
# Generic display-handling stuff for Upshot
#
# Ed Karrels
# Argonne National Laboratory
#

# display() structure:
#  static members:
#  _list - list of display types, like {Timelines {Mountain Ranges}}
#  <name> - the command for creating displays of this name.  For example,
#           set {display(Mountain Ranges)} mtn
#
#  nonstatic members:
#  mainwin - name of the main window this is in
#  list - list of current displays, in order top to bottom
#  log - command for the logfile to which these displays are attached
#  timevar - global variable to set to show what time the cursor is over
#  setzoomptcmd - command to which a new zoom point can be appended to
#                 set the point at which all displays will zoom
#  maxheight - maximum height for each display
#  scan - command to which a new left edge time (xleft) will be appended
#         so any display can order the other displays to scroll (scanning)
#  spclist - list of spacer bars that are packed after their corresponding
#            display.  The last display will not have a corresponding spacer.
#
#
# display widget required commands:
#   name - descriptive name for the widget
#          i.e. "Timelines", "Mountain Range"
#

source $progdir/common.tcl
source $progdir/args.tcl


#
# Initialize the display database
#
proc Display_Init {} {
   global display

   set display(_list) {}
}


#
# Add a display type to the display database
#

   # Each display registration must provide:
   #   1. A formal name for the diplay type, i.e. "Timelines".
   #      The formal name should not start with an underscore.
   #   2. A widget creation command.  The creation command must
   #      recognize the following options:
   #
   #         -width <width>
   #            width to request
   #         -maxheight <height>
   #            maximum height to request
   #         -timevar <varname>
   #            global variable to set representing what point in time
   #            the cursor is over
   #         -setzoomptcmd <cmd>
   #            command to execute if a new horizontal zoom point is
   #            selected.  Append the new zoom point to the command,
   #            in seconds.
   #         -scan <cmd>
   #            command to execute if this widget is personally dragged
   #            (change of view, but not by the scrollbar).  It sends
   #            the 'setleft' command to the scrollbars and every display
   #            including this one.
   #
   #      Oh, and the first two arguments to any display widget creation
   #      command will be the window to create and the logfile widget to
   #      attach to.
   #

proc Display_Register {name command} {
   global display

   if ![info exists display(_list)] {
      error "Displays not initialized (call Display_Init)"
   }

   set i [lsearch -exact $display(_list) $name]
   if {$i == -1} {
      lappend display(_list) $name
      set display(_list) [lsort $display(_list)]
      set display($name) $command
   }
}


#
# Get an alphabetized list of the display types in the display database
#
proc Display_Types {} {
   global display

   if ![info exists display(_list)] {
      error "Displays not initialized.  Call Display_Init and\
	    register some display types with Display_Register first."
   }

   return $display(_list)
}


#
# Get the command for creating the given display type
#
proc Display_Cmd {type} {
   global display

   if ![info exists display(_list)] {
      error "Displays not initialized.  Call Display_Init and\
	    register some display types with Display_Register first."
   }

   if {[lsearch -exact $display(_list) $type] == -1} {
      error "$type -- unrecognized display type"
   } else {
      return $display($type)
   }
}


#
# Create a new display widget
#
# win - window to create
# log - command for the logfile that all the displays within this widget
#       will be accessing
proc display {win mainwin log args} {
   global display

   frame $win
   display_config $win $args

   set display($win,mainwin) $mainwin
   set display($win,list) {}
   set display($win,spclist) {}
   set display($win,log) $log

      # rename the command that the frame I usurped created
   rename $win $win.frame_cmd
   proc $win {cmd args} "return \[eval display_cmd $win \$cmd \$args]"

      # the frame will destroy my command, I might as well return the favor
   bind $win <Destroy> "rename $win.frame_cmd {}"
}



#
# grab the command line options and store them
#
proc display_config {win arg_list} {
   global display

   # if the option is not found, ArgOpt will not touch the variable

   set display($win,timevar) ""
   ArgOpt arg_list -timevar display($win,timevar)

   set display($win,setzoomptcmd) ""
   ArgOpt arg_list -setzoomptcmd display($win,setzoomptcmd)

   set display($win,maxheight) ""
   ArgOpt arg_list -maxheight display($win,maxheight)

   set display($win,scan) ""
   ArgOpt arg_list -scan display($win,scan)
}



proc display_cmd {win cmd args} {
   global display

   switch $cmd {
      add {
	 return [eval display_add $win $args]
      }
      list {
	 return $display($win,list)
      }
      config {
	 return [eval $win.frame_cmd config $args]
      }
      default {
	 error "unrecognized command \"$cmd\" sent to display widget"
      }
   }
}



#
# Add a new display to a main window
#
# win - prefix for this display widget
# type - type of display, i.e. "Timelines", "Mountain Range"
# idx - where to insert this display.  $idx<=0 inserts at the top,
#       'end' adds to the end
# opts - a string of options to be sent to the new display
#
proc display_add {win type idx opts} {
   global display

   if ![info exists display($win,list)] {
      error "$win is not a display widget. :-("
   }

   if {$idx != "end" && ![is_int $idx]} {
      error "$idx is not an integer.  I want an integer.  Gimme an integer."
   }

   if {$display($win,timevar) != ""} {
      lappend opts -timevar $display($win,timevar)
   }
   if {$display($win,setzoomptcmd) != ""} {
      lappend opts -setzoomptcmd $display($win,setzoomptcmd)
   }
   if {$display($win,maxheight) != ""} {
      lappend opts -maxheight $display($win,maxheight)
   }
   if {$display($win,scan) != ""} {
      lappend opts -scan $display($win,scan)
   }

   set new_win $win.[GetUniqueWindowID]

      # if this is the first display to be added, it doesn't need
      # a spacer
   if {$display($win,list) == ""} {
      set spc_win ""
   } else {
      set spc_win $win.[GetUniqueWindowID]
         # create a spacer bar
      display_spacer $win $spc_win
   }

      # create the new display
   # puts "eval [Display_Cmd $type] $new_win $display($win,log) $opts"
   eval [Display_Cmd $type] $new_win $display($win,log) $opts

      # give the new display an initial scroll position
   set w $display($win,mainwin)
   global mainwin
   $new_win setview $mainwin($w,left) $mainwin($w,span)
   
   if {$idx < 0} {
      set idx 0
   } elseif {$idx >= [llength $display($win,list)]} {
      set idx end
   }

   if {$display($win,list) == ""} {
         # first display to be added, just stick it in
      pack $new_win -expand 1 -fill both
   } elseif {$idx == "end"} {
         # simply added to the end, packing the spacer first 
      pack $spc_win -fill x
      pack $new_win -expand 1 -fill both
   } else {
         # pack this window and spacer before a certain other display
      set before [lindex $display($win,list) $idx]
      pack $new_win -expand 1 -fill both -before $before
      pack $spc_win -fill x -before $before
   }
      
   # the spclist will always one shorter than 'list'; it is a list
   # of the window names of the spacer after the corresponding
   # window named in 'list'.

   if {$idx == "end"} {
      if {$spc_win != ""} {
	 lappend display($win,spclist) $spc_win
      }
      lappend display($win,list) $new_win
   } else {
      if {$spc_win != ""} {
	 set display($win,spclist) [linsert $display($win,spclist) \
	       $idx $spc_win]
      }
      set display($win,list) [linsert $display($win,list) $idx $new_win]
   }

   return $new_win
}



#
# Move (by changing the packing order) a display
#
proc display_move {win fromidx toidx} {
   global display

   if {[llength $display($win,list)] < 2} {
         # well, geez, there is only one or no displays, trying to move
         # it would be silly
      return
   }
   
   if {$toidx != "end" && ![is_int $toidx]} {
      error "$toidx is not an integer.  I want an integer.  Gimme an integer."
   }

   if {$toidx < 0} {
      set idx 0
   }

   set ndisplays [llength $display($win,list)]
   if {$toidx >= $ndisplays} {
      set toidx end
   }

   if {![is_int $fromidx] || $fromidx < 0 || $fromidx >= $ndisplays} {
      error "invalid 'from' index -- $fromidx"
   }

      # save the window name of the display before which the mobile
      # one will be packed
   if {$toidx != "end"} {
      set todisp [lindex $display($win,list) $toidx]
   }

      # save the window name of the display to be moved
   set fromdisp [lindex $display($win,list) $fromidx]
      # remove it from the main list of displays
   set display($win,list) [lreplace $display($win,list) $fromidx $fromidx]

      # remove the spacer from the mail spacer list
   if {$fromidx == $ndisplays - 1} {
      set spcidx [expr $fromidx - 1]
         # if this is the last display, grab the preceding spacer
      set fromspc [lindex $display($win,spclist) $spcidx]
      set display($win,spclist) [lreplace $display($win,spclist) \
	    $spcidx $spcidx]
   } else {
         # for any other display, grab the following spacer
      set fromspc [lindex $display($win,spclist) $fromidx]
      set display($win,spclist) [lreplace $display($win,spclist) \
	    $fromidx $fromidx]
   }

      # suck out the display and spacer to be removed
   pack forget $fromdisp $fromspc

      # repack the display and spacer where they belong
   if {$toidx == "end"} {
         # tack it onto the end
      pack $fromspc -fill x
      pack $fromdisp -expand 1 -fill both
      lappend display($win,list) $fromdisp
      lappend display($win,spclist) $fromspc

   } else {
      pack $fromdisp -expand 1 -fill both -before $todisp
      pack $fromspc -fill x -before $todisp
      set display($win,list) [linsert $display($win,list) $toidx $fromdisp]
      set display($win,spclist) [linsert $display($win,spclist) \
	    $toidx $fromspc]
   }
}


#
# Remove a display from a main window
#
#  win - win display window
#  idx - index of display to remove
#
proc display_remove {win idx} {
   global display

   set displist $display($win,list)
   if {$idx < 0 || $idx >= [llength $displist]} {
      error "invalid index"
   }

      # get the window name for the display to be removed
   set win_name [lindex $displist $idx]
      # destroy the window
   destroy $win_name

      # close the neighboring spacer if necessary

      # if this is the only display, don't worry about the spacer
   if {[llength $displist] != 1}  {
      if {$idx == [llength $displist]-1} {
            # if this is the last display, remove the preceding spacer
	 set spcidx [expr $idx-1]
      } else {
	    # for any other display, remove the following spacer
	 set spcidx $idx
      }
         # destroy the spacer
      destroy [lindex $display($win,spclist) $spcidx]
         # remove the spacer from the spacer list
      set display($win,spclist) [lreplace $display($win,spclist) \
	    $spcidx $spcidx]
   }

   set display($win,list) [lreplace $displist $idx $idx]
}

      





#
# Create a spacer bar between display widgets.  The cursor will change
# over this bar to an up-arrow/down-arrow, signifying that the user can
# drag the space bar to stretch and shrink the display widgets
#
proc display_spacer {win spc_win} {
   frame $spc_win -borderwidth 2 -relief raised -height 4 \
	 -cursor double_arrow
}
   


#
# Bring up display control dialog.  A list of the window names of the
# current displays will be provided.  The dialog will have the option of
# adding or removing displays, configuring the displays directly, and
# repacking them in a different order.
#
# display_dialog structure:
#   prefix: dialog toplevel window name
#
#   <n> - window name of display #<n>
#   ndisplays - # of displays represented
#   drag - name of window being dragged
#   selected - # of the selected display
#
proc Display_Dialog {win} {
   global display display_dialog

   set dlg .[GetUniqueWindowID]
   toplevel $dlg
   wm title $dlg "Display Configuration"

   frame $dlg.list -relief ridge -borderwidth 3
   pack $dlg.list

   set i 0
      # go through the list of displays being shown
   foreach disp $display($win,list) {
      set name [$disp name]

      Display_Dialog_AddButton $win $dlg $disp $name end
      # puts "disp = $name"
      set display_dialog($dlg,$i) $disp
      incr i
   }
   if {$i > 0} {
      set display_dialog($dlg,selected) 0
      $dlg.list.0 config -relief ridge
   }
   set display_dialog($dlg,ndisplays) $i

   set display_dialog($dlg,drag) ""
   label $dlg.help -text "Drag with button 2 to reorder displays."
   pack $dlg.help

   frame $dlg.controls
   set menu $dlg.controls.add.menu
   menubutton $dlg.controls.add -text "Add" -menu $menu
   menu $menu
   foreach type $display(_list) {
      $menu add command -label $type \
	    -command "Display_Dialog_Add $win $dlg {$type}"
   }
   # button $dlg.controls.config -text "Config" -command \
	 "Display_Dialog_Config $dlg"
   button $dlg.controls.remove -text "Remove" -command \
	 "Display_Dialog_Remove $win $dlg"
   # pack $dlg.controls.add $dlg.controls.config $dlg.controls.remove \
	 -side left -fill x -expand 1 -padx 5 -pady 5
   pack $dlg.controls.add $dlg.controls.remove \
	 -side left -fill x -expand 1 -padx 5 -pady 5
   pack $dlg.controls -fill x -expand 1

   button $dlg.ok -text "OK" -command "destroy $dlg"
   pack $dlg.ok

   # displayprint $win $dlg

      # wait until the window is destroyed
   tkwait window $dlg

if 0 {
   set result {}
      # unpack all the displays
   eval pack forget [pack slaves $win]
      # repack them in the new order
   set ndisplays $display_dialog($dlg,ndisplays)
   for {set i 0} {$i < [expr $ndisplays - 1]} {incr i} {
      pack $display_dialog($dlg,$i) -expand 1 -fill both
      pack [lindex $display($win,spclist) $i] -fill x
      lappend result $display_dialog($dlg,$i)
   }
      # if there is at least one more, pack it (no spacer) on the end
   if $ndisplays {
      pack $display_dialog($dlg,$i) -expand 1 -fill both
   }
}

   # Woah, it works!
}


#
# Select one of the displays in the display dialog
#
proc Display_Dialog_Select {win i} {
   global display_dialog

      # reset the previously selected frame
   $win.list.$display_dialog($win,selected) config -relief flat

      # set the newly selected frame
   $win.list.$i config -relief ridge

   set display_dialog($win,selected) $i
}


#
# Start the drag of one of the display names, so they can be reordered
#
# Dragging uses a few new members of the display_dialog structure:
#   drag_offx, drag_offy - offset (from the center of the name being
#                          dragged) where the mouse button was clicked
#   drag - name of the toplevel window being dragged
#
proc Display_StartDrag {win i x y X Y} {
   global display_dialog
   set disp_win $display_dialog($win,$i)
   set disp_btn $win.list.$i.b

      # set drag_offx and drag_offy as the offsets from the center of
      # the name being dragged
   set display_dialog($win,drag_offx) \
	 [expr $x - [winfo width $disp_btn] / 2 + 1]
   set display_dialog($win,drag_offy) \
	 [expr $y - [winfo height $disp_btn] / 2 + 1]

   # sometimes you need it, sometimes you don't!!
   #set X [expr $X+[winfo vrootx .] - $x - 3]
   #set Y [expr $Y+[winfo vrooty .] - $y - 3]

   set X [expr $X - $x - 3]
   set Y [expr $Y - $y - 3]

      # make floating window
   set display_dialog($win,drag) $win.drag
   toplevel $win.drag

   frame $win.drag.f -width [expr [winfo width $disp_btn]+4] \
	 -height [expr [winfo height $disp_btn]+4] -borderwidth 2 \
	 -relief raised
   label $win.drag.f.l -text [$disp_win name]
   pack $win.drag.f
   place $win.drag.f.l -x [expr [winfo width $disp_btn]/2] \
	 -y [expr [winfo height $disp_btn]/2] -anchor center
   wm overrideredirect $win.drag 1
   wm geometry $win.drag +$X+$Y
}


#
# Do the actual 'drag' of the display name
#
proc Display_DoDrag {win i x y X Y} {
   global display_dialog

   if {$display_dialog($win,drag) == ""} return

   # check if I'm over one of the other guys
   set n $display_dialog($win,ndisplays)
   for {set i 0} {$i<$n} {incr i} {
      if [in_window $win.list.$i $X $Y] {
	 if {$i != $display_dialog($win,selected)} {
	    Display_Dialog_Select $win $i
	 }
      }
   }

   set drag_win $display_dialog($win,drag)
   set X [expr $X+[winfo vrootx .] - [winfo width $drag_win]/2 - \
	 $display_dialog($win,drag_offx)]
   set Y [expr $Y+[winfo vrooty .] - [winfo height $drag_win]/2 - \
	 $display_dialog($win,drag_offy)]

   wm geometry $win.drag +$X+$Y
}


#
# End of the drag
#
proc Display_EndDrag {win dlg i x y X Y} {
   global display_dialog

   if {$display_dialog($dlg,drag) == ""} return

   destroy $dlg.drag

      # if the display selected has been dragged to a different slot,
      # repack my list of windows
   set j $display_dialog($dlg,selected)
   if {$i == $j} return

   # i is the previous slot, j is the new slot


   if {$i < $j} {
      # moving down the list

      set temp $display_dialog($dlg,$i)
      for {set n $i} {$n < $j} {incr n} {
	 set display_dialog($dlg,$n) \
	       $display_dialog($dlg,[expr $n+1])
      }
      set start $i
      set end $j

      display_move $win $i [expr $j+1]
   } else {
      # moving up the list

      set temp $display_dialog($dlg,$i)
      for {set n $i} {$n > $j} {incr n -1} {
	 set display_dialog($dlg,$n) \
	       $display_dialog($dlg,[expr $n-1])
      }
      set start $j
      set end $i

      display_move $win $i $j
   }
   set display_dialog($dlg,$j) $temp

   # rename the buttons to match their new masters

   for {set n $start} {$n <= $end} {incr n} {
      $dlg.list.$n.b config -text [$display_dialog($dlg,$n) name]
   }

   set display_dialog($dlg,drag) ""

   # displayprint $win $dlg
}



#
# Add a button representing a display to the dialog window
#
# disp - name of window this button is representing
# name - name to put on the button
# idx - index where to insert the button.  "end" is recognized.
proc Display_Dialog_AddButton {win dlg disp name idx} {
   global display_dialog

   # puts "AddButton $win $dlg (win)$disp (name)$name $idx"

   upvar #0 display_dialog($dlg,ndisplays) ndisplays

   if ![info exists ndisplays] {
      set ndisplays 0
   }
   
   if {$idx == "end"} {
      set idx $ndisplays
   }
   if {$idx > $ndisplays || $idx < 0} {
      error "out of range index -- $idx"
   }

      # create the button for this display
   frame $dlg.list.$ndisplays -relief flat -borderwidth 4
   set button $dlg.list.$ndisplays.b
   button $button -command "Display_Dialog_Select $dlg $ndisplays"
   bind $button <2> \
	 "Display_StartDrag $dlg $ndisplays %x %y %X %Y"
   bind $button <B2-Motion> \
	 "Display_DoDrag $dlg $ndisplays %x %y %X %Y"
   bind $button <B2-ButtonRelease> \
	 "Display_EndDrag $win $dlg $ndisplays %x %y %X %Y"
   pack $button -fill x -padx 2
   pack $dlg.list.$ndisplays -fill x -padx 2 -pady 2

      # insert the window name for this display in the dialog list
   if {$idx == "end"} {
      set display_dialog($dlg,$ndisplays) $disp
   } else {
      for {set i $ndisplays} {$i > $idx} {incr i -1} {
	 set new_win $display_dialog($dlg,[expr $i-1])
	 set display_dialog($dlg,$i) $new_win
	 $dlg.list.$i.b config -text [$new_win name]
      }
      set display_dialog($dlg,$idx) $disp
      $dlg.list.$idx.b config -text [$disp name]
   }

      # increment the # of displays in the dialog
   incr ndisplays
}


#
# Remove a button representing a display from the dialog window
#
#  dlg - name of dialog window
#  idx - index of button to remove
proc Display_Dialog_RemoveButton {dlg idx} {
   global display_dialog

   upvar #0 display_dialog($dlg,ndisplays) ndisplays

   if {$idx < 0 || $idx >= $ndisplays} {
      error "invalid index"
   }
   
   for {set i $idx} {$i < [expr $ndisplays-1]} {incr i} {
      set new_win $display_dialog($dlg,[expr $i+1])
         # shift the names of the displays in the dialog's list
      set display_dialog($dlg,$i) $new_win
         # shift the names of the buttons
      $dlg.list.$i.b config -text [$new_win name]
   }
      # destroy the last button's frame, which will destroy the button too
   destroy $dlg.list.$i
      # clear the array entry, like it would make a difference
   unset display_dialog($dlg,$i)

   incr ndisplays -1   
}



if 0 {
proc displayprint {win dlg} {
   global display display_dialog

   puts "from the dialog:"
   for {set i 0} {$i < $display_dialog($dlg,ndisplays)} {incr i} {
      puts "  $display_dialog($dlg,$i)"
   }
   puts "from the window:"
   foreach disp $display($win,list) {
      puts "  $disp"
   }
}  
}


#
# Use the display dialog 'add' button to add displays.
#  win - display window
#  dlg - display dialog window
#  type - type of display to add
#
proc Display_Dialog_Add {win dlg type} {
   global display display_dialog

   set insert_idx $display_dialog($dlg,selected)
   set new_disp [display_add $win $type $insert_idx {}]
   
   Display_Dialog_AddButton $win $dlg $new_disp $type $insert_idx

   # displayprint $win $dlg
}


#
# Use the display dialog 'remove' button to remove displays.
#  win - display window
#  dlg - display dialog window
#
proc Display_Dialog_Remove {win dlg} {
   global display display_dialog

   set idx $display_dialog($dlg,selected)
   display_remove $win $idx
   Display_Dialog_RemoveButton $dlg $idx

   # displayprint $win $dlg
}
