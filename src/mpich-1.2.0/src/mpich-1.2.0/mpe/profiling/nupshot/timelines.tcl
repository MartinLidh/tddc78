#
# Timeline widget for Upshot
#



if ![info exists package(timelines)] {
set package(timelines) yup
if ![info exists progdir] {set progdir .}

source $progdir/args.tcl
source $progdir/common.tcl
source $progdir/procnums.tcl

proc timeline {win log args} {
   global timeline bw

   frame $win
   set canvas $win.c

   proc $win {cmd args} "
      eval Timeline_Cmd $win \$cmd \$args
   "

   set timeline($win,log) $log
   set timeline($win,isdragto) 0

   # puts stderr "Win: $win, [array names timeline]"

   set timeline($win,c_cmd) $win.c_cmd

      # C routines need a command via which we can reach it, the logfile
      # command, the canvas name, the name of the Tcl array to which
      # all the data should be linked, and the prefix into that array
   set timeline($win,tl) [Timeline_C_Init $win.c_cmd $log $win.c \
	 timeline $win]

   # The C stuff expects me to set visWidth, visHeight, xleft, xspan,
   # ytop, yspan, bg, outlineColor, msgColor, and bw

   Timeline_Config $win $args

   canvas $canvas \
	 -width $timeline($win,visWidth) \
	 -height $timeline($win,visHeight) \
	 -bg $timeline($win,bg) \
	 -yscrollcommand "Timeline_YSet $win" \
	 -scrollincrement 1

   # -scrollregion [list 0 0 [expr $timeline($win,width)-1] \
	                [expr $timeline($win,height) - 1] ]

   set timeline($win,ytop) 0
   set timeline($win,yspan) [$log np]

   Procnums_Create $win.pn $log
   scrollbar $win.sc -orient v -command "$win yview"

      # draw everything (pretty quick now--2.6 sec for ~35000 events on ptera)
   $timeline($win,c_cmd) draw
   
      # be sure to pack the canvas last so it sucks up all the space
   pack $win.pn -side left -fill y
   pack $win.sc -side right -fill y
   pack $canvas -expand 1 -fill both

   bind $canvas <Configure> "Timeline_Resize $win %w %h"

   # bindings for 2nd button quick dragging
   bind $canvas <ButtonPress-2> "Timeline_Mark $win %x %y"
   bind $canvas <Button2-Motion> "Timeline_Dragto $win %x %y"

   # click on a state to get info on it
   bind $canvas <1> "Timeline_ClickState $win $log %x %y %X %Y"
   bind $canvas <ButtonRelease-1> "Timeline_UnclickState $win"

   # click-3 to set zoom point
   bind $canvas <3> "Timeline_SetZoomPt $win %x %y"

   # send a variable the time that the pointer is over
   if {$timeline($win,timevar) != ""} {
      bind $canvas <Motion> "Timeline_TimeVarSet $win %x"
   }

   # Call delete procedure if we get destroyed
   bind $canvas <Destroy> "Timeline_Destroy $win"
}



proc Timeline_Config {win arg_list} {
   global timeline color bw

   # puts stderr "Timeline_Config $arg_list"

   set log $timeline($win,log)

   # if both or neither flags are present, figure out whether we're on
   # a 'chromatically challenged' monitor
   if {![ArgBool arg_list -bw tempvar]} {
      set timeline($win,bw) $bw
   } else {
      set timeline($win,bw) $tempvar
   }

   if {![ArgOpt arg_list -width tempvar]} {
      set timeline($win,visWidth) [GetDefault disp_width 500]
   } else {
      set timeline($win,visWidth) [expr int($tempvar)]
   }


   # if height is not specified explicitly,
   if {![ArgOpt arg_list -height tempvar]} {

      # set my height to np * 25
      set timeline($win,visHeight) [expr [$log np] * \
	    [GetDefault timeline_proc_height 25]]

      # if a maximum height is specified,
      if {[ArgOpt arg_list -maxheight tempvar]} {

	 # cut down my height so it is no bigger than maxheight
	 if {$timeline($win,visHeight) > $tempvar} {
	    set timeline($win,visHeight) $tempvar
	 }
      }

   } else {
      set timeline($win,visHeight) $tempvar
   }

   if {![ArgOpt arg_list -bg tempvar]} {
      set timeline($win,bg) $color(disp_bg)
   } else {
      set timeline($win,bg) $tempvar
   }

   if {![ArgOpt arg_list -outlineColor tempvar]} {
      set timeline($win,outlineColor) $color(fg)
   } else {
      set timeline($win,outlineColor) $tempvar
   }

   if {![ArgOpt arg_list -msgColor tempvar]} {
      set timeline($win,msgColor) $color(arrowfg)
   } else {
      set timeline($win,msgColor) $tempvar
   }

   if {![ArgOpt arg_list -lineColor tempvar]} {
      set timeline($win,lineColor) $color(timeline)
   } else {
      set timeline($win,lineColor) $tempvar
   }

   if {![ArgOpt arg_list -timevar timeline($win,timevar)]} {
      set timeline($win,timevar) ""
   }

   if {![ArgOpt arg_list -setzoomptcmd timeline($win,setzoomptcmd)]} {
      set timeline($win,setzoomptcmd) ""
   }

   if {![ArgOpt arg_list -scan timeline($win,scan)]} {
      set timeline($win,scan) ""
   }
}


