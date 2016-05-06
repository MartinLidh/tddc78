
#
# Histogram widget stuff
#
# hist() structure:
#  (all fields except $w,data are linked to the C structure held by $w,data)
#
#   $w,window - name of the window
#   $w,canvas - name of the canvas
#   $w,time_lbl - name of the time_lbl
#   $w,xscrollbar - name of the horizontal scrollbar
#   $w,data - token for the C side of the histogram stuff
#   $w,nbins - # of bins
#   $w,maxbin - size of a bin that will fill the canvas vertically
#               (used to set vertical scale)
#   $w,left - far left edge
#   $w,right - far right edge
#   $w,width - width of canvas
#   $w,height - height of canvas
#   $w,color - color of the polygon
#   $w,bitmap - bitmap of the polygon
#   $w,xscrollcommand - command to execute to set an attached horiz. scrollbar
#
#  markers for dragging & stretching the canvas
#   $w,scan_time
#   $w,rclick_time
#   $w,lclick_time
#   $w,yclick
#
#  stats on all the data
#   nlens
#   sum
#   average
#   std_dev
#   shortest
#   longest
#
#  stats on the visible data
#   vis_nlens
#   vis_sum
#   vis_average
#   vis_std_dev
#   vis_shortest
#   vis_longest

proc Hist_Open {log state_no args} {
   global color hist

   set w .[GetUniqueWindowID]
   toplevel $w
   wm minsize $w 10 10

      # get the name of the state, use it for the title
   set hist($w,stateName) [lindex [$log getstatedef $state_no] 0]
   wm title $w "state $hist($w,stateName) lengths"

      # build user interface
   frame $w.help
   pack $w.help -side bottom -fill x -padx 5 -pady 5
      set help $w.help.t

   frame $w.r
   pack $w.r -side right -fill both -expand 1
      set canvas $w.r.c
      set time_lbl $w.r.t
      set xscroll $w.r.xsc

   frame $w.l
   pack $w.l -side left
      frame $w.l.cur -relief groove -borderwidth 2
      pack $w.l.cur -pady 5 -padx 5 -fill x
         set cursor_lbl $w.l.cur.lbl
         set cursor_val $w.l.cur.val
      frame $w.l.ns -relief groove -borderwidth 2
      pack $w.l.ns -pady 5 -padx 5 -fill x
         set nstates_lbl $w.l.ns.lbl
         set nstates_val $w.l.ns.val
         set nstates_pct $w.l.ns.pct
      frame $w.l.nbins -relief groove -borderwidth 2
      pack $w.l.nbins -pady 5 -padx 5 -fill x
         set bin_lbl $w.l.nbins.lbl
         set bin_control $w.l.nbins.control
      set resize_to_fit $w.l.rstf
      set print $w.l.print
      set close $w.l.close

      # open the data on the C end
   set hist($w,data) [hist open $log $state_no hist $w]

   set hist($w,window) $w
   set hist($w,canvas) $canvas
   set hist($w,time_lbl) $time_lbl
   set hist($w,xscrollbar) $xscroll
   set hist($w,nbins)  25
   set hist($w,maxbin)  25

   if {$hist($w,n) == 0} {
      message $w.m -text "There are no instances of $hist($w,stateName)\
	    states." -aspect 350 -relief ridge -borderwidth 2
      button $w.ok -text "OK" -command "destroy $w"
      pack $w.m $w.ok -padx 5 -pady 5
      return
   }


      # Initially, I wanted to set the view to center at the
      # average state length, and to one standard deviation to either
      # side of that, but it never worked out real well.  Just do it
      # to the far end on either side
   if 0 {
   set center $hist($w,average)

   set hist($w,left) [expr $center - $hist($w,std_dev)]
   if {$hist($w,left) < $hist($w,shortest)} {
      set hist($w,left) $hist($w,shortest)
   }
   set hist($w,right) [expr $center + $hist($w,std_dev)]
   if {$hist($w,right) > $hist($w,longest)} {
      set hist($w,right) $hist($w,longest)
   }
   }

   set hist($w,left) $hist($w,shortest)
   set hist($w,right) $hist($w,longest)

      # Figure the bins, get the tallest one, and scale according to it.
      # Don't worry about recomputing the bins again to draw, the C
      # routines will notice the repeated request and reuse the data
      # computed in this call.
   hist $hist($w,data) bins $hist($w,nbins) \
	 $hist($w,left) $hist($w,right) \
	 tallest_bin
   set hist($w,maxbin) [expr $tallest_bin * 1.05]

   set hist($w,width) 400
   set hist($w,height) 250

   SplitList [$log getstatedef $state_no] \
	 [list hist($w,name) hist($w,color) hist($w,bitmap)]

      # set the fill color/stipple pattern
   global bw
   set hist($w,bw) $bw
   if $bw {
      set hist($w,fill_cmd) -stipple
      set hist($w,filler) $hist($w,bitmap)
   } else {
      set hist($w,fill_cmd) -fill
      set hist($w,filler) $hist($w,color)
   }
   set hist($w,outlineColor) $color(fg)

      # put help message to the bottom
   text $help -height 4 -relief groove -borderwidth 2
   $help insert 1.0 "Drag any button to stretch histograms vertically.\n"
   $help insert 1.0 "Drag right button to stretch out right side of\
	 histogram display.\n"
   $help insert 1.0 "Drag middle button to slide histogram display.\n"
   $help insert 1.0 "Drag left button to stretch out left side of\
	 histogram display.\n"

      # create the canvas on which everything will be drawn
   canvas $canvas -height $hist($w,height) -width $hist($w,width) \
	 -bg $color(disp_bg)
      # capture resize requests
   bind $canvas <Configure> "Hist_Resize $w %w %h"
      # open a time_lbl at the bottom
   time_lbl $time_lbl $hist($w,shortest) $hist($w,longest)
      # create close button
   button $close -text "Close" -command "Hist_Close $w"
      # create scrollbar at the bottom
   scrollbar $xscroll -orient horiz -command "Hist_Xview $w"

      # create cursor position display
   label $cursor_lbl -text "Cursor:"
   set hist($w,cursor) 0
   label $cursor_val -textvariable hist($w,cursor) -width 12

      # show the current # of states and what percentage of the total that is
   label $nstates_lbl -text "# of states:"
   set hist($w,vis_n) 0
   label $nstates_val -textvariable hist($w,vis_n)
   label $nstates_pct -textvariable hist($w,percent)

      # create #-of-bins slider bar
   label $bin_lbl -text "# of bins:"
   scale $bin_control -show 1 -from 1 -to 200 -command "Hist_SetNbins $w" \
	 -orient horiz
      # set inital value of slider
   $bin_control set $hist($w,nbins)

      # additional buttons
      # I want to add a 'Reset-view' button
   button $resize_to_fit -text "Resize to fit" -command "Hist_ResizeToFit $w"
   button $print -text "Print" -command "Print_Hist_Dialog $log $w"

      # pack everybody
   pack $help -fill x
   pack $cursor_lbl -side left -padx 5 -pady 5
   pack $cursor_val -fill x -padx 5 -pady 5
   pack $nstates_lbl -side left -padx 5 -pady 5
   pack $nstates_val -fill x -padx 5 -pady 5
   pack $nstates_pct -fill x -anchor e
   pack $bin_lbl -fill x -anchor w
   pack $bin_control -fill x
   pack $resize_to_fit -padx 5 -pady 5
   pack $print -padx 5 -pady 5
   pack $close -padx 10 -pady 10

   pack $xscroll $time_lbl -side bottom -fill x
   pack $canvas -fill both -expand 1

   Hist_Draw $w

   bind $canvas <Motion> "Hist_SetCursor $w %x"

   # bindings for 1st button left edge dragging
   bind $canvas <ButtonPress-1> "Hist_LeftClick $w %x %y"
   bind $canvas <Button1-Motion> "Hist_LeftDrag $w %x %y"

   # bindings for 2nd button scanning
   bind $canvas <ButtonPress-2> "Hist_MidClick $w %x %y"
   bind $canvas <Button2-Motion> "Hist_MidDrag $w %x %y"

   # bindings for 3rd button right edge dragging
   bind $canvas <ButtonPress-3> "Hist_RightClick $w %x %y"
   bind $canvas <Button3-Motion> "Hist_RightDrag $w %x %y"
}



