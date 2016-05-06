#
# Legend stuff for Upshot
#
# Ed karrels
# Argonne National laboratory
#
# legend structure:
#   log - the log this legend represents
#   nstates - # of states represented in the legend
#   width - width of the legend window
#   height - height of the legend window
#


#
# Create the legend
#
proc Legend_Create {frame log width} {
   global bw color legend

   canvas $frame -height 1 -width $width -bg $color(bg) -relief raised
   set canvas $frame
   set legend($frame,log) $log

   # margins
   set marginVert 5
   set marginHoriz 5
   # dimensions of boxes with sample color or bitmap
   set boxWidth 45
   set boxHeight 15
   set boxMiddle [expr $boxHeight/2]
   # space between box and corresponding label
   set spcBtwnBoxLbl 6
   # space between label and the next box
   set spcBtwnLblBox 10
   # vertical spacing between rows of boxes&labels
   set spcVert 10

   # draw each of the color boxes and labels
   set ycor 0
   set nstatedefs [$log nstatedefs]
   for {set stateno 0;set i 0} {$stateno < $nstatedefs} {incr stateno} {
      # i is the number of the legend button
      # i is separate from stateno so that some states may be skipped

         # skip this state if it is never used
      # puts "[$log nstatetypeinst $stateno] instances of state $stateno"
      if ![$log nstatetypeinst $stateno] continue;

         # get the state definition
      SplitList [$log getstatedef $stateno] {name state_color bitmap}

         # Use $stateno in the color tag so that when the legend is printed,
         # they get recolored correctly.  Use $i in the legend tag so
         # it gets moved correctly when the window is resized.
         # Use color_outline so the outline gets recolored too.
      
      if $bw {
	 set fill $color(fg)
	 set stipple $bitmap
      } else {
	 set fill $state_color
	 set stipple ""
      }
      $canvas create rectangle 0 $ycor $boxWidth \
	    [expr $ycor+$boxHeight]  -fill $fill \
	    -outline $color(fg) -tags [list legend l_$i \
	    color_$stateno color_outline] \
	    -stipple $stipple

      button $canvas.b$i -text $name -command "Hist_Open $log $stateno"
#     Can use "bind" to turn states on/off
#      bind   $canvas.b$i <2> "StateToggleActive $stateno"
      $canvas create window [expr $boxWidth+$spcBtwnBoxLbl] \
	    [expr $ycor+$boxMiddle] -window $canvas.b$i \
	    -tags [list legend l_$i color_fg] -anchor w
      #set y-coordinate of next box and label
      set ycor [expr [lindex [$canvas bbox l_$i] 3]+$spcVert]

      incr i
   }

      # save the # of states represented in the legend
   set legend($frame,nstates) $i

   # bind $canvas <Button> "Legend_Button $frame %x %y"
   bind $canvas <Configure> "Legend_Resize $frame %w"
   bind $canvas <Destroy> "Legend_Destroy $frame"
   # return [ResizeLegend $frame $log $givenWidth]
}


#
# Shuffle everyone around to fit the window again.
# Called automatically when the legend window is resized.
#
proc Legend_Resize {frame givenWidth} {
   global bw color legend

   set log $legend($frame,log)
   set canvas $frame

   if {![info exists legend($frame,width)] \
       || $legend($frame,width) != $givenWidth} {
      set legend($frame,width) $givenWidth
   } else {
      # puts "don't need to resize legend"
      return
   }

   set nstatedefs [$log nstatedefs]

   # these are all repeated in CreateLegend
   # margins
   set marginVert 5
   set marginHoriz 5
   # dimensions of boxes with sample color or bitmap
   set boxWidth 45
   set boxHeight 15
   set boxMiddle [expr $boxHeight/2]
   # space between box and corresponding label
   set spcBtwnBoxLbl 6
   # space between label and the next box
   set spcBtwnLblBox 10
   # vertical spacing between rows of boxes&labels
   set spcVert 10

   set AddToLine {
      $canvas move l_$i [expr $horiz-[lindex $bbox 0]] \
	    [expr $vert-[lindex $bbox 1]]
      if {$bheight>$maxHeight} {set maxHeight $bheight}
      set horiz [expr $horiz+$bwidth+$spcBtwnLblBox]
   }
   set ResetLine {
      set vert [expr $vert+$maxHeight]
      set horiz $marginHoriz
      set maxHeight 0
   }

   set width [expr $givenWidth-2*$marginHoriz]
   # legendLines = list of {maxHeight {0 1 2...}}
   set legendLines {}
   set horiz $marginHoriz
   set vert $marginVert
   # maxHeight = tallest state in this line
   set maxHeight 0
   # thisLine = {0 1 2... (which states are in this line)}
   set thisLine {}

      # The following trick is to keep the legend from
      # taking up the entire vertical space when there are dozens,
      # maybe hundreds, of different event names.  If there are too
      # many, then show none at all so it will be obvious to the
      # user that something is different.  Thanks to Dennis Cottel
      # (dennis@nosc.mil) for this change.

   set maxStates 30
   if {$legend($frame,nstates) > $maxStates} {
      set legend($frame,nstates) 0
      set marginVert 0
      set vert 0
   }
     
      # count through the list of states depicted, not every state type
      # some state types might not have ever been used, and should
      # not be depicted
   for {set i 0} {$i < $legend($frame,nstates)} {incr i} {
      # get width of current box and label
      set bbox [$canvas bbox l_$i]
      set bwidth [expr [lindex $bbox 2]-[lindex $bbox 0]]
      set bheight [expr [lindex $bbox 3]-[lindex $bbox 1]]
      
      if {$bwidth+$spcBtwnLblBox+$horiz <= $width} {
	 # if this state fits, good
	 eval $AddToLine
      } elseif {$horiz==$marginHoriz} {
	 # if this state is too long, but the only one on the line,
	 # well, tough luck.
	 eval $AddToLine
	 eval $ResetLine
      } else {
	 # state is too long, go to next line
	 eval $ResetLine
	 eval $AddToLine
      }
   }

   set legend($frame,height) [expr $vert+$marginVert+$maxHeight]
   $canvas configure -height $legend($frame,height)
}


#
# squeeze the legend by 40 on the left an 19 on the right to fit the region
# between the gaps left for process numbers and vertial scrollbars
#
proc Legend_Squeeze {win} {
   global legend

   set legend($win,sq_width) $legend($win,width)
      # subtract 40 for the left gap, 19 for the right gap
   Legend_Resize $win [expr $legend($win,width) - 40 - 19]
      # return the temporary height
   return $legend($win,height)
}

#
# unsqueeze the legend
#
proc Legend_Unsqueeze {win} {
   global legend

   Legend_Resize $win $legend($win,sq_width)
}


#
# copy the legend to a different canvas
#
proc Legend_Copy {win new_canvas x y} {
   global legend

   set canvas $win
   CopyCanvas $canvas [list 0 0 $legend($win,width) $legend($win,height)] \
	 $new_canvas [list $x $y] color_bg
}



proc Legend_Destroy {win} {
   global legend
   unset legend($win,width)
}