proc Timeline_Draw {win} {
   global timeline

   $timeline($win,c_cmd) draw
}



proc Timeline_Resize {win width height} {
   global timeline

   set timeline($win,visWidth) $width
   set timeline($win,visHeight) $height

   # puts "Timeline_Resize $width $height"
   Timeline_Draw $win
}



proc Timeline_Recalc {win} {
   global timeline

   set log $timeline($win,log)

   set timeline($win,pix2proc) \
	 [expr 1.0 * [$log np]  / $timeline($win,height)]

   set timeline($win,pix2time) \
	 [expr 1.0 * ([$log endtime] - [$log starttime]) / \
	 $timeline($win,width)]

}



proc Timeline_Cmd {win cmd args} {
   # puts "command '$win $cmd' called with args '$args'"

   switch $cmd {
      set {
	 return [eval Timeline_Set $win $args]
      }
      copy {
	 return [eval Timeline_Copy $win $args]
      }
      draw {
	 return [eval Timeline_C_Draw $win $args]
      }
      name {
	 return Timelines
      }
      pix2time {
	 return [eval Timeline_Pix2Time $win $args]
      }
      time2pix {
	 return [eval Timeline_Time2Pix $win $args]
      }
      pix2proc {
	 return [eval Timeline_Pix2Proc $win $args]
      }
      proc2pix {
	 return [eval Timeline_Proc2Pix $win $args]
      }
      zoom_time {
	 return [eval Timeline_ZoomTime $win $args]
      }
      zoom_proc {
	 return [eval Timeline_ZoomProc $win $args]
      }
      setleft {
	 return [eval Timeline_SetLeft $win $args]
      }
      setview {
	 return [eval Timeline_SetView $win $args]
      }
      xview {
	 eval $win.c xview $args
	 # update idletasks
      }
      yview {
	 eval $win.c yview $args
	 # update idletasks
      }
      default {
	 eval $win.c $cmd $args
      }
   }
   
}   



proc Timeline_Destroy {win} {
   global timeline

   # puts stderr "DESTROYING TIMELINE"

   $timeline($win,c_cmd) destroy

   EraseArrayElements timeline $win
}



proc Timeline_YSet {win t w f l} {
   # set the timeline display with scrollbar-like settings

   global timeline

   # puts "Timeline YSet $t $w $f $l"

   if $t==1 {
      set t [expr $w + 2]
      # puts "Changed total to $t"
   }

   $win.pn set $t $w $f $l
   $win.sc set $t $w $f $l
}


proc Timeline_TimeVarSet {win mark} {
   global timeline

      # why does this work, but 'global' doesn't?
   upvar #0 $timeline($win,timevar) var
   set var [format %.6f [$win pix2time [$win canvasx $mark]]]
}


proc Timeline_YScroll {scroll_list t w l r} {
   foreach scroll $scroll_list {
      $scroll set $t $w $l $r
   }
}


proc Timeline_SetZoomPt {win x y} {
   global timeline

   if {$timeline($win,setzoomptcmd) != ""} {
      # puts "convert pix $x to [$win canvasx $x] to\
	    [$win pix2time [$win canvasx $x]]"
      eval $timeline($win,setzoomptcmd) [$win pix2time [$win canvasx $x]]
   }
}