proc Hist_Config {w args} {
   global hist

   if {![ArgOpt args -xscrollcommand tempvar]} {
      set hist($w,xscrollcommand) ""
   } else {
      set hist($w,xscrollcommand) $tempvar
   }

   if {![ArgOpt args -yscrollcommand tempvar]} {
      set hist($w,yscrollcommand) ""
   } else {
      set hist($w,yscrollcommand) $tempvar
   }
}



proc Hist_SetNbins {w nbins} {
   global hist

   set hist($w,nbins) $nbins

   Hist_Draw $w
}


proc Hist_SetMaxbin {w log_maxbin} {
   global hist

   # set hist($w,maxbin) [expr pow(10,$log_maxbin * 10)]

   # Hist_Draw $w
}



proc Hist_Resize {w width height} {
   global hist

   set canvas $hist($w,canvas)

   set hist($w,width) $width
   set hist($w,height) $height

   Hist_Draw $w
}


proc Hist_ResizeToFit {w} {
   global hist

   set bins [hist $hist($w,data) bins $hist($w,nbins) \
	 $hist($w,left) $hist($w,right) \
	 tallest_bin]

   set hist($w,maxbin) [expr $tallest_bin * 1.05]

   Hist_Draw $w
}



proc Hist_Draw {w} {
   global hist

   set canvas $hist($w,canvas)
   set h $hist($w,data)

   $canvas delete hist

   hist $h draw

   # hist $h pointlist pointlist $hist($w,nbins) \
	 $hist($w,left) $hist($w,right) \
	 $hist($w,width) $hist($w,height) $hist($w,maxbin)

   # fill_cmd is either -fill or -stipple
   # filler is either the color or the bitmap
   # eval $canvas create polygon $pointlist \
	 $hist($w,fill_cmd) $hist($w,filler) \
	 -tags hist
   # hist $h boundary $canvas $hist($w,outlineColor) hist
   # Hist_Boundary $canvas $pointlist $hist($w,outlineColor) hist

   # hist $hist($w,data) scroll
   Hist_SetScroll $w

   set hist($w,vis_n) $hist($w,vis_n)
   set hist($w,percent) "[expr int( 100*$hist($w,vis_n)/$hist($w,n))]%"
   set hist($w,canvas_tag) hist

   update idletasks
}



