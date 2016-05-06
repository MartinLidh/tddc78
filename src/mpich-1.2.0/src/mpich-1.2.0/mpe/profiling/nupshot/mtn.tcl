#
# Mountain range stuff for Upshot
#
 ######    ####		  #   #	   ## #	 #     #      ###    ##   ###
 #     	  #   #        	  #  # 	  #  ##	 #     #     #       #	 #   #
 #     	  #   #  	  # #  	  #   #	 ##    ##    #####   #	     #
 ####  	   ####  	  ##   	   ### 	 # ##  # ##   ###    #	  ###
 #     	      #  	  # #  				     #	 #
 #     	      #  	  #  #				     #	  ###
 ######	      #  	  #   #				     #

#
#  Argonne National Laboratory
#  (I don't have time to do "Argonne National Laboratory" in big
#   letters, sorry)
#
#
# mtn structure:
#  markpt - for a scan the window x-coordinate where the button was first
#           clicked
#  markleft - for a scan, the initial 'left' edge
#  pixwidth - used in scanning, the amount of time between 10 pixels


# create a mountain range widget
proc mtn {frame log args} {
   global mtn

   set w $frame
   set canvas $w.c

   frame $w

   set mtn($w,c_cmd) ${w}_c_cmd
   mtn_c_init $mtn($w,c_cmd) $log $canvas mtn $w
   set mtn($w,canvasName) $canvas

   # configure--fill in colors&stuff
   eval mtn_config $w $args

   # frame $w.l -width [GetDefault display_left_gap 40]
   Procnums_Create $w.pn $log

   canvas $canvas \
	 -height $mtn($w,visHeight) \
	 -width $mtn($w,visWidth) \
	 -bg $mtn($w,bg) \
	 -yscrollcommand "mtn_yset $w" \
	 -scrollincrement 1 \
	 -scrollregion [list 0 0 [expr $mtn($w,width) - 1] \
	                     [expr $mtn($w,height) - 1] ]

   scrollbar $w.sc -orient vert -command "$frame yscrollset"

   set mtn($w,xleft) $mtn($w,startTime)
   set mtn($w,xspan) $mtn($w,totalTime)
   set mtn($w,ytop)  0
   set mtn($w,yspan) $mtn($w,np)

      # be sure to pack the canvas last so it sucks up all the space
   pack $w.pn -side left -fill y
   pack $w.sc -side right -fill y
   pack $canvas -fill both -expand 1 -side left

   # rename the command created for my host, the frame
   set mtn($w,frame_cmd) ${w}_frame_cmd
   rename $w $mtn($w,frame_cmd)

   # create a command just for me
   proc $frame {cmd args} "eval mtn_cmd $w {\$cmd} \$args"

   bind $canvas <Configure> "mtn_resize $w %w %h"

   # bindings for 2nd button scanning
   bind $canvas <2> "mtn_mark $w %x %y"
   bind $canvas <Button2-Motion> "mtn_dragto $w %x %y"

   # click-3 to set zoom point
   bind $canvas <3> "mtn_setZoomPt $w %x %y"

   # send a variable the time that the pointer is over
   if {$mtn($w,timevar) != ""} {
      bind $canvas <Motion> "mtn_timeVarSet $w %x"
   }

      # delete the frame that I renamed
   bind $canvas <Destroy> "mtn_destroy $w"
}


proc mtn_config {w args} {
   global mtn color bw

   # set visHeight, visWidth, bg, outline, and bw

   if {![ArgBool args -bw mtn($w,bw)]} {
      set $mtn($w,bw) $bw
   }
   
   if {![ArgOpt args -bg tempvar]} {
      set mtn($w,bg) $color(disp_bg)
   } else {
      set mtn($w,bg) $tempvar
   }

   if {![ArgOpt args -fg tempvar]} {
      set mtn($w,outline) $color(fg)
   } else {
      set mtn($w,outline) $tempvar
   }

   if {![ArgOpt args -width tempvar]} {
      set mtn($w,visWidth)  [GetDefault disp_width 500]
   } else {
      set mtn($w,visWidth) $tempvar
   }

   if {![ArgOpt args -height tempvar]} {
      set mtn($w,visHeight) [GetDefault disp_height 200]
   } else {
      set mtn($w,visHeight) $tempvar
   }

   # if {![ArgOpt args -xscrollcommand mtn($w,xscrollcommand)]} {
   #    set mtn($w,xscrollcommand) ""
   # }

   if {![ArgOpt args -scan mtn($w,scan)]} {
      set mtn($w,scan) ""
   }

   if {![ArgOpt args -timevar mtn($w,timevar)]} {
      set mtn($w,timevar) ""
   }

}


proc mtn_xset {win t w f l} {
   global mtn

   set xsc $mtn($win,xscrollcommand)
   set xva $mtn($win,xviewall)

   if {$xsc != ""} {
      eval $xsc $t $w $f $l
      if {$mtn($win,isdragto) && ($xva != "")} {
	 eval $xva $f
      }
   }
}

proc mtn_yset {win t w f l} {
   $win.sc set $t $w $f $l
   $win.pn set $t $w $f $l
   # puts "$win.sc set $t $w $f $l"
}


proc mtn_timeVarSet {win x} {
   global mtn

   upvar #0 $mtn($win,timevar) var
   set var [format %.6f [$win pix2time [$win canvasx $x]]]
}



proc mtn_resize {w width height} {
   global mtn

   set mtn($w,visWidth) $width
   set mtn($w,visHeight) $height

   # puts "mtn_resize to ${width}x$height"

   # puts "drawing mountain range"
   $w.c delete all
   $w draw
}