proc Timeline_Copy {win dest_canvas x y} {
   global timeline color procnums

   $win.pn copy $dest_canvas $x $y

   # set left_gap [GetDefault display_left_gap 40]
   set left_gap $procnums($win.pn,width)
   set canvas $win.c

   set left [$canvas canvasx 0]
   set right [expr $timeline($win,visWidth) + $left]
   set top [$canvas canvasy 0]
   set bottom [expr $timeline($win,visHeight) + $top]
   CopyCanvas $canvas [list $left $top $right $bottom] $dest_canvas \
	 [list [expr $x+$left_gap] [expr $y + 1]] color_disp_bg
   set left [expr $x + $left_gap]
   set top $y
   set right [expr $timeline($win,visWidth) + $x + $left_gap]
   set bottom [expr $timeline($win,visHeight) + $y + 1]
   trim_canvas $dest_canvas [expr $left+1] [expr $top+1] \
	 [expr $right-1] [expr $bottom-1]
   $dest_canvas create rect $left $top $right $bottom -outline $color(fg)
}



proc Timeline_ClickState {tl log x y screen_x screen_y} {
   global timeline

   set timeline($tl,post) 0
   set item [$timeline($tl,c_cmd) currentitem]
   if {$item == ""} return

   if {[lindex $item 0] == "state" } {
      # puts [$log state [lindex $item 1]]
      # puts "item = $item, index = [lindex $item 1]"
      set stateInfo [$log getstate [lindex $item 1]]
         # info: type, proc, startTime, endTime, parent, firstChild,
         # overlapLevel
      set stateName [lindex [$log getstatedef \
	    [lindex $stateInfo 0]] 0]
         # def: name, color, bitmap
      set startTime [format "%.6f" [lindex $stateInfo 2]]
      set endTime [format "%.6f" [lindex $stateInfo 3]]
      set len [format "%.6f" [expr $endTime - $startTime]]
      Timeline_PostStateInfo $tl $x $y $stateName $startTime $endTime $len \
	    $screen_x $screen_y
   } else {
      puts "Wierd. $tag is not a state."
   }
}
 
proc Timeline_PostStateInfo {tl x y name start end len screen_x screen_y} {
   global timeline

   toplevel $tl.infowin
   label $tl.infowin.l1 \
	 -text "State: $name, from $start to $end" \
	 -borderwidth 3 -relief raised
   label $tl.infowin.l2 \
	 -text "Length: $len sec."
   pack $tl.infowin.l1 $tl.infowin.l2
   # puts [wm overrideredirect $tl.infowin 1]
   wm transient $tl.infowin $tl
      # take virual window offset into account
   set x_cor [expr $screen_x - [winfo reqwidth $tl.infowin.l1] / 2 + \
	 [winfo vrootx $tl]]
   set y_cor [expr $screen_y - [winfo reqheight $tl.infowin.l1] - \
	 [winfo reqheight $tl.infowin.l2] + \
	 [winfo vrooty $tl] - 10]
      # move it up by, oh, 10 pixels to account for a border
   wm geometry $tl.infowin +$x_cor+$y_cor
   set timeline($tl,post) 1
}


proc Timeline_UnclickState {tl} {
   global timeline
   if {[info exists timeline($tl,post)]} {
      if {$timeline($tl,post)} {
	 destroy $tl.infowin
      }
   }
   set timeline($tl,post) 0
}


proc PrintTL {tl} {
   set canvas $tl.c

   set list [$canvas find withtag timelines]
   foreach line $list {
      set coords [$canvas coords $line]
      puts "line $line coords: $coords"
   }
}