proc Hist_Boundary {canvas pointList color tags} {

   set npoints [expr [llength $pointList]/2]
   if $npoints==0 {return}

   set firstx [lindex $pointList 0]
   set firsty [lindex $pointList 1]
   set lastx $firstx
   set lasty $firsty
   for {set i 1} {$i<$npoints} {incr i} {
      set thisx [lindex $pointList [expr $i*2]]
      set thisy [lindex $pointList [expr $i*2+1]]
      $canvas create line $lastx $lasty $thisx $thisy -fill $color -tags $tags
      set lastx $thisx
      set lasty $thisy
   }
   $canvas create line $lastx $lasty $firstx $firsty -fill $color -tags $tags
}



proc Hist_XScroll {w tu wu fu lu} {
   $w.r.xsc set $tu $wu $fu $lu
   #$w.r.t set $tu $wu $fu $lu
}



proc Hist_SetScroll {w} {
   global hist

   return [hist $hist($w,data) setscroll]

   set time_lbl $w.r.t
   set xscroll $w.r.xsc

   set left $hist($w,left) 
   set span [expr $hist($w,right) - $hist($w,left)]
   $time_lbl setview $left $span
   set hist($w,windowUnits) [expr int(10000.0 * $span / \
	 ($hist($w,longest) - $hist($w,shortest)) )]
   set l [expr int(10000.0*($left-$hist($w,shortest)) / \
	 ($hist($w,longest) - $hist($w,shortest)) )]
   set r [expr $l+$hist($w,windowUnits)]
   $xscroll set 10000 $hist($w,windowUnits) $l $r
   return
}



proc Hist_SetCursor {w pix} {
   global hist

   set hist($w,cursor) [format "%12.6f" [Hist_Pix2Time $w $pix]]
}



proc Hist_Pix2Time {w pix} {
   global hist

   return [expr 1.0 * $pix / $hist($w,width) * \
	 ($hist($w,right) - $hist($w,left)) + $hist($w,left)]
}



proc Hist_Pix2Ht {w pix} {
   global hist

   # convert y coordinate to the height of bin that would rise to
   # that point

   set ht [expr 1.0 * ($hist($w,height) - $pix) / \
	   $hist($w,height) * $hist($w,maxbin)]

   return $ht
}