proc mtn_cmd {w cmd args} {
   global mtn

   set canvas $w.c

   switch $cmd {
      pix2time {
	 # the only argument should be the pixel coordinate
	 return [expr $args * $mtn($w,totalTime) \
	       / $mtn($w,width) + $mtn($w,startTime)]
      }

      time2pix {
	 # the only argument should be the time
	 return [expr ($args - $mtn($w,startTime)) * $mtn($w,width) / \
	       $mtn($w,totalTime)]
      }

      setleft {
	 return [eval mtn_setleft $w $args]
      }

      canvasx {
	 return [eval $canvas canvasx $args]
      }

      canvasy {
	 return [eval $canvas canvasy $args]
      }

      copy {
	 return [eval mtn_copy $w $args]
      }

      close {
	 return [eval $mtn($w,c_cmd) $cmd $args]
      }

      draw {
	 set status [eval $mtn($w,c_cmd) $cmd $args]
	 return $status
      }

      name {
	 return "Mountain Ranges"
      }

      setview {
	 return [eval mtn_setview $w $args]
      }

      xview {
	 return [eval $canvas xview $args]
      }

      zoom_time {
	 return [eval Mtn_ZoomTime $w $args]
      }

      default {
	 error "unrecognized command '$cmd' for mountain range widget"
      }

   }
}


proc mtn_destroy {w} {
   global mtn

   $w close

      # destroy the command for the frame that I renamed
   rename $mtn($w,frame_cmd) ""
}


proc Mtn_ZoomTime {win time factor} {
   global mtn

   set canvas $win.c

   # Get the x-coordinate of the zoom point
   set x [$win time2pix $time]

   # Get the distance of that point from the left side
   # of the screen so we can keep that point stationary
   set screen_offset [expr $x - [$canvas canvasx 0] ]

   # puts "mtn xcoord $x, screen left [$canvas canvasx 0],\
	 offset $screen_offset"

   # Zoom relative to the far left
   $canvas scale all 0 0 $factor 1

   # keep vertical constant
   $canvas scale timelines 0 0 [expr 1.0 / $factor] 1

   # reset my copy of the width
   set mtn($win,xsc_t) [expr int($mtn($win,xsc_t) * $factor)]

   # Reset the canvas's scrollable width; this triggers the scrollbars
   $canvas config -scrollregion [list 0 0 $mtn($win,xsc_t) \
	 $mtn($win,ysc_t)]

   # puts "mountain width = $mtn($win,width)"

   # Figure out the new xview coordinate so the screen seems
   # not to move much
   set xview [expr int($x * $factor - $screen_offset) ]
   
   # puts "mtn xview = $xview ... expr int($x * $factor - $screen_offset)"

   if {$xview < 0} {
      set xview 0
   }

   # Reposition the screen so the zoom point seems stationary
   $win xview $xview
}


proc mtn_setleft {win left} {
   global mtn

   set mtn($win,xleft) $left
   $win draw
}


proc mtn_setview {win left span} {
   global mtn

   set mtn($win,xleft) $left
   set mtn($win,xspan) $span
   $win.c delete all
   $win draw
}


#
# Start a drag.  Remember the x-coordinate where the button was pushed
# and the left edge at the time.
#
proc mtn_mark {win x y} {
   global mtn
      # convert window coordinate to canvas coordinate
   set mtn($win,markpt) $x
   set mtn($win,markleft) $mtn($win,xleft)
      # remember how much time is covered in 10 pixels, so this
      # can be scaled from 1 pixel in the drag
   set mtn($win,pixwidth) [expr [mtn_cmd $win pix2time 10] - \
	 [mtn_cmd $win pix2time 0]]
}


#
# Do a drag.  Given the data stored in mtn_mark, when the button was
# first pressed, move the display in the direction the mouse has moved,
# but magnify the distance 10 times.
#
proc mtn_dragto {win x y} {
   global mtn
   set new_left [expr $mtn($win,markleft) + $mtn($win,pixwidth) * \
	 ($mtn($win,markpt) - $x)]

      # don't scroll past the end
   if {$new_left < $mtn($win,startTime)} {
      set new_left $mtn($win,startTime)
   } elseif {$new_left > $mtn($win,endTime) - $mtn($win,xspan)} {
      set new_left [expr $mtn($win,endTime) - $mtn($win,xspan)]
   }

   set scan $mtn($win,scan)
   if {$scan != ""} {
      # if everyone wants the new coordinate, send to all
      eval $scan $new_left
   } else {
      # if nobody else cares, do it manually
      mtn_setleft $win $new_left
   }
}


#
# Copy this display (including procnums) to another canvas
#
proc mtn_copy {win dest_canvas x y} {
   global mtn color

   $win.pn copy $dest_canvas $x $y

   set left_gap [GetDefault display_left_gap 40]

   set canvas $win.c

   set left [$canvas canvasx 0]
   set right [expr $mtn($win,visWidth) + $left]
   set top [$canvas canvasy 0]
   set bottom [expr $mtn($win,visHeight) + $top]
   CopyCanvas $canvas [list $left $top $right $bottom] $dest_canvas \
	 [list [expr $x+$left_gap] [expr $y + 1]] color_disp_bg
   set left [expr $x + $left_gap]
   set top $y
   set right [expr $mtn($win,visWidth) + $x + $left_gap]
   set bottom [expr $mtn($win,visHeight) + $y + 1]
   # trim_canvas $dest_canvas $left $top $right $bottom
   $dest_canvas create rect $left $top $right $bottom -outline $color(fg)
}