proc Timeline_ZoomTime {tl time factor} {
   global timeline

   set canvas $tl.c

   # Get the x-coordinate of the zoom point
   set x [$tl time2pix $time]

   # Get the distance of that point from the left side
   # of the screen so we can keep that point stationary
   set screen_offset [expr $x - [$canvas canvasx 0] ]

   # puts "tl xcoord $x, screen left [$canvas canvasx 0],\
	 offset $screen_offset"

   # Zoom relative to the far left
   $canvas scale all 0 0 $factor 1

   # PrintTL $tl

   # keep timelines constant
   $canvas scale timelines 0 0 [expr 1.0 / $factor] 1

   # PrintTL $tl

   # reset my copy of the width
   set timeline($tl,width) [expr int($timeline($tl,width) * $factor)]

   # Reset the canvas's scrollable width; this triggers the scrollbars
   $canvas config -scrollregion [list 0 0 $timeline($tl,width) \
	 $timeline($tl,height)]

   # puts "timeline width = $timeline($tl,width)"

   # recalc the pix2time
   Timeline_Recalc $tl

   # Figure out the new xview coordinate so the screen seems
   # not to move much
   set xview [expr int($x * $factor - $screen_offset) ]

   # puts "tl xview = $xview ... expr int($x * $factor - $screen_offset)"

   if {$xview < 0} {
      set xview 0
   }

   # Reposition the screen so the zoom point seems stationary
   $tl xview $xview
}



proc Timeline_ZoomProc {tl proc factor} {
   global timeline

   set canvas $tl.c

   set y [$tl proc2pix $proc]
   set screen_offset [expr $y - [$canvas canvasy 0] ]

   # puts "proc $proc, ycoord $y, screen left [$canvas canvasy 0],\
	 offset $screen_offset"

   $canvas scale all 0 0 1 $factor
   set timeline($tl,height) [expr int($timeline($tl,height) * $factor)]
   $canvas config -scrollregion [list 0 0 $timeline($tl,width) \
	 $timeline($tl,height)]

   set yview [expr int(($y * $factor - $screen_offset) / 10) ]

   # puts "yview $yview"

   if {$yview < 0} {
      set yview 0
   }

   $tl.c yview $yview
}


proc Timeline_Pix2Time {tl pix} {
   global timeline

   return [expr $pix * $timeline($tl,totalTime) / \
	 $timeline($tl,width) + $timeline($tl,startTime)]
}

proc Timeline_Time2Pix {tl time} {
   global timeline

   return [expr 1.0 * ($time - $timeline($tl,startTime)) \
	 / $timeline($tl,totalTime) * $timeline($tl,width) ]
}


proc Timeline_Pix2Proc {tl pix} {
   global timeline

   return [expr $pix * $timeline($tl,np) / $timeline($tl,height)]
}

proc Timeline_Proc2Pix {tl proc} {
   global timeline

   return [expr 1.0 * $proc * $timeline($tl,height) / $timeline($tl,np)]
}


proc Timeline_SetView {win left span} {
   global timeline

   set timeline($win,xleft) $left
   set timeline($win,xspan) $span

   Timeline_Draw $win
}


proc Timeline_SetLeft {win left} {
   global timeline

   set timeline($win,xleft) $left
   Timeline_Draw $win
}


#
# Start a drag.  Remember the x-coordinate where the button was pushed
# and the left edge at the time.
#
proc Timeline_Mark {tl x y} {
   global timeline

      # convert window coordinate to canvas coordinate
   set timeline($tl,markpt) $x
   set timeline($tl,markleft) $timeline($tl,xleft)

      # remember how much time is covered in 10 pixels, so this
      # canvas be scaled from 1 pixel in the drag
   set timeline($tl,pixwidth) [expr [Timeline_Cmd $tl pix2time 10] - \
	 [Timeline_Cmd $tl pix2time 0]]
}

#
# Do a drag.  Given the data stored in Timeline_Mark, when the button was
# first pressed, move the display in the direction the mouse has moved,
# but magnify the distance 10 times.
#
proc Timeline_Dragto {tl x y} {
   global timeline

   set new_left [expr $timeline($tl,markleft) + $timeline($tl,pixwidth) * \
	 ($timeline($tl,markpt) - $x)]

      # don't scroll past the end
   if {$new_left < $timeline($tl,startTime)} {
      set new_left $timeline($tl,startTime)
   } elseif {$new_left > $timeline($tl,endTime) - $timeline($tl,xspan)} {
      set new_left [expr $timeline($tl,endTime) - $timeline($tl,xspan)]
   }

   set scan $timeline($tl,scan)
   if {$scan != ""} {
         # if everyone wants the new coordinate, send it to them
      eval $scan $new_left
   } else {
         # if nobody else cares, do it manually
      Timeline_SetLeft $win $new_left
   }
}


}
# set package(timelines) yup