proc Hist_MidClick {w x y} {
   global hist

   set hist($w,scan_time) [Hist_Pix2Time $w $x]
   Hist_YClick $w $y
}


proc Hist_MidDrag {w x y} {
   global hist

   set span [expr $hist($w,right) - $hist($w,left)]
   set left [expr $hist($w,scan_time) - \
	 $span * $x / $hist($w,width)]
   set right [expr $left + $hist($w,right) - $hist($w,left)]

   if {$left < $hist($w,shortest)} {
      set diff [expr $hist($w,shortest) - $left]
      set left [expr $left + $diff]
      set right [expr $right + $diff]

   } elseif {$right > $hist($w,longest)} {
      set diff [expr $hist($w,longest) - $right]
      set left [expr $left + $diff]
      set right [expr $right + $diff]
   }

   set hist($w,left) $left
   set hist($w,right) $right

   Hist_YDrag $w $y

   Hist_Draw $w
}


proc Hist_RightClick {w x y} {
   global hist

   set hist($w,rclick_time) [Hist_Pix2Time $w $x]
   Hist_YClick $w $y
}



proc Hist_RightDrag {w x y} {
   # set the right point so that the click point move to wherever I dragged
   # to

   global hist

   if {$x < 1} {
      set x 1
   }

   set new_right [expr ($hist($w,rclick_time) - $hist($w,left)) * \
	 $hist($w,width) / $x + $hist($w,left)]

   if {$new_right > $hist($w,longest)} {
      set new_right $hist($w,longest)
   }

   set hist($w,right) $new_right

   Hist_YDrag $w $y

   Hist_Draw $w
}



proc Hist_LeftClick {w x y} {
   global hist

   set hist($w,lclick_time) [Hist_Pix2Time $w $x]
   Hist_YClick $w $y
}



proc Hist_LeftDrag {w x y} {
   # set the left point so that the click point moves to wherever I dragged
   # to

   global hist

   if {$x >= $hist($w,width)} {
      set x [expr $hist($w,width) - 1]
   }

   set new_left [expr $hist($w,right) - \
	 ($hist($w,right) - $hist($w,lclick_time)) * \
	 $hist($w,width) / \
	 ($hist($w,width) - $x)]

   if {$new_left < $hist($w,shortest)} {
      set new_left $hist($w,shortest)
   }

   set hist($w,left) $new_left

   Hist_YDrag $w $y

   Hist_Draw $w
}



proc Hist_YClick {w y} {
   global hist

   set hist($w,yclick) [Hist_Pix2Ht $w $y]

   # puts "yclick at $hist($w,yclick)"
}


proc Hist_YDrag {w y} {
   global hist

   if {$y >= $hist($w,height)} {
      set y [expr $hist($w,height) - 1]
   }

   set new_maxbin [expr 1.0 * $hist($w,yclick) * $hist($w,height) / \
	 ($hist($w,height) - $y)]

   # puts "new_maxbin = $new_maxbin"
   
   set hist($w,maxbin) $new_maxbin
}



proc Hist_Xview {w x} {
   global hist

   return [hist $hist($w,data) xview $x]

   set screenWidth [expr $hist($w,right) - $hist($w,left)]
   set totalWidth [expr 1.0 * $hist($w,width) * \
	 ($hist($w,longest) - $hist($w,shortest)) / $screenWidth]
   set new_left [expr 1.0 * $x * ($hist($w,longest) - $hist($w,shortest)) / \
	 $totalWidth + $hist($w,shortest)]

   if {$new_left < $hist($w,shortest)} {
      # if this is past the left edge, set to exactly the left edge
      set new_left $hist($w,shortest)
   } elseif {$new_left + $screenWidth > $hist($w,longest)} {
      # if this is past the right edge, set to exactly the right edge
      set new_left [expr $hist($w,longest) - $screenWidth]
   }

      # if no change, don't redraw
   if {$new_left == $hist($w,left)} return

   # puts "set new left edge to $new_left"
   set hist($w,left) $new_left
   set hist($w,right) [expr $new_left + $screenWidth]

   Hist_Draw $w
}



proc Hist_Close {w} {
   global hist

   hist $hist($w,data) close
   EraseArrayElements hist $w
   destroy $w
}
