#
# Horizontal zoom-handling for Upshot
#
# Ed Karrels
# Argonne National laboratory
#


#
# display - name of display widget
#
# id - id of mainwin
#
proc Zoom {id display orient {factor 2.0}} {
   global mainwin
   # orient is either "horiz" or "vert"

      # vertical zooming is going to be on a per-display basis
   if {$orient == "vert"} return

      # don't try anything if there are no displays
   set display_list [$display list]
   if {$display_list == ""} return

   set disp [lindex $display_list 0]

   if {$orient == "reset"} {
      SetView $id $mainwin($id,startTime) $mainwin($id,totalTime)
      return
   }

   if {$orient == "horiz"} {
      if [info exists mainwin($id,zoom_time)] {
	 set point $mainwin($id,zoom_time)
      } else {
	 set point [expr $mainwin($id,left) + $mainwin($id,span)/2]
      }


      set left [expr $point - ($point - $mainwin($id,left)) / $factor]
      set span [expr 1.0 * $mainwin($id,span) / $factor]

         # if the zoom will in any way go out of bounds, change it
         # to fit
      if {$span > $mainwin($id,totalTime)} {
	 set span $mainwin($id,totalTime)
      }
      if {$left < $mainwin($id,startTime)} {
	 set left $mainwin($id,startTime)
      } elseif {$left + $span > $mainwin($id,endTime)} {
	 set left [expr $mainwin($id,endTime) - $span]
      }
      SetView $id $left $span

   } else {
      if [info exists mainwin($id,zoom_proc)] {
	 set point $mainwin($id,zoom_proc)
      } else {
	 set top [$disp canvasy 0]
	 set bottom [$disp canvasy [winfo height $disp]]
	 set center [expr ($top + $bottom) / 2.0]
	 set point [$disp pix2proc $center]
      }

      set cmd zoom_proc

   }

}



proc Zoom_SetTime {id time} {
   global mainwin

   # puts "Zoom time $time"
  
   set mainwin($id,zoom_time) $time
}
   


proc DetailedZoom {id} {
   # Give the user a dialog box from which to pick exactly what type of
   # zoom he wants
   global dzoom setting

   set dzoomwin .[GetUniqueWindowID]

   toplevel $dzoomwin
   wm title $dzoomwin "Detailed zoom on $setting($id,logfilename)"
   set timeframe $dzoomwin.time
   set procframe $dzoomwin.proc
   frame $timeframe -relief raised
   frame $procframe -relief raised
   frame $timeframe.fill -width 20
   frame $procframe.fill -width 20
   frame $timeframe.factor -relief raised
   frame $timeframe.explicit -relief raised
   frame $timeframe.factor.factor_f
   frame $timeframe.factor.point_f
   frame $timeframe.explicit.from_f
   frame $timeframe.explicit.to_f
   frame $procframe.values
   frame $procframe.values.from_f
   frame $procframe.values.to_f

   radiobutton $timeframe.btn -text Time -variable dzoom($dzoomwin,axis) \
	 -value 0
   radiobutton $procframe.btn -text Processes -variable dzoom($dzoomwin,axis) \
	 -value 1

   radiobutton $timeframe.factorbtn -text Factor -variable \
	 dzoom($dzoomwin,hzoomtype) -value 0
   radiobutton $timeframe.explicitbtn -text "Explicit time" -variable \
	 dzoom($dzoomwin,hzoomtype) -value 1

   label $timeframe.factor.point_f.pointlbl   -text "Zoom point"
   label $timeframe.factor.factor_f.factorlbl -text "Zoom factor"
   label $timeframe.explicit.from_f.fromlbl -text "From"
   label $timeframe.explicit.to_f.tolbl   -text "To"

   label $procframe.values.from_f.fromlbl   -text "From"
   label $procframe.values.to_f.tolbl       -text "To"

   entry $timeframe.factor.point_f.point  -textvariable \
	 dzoom($dzoomwin,time,point) \
	 -relief sunken
   entry $timeframe.factor.factor_f.factor -textvariable \
	 dzoom($dzoomwin,time,factor) \
	 -relief sunken
   entry $timeframe.explicit.from_f.from   -textvariable \
	 dzoom($dzoomwin,time,from) \
	 -relief sunken
   entry $timeframe.explicit.to_f.to     -textvariable \
	 dzoom($dzoomwin,time,to) \
	 -relief sunken
   entry $procframe.values.from_f.zoomfrom   -textvariable \
	 dzoom($dzoomwin,process,from) \
	 -relief sunken
   entry $procframe.values.to_f.zoomto     -textvariable \
	 dzoom($dzoomwin,process,to) \
	 -relief sunken
   
   button $dzoomwin.zoom -text Zoom -command \
	 "DetailedZoomZoomit $id $dzoomwin"
   button $dzoomwin.cancel -text Cancel -command \
	 "DetailedZoomCancel $id $dzoomwin"

   SetDetailedZoomVars $id $dzoomwin

   pack append $dzoomwin \
	 $timeframe {top frame w padx 20 pady 20} \
	 $procframe {top frame w padx 20 pady 20} \
	 $dzoomwin.zoom {left expand} \
	 $dzoomwin.cancel {right expand}
   pack append $timeframe \
	 $timeframe.btn {top frame w} \
	 $timeframe.fill {left} \
	 $timeframe.factorbtn {top frame w padx 10} \
	 $timeframe.factor {top padx 20} \
	 $timeframe.explicitbtn {top frame w padx 10} \
	 $timeframe.explicit {top padx 20}
   pack append $procframe \
	 $procframe.btn {top frame w} \
	 $procframe.fill {left} \
	 $procframe.values {top padx 10}
   pack append $timeframe.factor \
	 $timeframe.factor.factor_f {top fillx} \
	 $timeframe.factor.point_f {top fillx}
   pack append $timeframe.explicit \
	 $timeframe.explicit.from_f {top fillx} \
	 $timeframe.explicit.to_f {top fillx}
   pack append $procframe.values \
	 $procframe.values.from_f {top fillx} \
	 $procframe.values.to_f {top fillx}

   pack append $timeframe.factor.point_f \
	 $timeframe.factor.point_f.pointlbl {left} \
	 $timeframe.factor.point_f.point {right}
   pack append $timeframe.factor.factor_f \
	 $timeframe.factor.factor_f.factorlbl {left} \
	 $timeframe.factor.factor_f.factor {right}
   pack append $timeframe.explicit.from_f \
	 $timeframe.explicit.from_f.fromlbl {left} \
	 $timeframe.explicit.from_f.from {right}
   pack append $timeframe.explicit.to_f \
	 $timeframe.explicit.to_f.tolbl {left} \
	 $timeframe.explicit.to_f.to {right}
   pack append $procframe.values.from_f \
	 $procframe.values.from_f.fromlbl {left} \
	 $procframe.values.from_f.zoomfrom {right}
   pack append $procframe.values.to_f \
	 $procframe.values.to_f.tolbl {left} \
	 $procframe.values.to_f.zoomto {right}
	 
	 
}


proc SetDetailedZoomVars {id dzoomwin} {
   global setting dzoom procWidth

   # get scroll region info

   GetVisibleRegion $id dzoom($dzoomwin,time,from) dzoom($dzoomwin,time,to) \
	 dzoom($dzoomwin,process,from) dzoom($dzoomwin,process,to)
   

   set dzoom($dzoomwin,axis) [GetDefault zoomaxis 0]
   set dzoom($dzoomwin,hzoomtype) [GetDefault hzoomtype 0]

   set dzoom($dzoomwin,time,factor) 2
   set dzoom($dzoomwin,time,point) [Pixel2Time $id $setting($id,mark,x)]
}



proc DetailedZoomZoomit {id dzoomwin} {
   global dzoom setting procWidth
   UpdateDefaults [list zoomaxis $dzoom($dzoomwin,axis) \
	 hzoomtype $dzoom($dzoomwin,hzoomtype)]

   if {$dzoom($dzoomwin,axis)==0} {
      # horizontal zoom
      if {$dzoom($dzoomwin,hzoomtype)==0} {
	 # factor zoom (easiest)
	 set setting($id,mark,x) [Time2Pixel $id $dzoom($dzoomwin,time,point)]
	 Zoom $id x $dzoom($dzoomwin,time,factor)
      } else {
	 GetVisibleRegion $id firstTime lastTime firstProc lastProc
	 # puts "firsttime = $setting($id,firstTime)"
	 set factor [expr {(($lastTime-$firstTime)/ \
	       ($dzoom($dzoomwin,time,to) - \
	       $dzoom($dzoomwin,time,from)))}]
	 set newCenter [expr {($dzoom($dzoomwin,time,from) + \
	       $dzoom($dzoomwin,time,to))/2.0}]
	 set setting($id,mark,x) [Time2Pixel $id $newCenter]
	 #puts "Zoom $id x $factor"
	 Zoom $id x $factor
	 GetVisibleRegion $id firstTime lastTime firstProc lastProc
	 # puts "set currentCenter expr ($lastTime+$firstTime)/2.0"
	 set currentCenter [expr ($lastTime+$firstTime)/2.0]
	 #puts "MoveTime $id [expr $currentCenter-$newCenter] "
	 MoveTime $id [expr $currentCenter-$newCenter]
      }
   } else {
      set firstproc $dzoom($dzoomwin,process,from)
      set lastproc $dzoom($dzoomwin,process,to)
      if {$firstproc>$lastproc || $firstproc < 0 || \
	    $lastproc > $setting($id,numProcs)-1} {
	 InvalidProcRange $firstproc $lastproc
      } else {
	 set toppt [expr {$firstproc * $procWidth}]
	 set bottompt [expr {($lastproc + 1) * $procWidth}]
	 # set the point relative to the top that we want at the
	 # new top of the visible region

	 set scrollInfo [lindex [$setting($id,tlc) config -scrollregion] 4]
	 set canvasleft   [lindex $scrollInfo 0]
	 set canvastop    [lindex $scrollInfo 1]
	 set canvasright  [lindex $scrollInfo 2]
	 set canvasbottom [lindex $scrollInfo 3]
	 set height [winfo height $setting($id,tlc)]

	 set toppt [expr {$firstproc*1.0/$setting($id,numProcs) * \
	       ($canvasbottom - $canvastop) + $canvastop}]
	 set bottompt [expr {($lastproc+1)*1.0/$setting($id,numProcs) * \
	       ($canvasbottom - $canvastop) + $canvastop}]
	 # puts "top = $toppt\nbottom = $bottompt"

	 set setting($id,mark,y) $toppt
	 # set zoom point to the top

	 set factor [expr {$height*1.0/($bottompt-$toppt)}]
	 # puts "zoom factor = $factor"
	 Zoom $id y $factor

	 set offset [expr {-1.0*$toppt}]
 	 # puts "Moving $offset vertically"

	 $setting($id,tlc) move all 0 $offset
	 set scrollInfo [lindex [$setting($id,tlc) config -scrollregion] 4]
	 set canvasleft   [lindex $scrollInfo 0]
	 set canvastop    [lindex $scrollInfo 1]
	 set canvasright  [lindex $scrollInfo 2]
	 set canvasbottom [lindex $scrollInfo 3]
	 $setting($id,tlc) config -scrollregion [list \
	       $canvasleft [expr $canvastop+$offset] \
	       $canvasright [expr $canvasbottom+$offset] ]

	 $setting($id,pnc) move all 0 $offset
	 set scrollInfo [lindex [$setting($id,pnc) config -scrollregion] 4]
	 set canvasleft   [lindex $scrollInfo 0]
	 set canvastop    [lindex $scrollInfo 1]
	 set canvasright  [lindex $scrollInfo 2]
	 set canvasbottom [lindex $scrollInfo 3]
	 $setting($id,pnc) config -scrollregion [list \
	       $canvasleft [expr $canvastop+$offset] \
	       $canvasright [expr $canvasbottom+$offset] ]

	 # move the canvases so that the point we want at the top
	 # is in place
      }
   }

   DetailedZoomCancel $id $dzoomwin
}



proc DetailedZoomCancel {id dzoomwin} {
   global dzoom

   UpdateDefaults [list zoomaxis $dzoom($dzoomwin,axis) \
	 hzoomtype $dzoom($dzoomwin,hzoomtype)]
   EraseArrayElements dzoom $dzoomwin
   destroy $dzoomwin
}


proc MoveTime {id timeoffset} {
   global setting

   set scrollInfo [lindex [$setting($id,tlc) config -scrollregion] 4]

   set canvasleft   [lindex $scrollInfo 0]
   set canvastop    [lindex $scrollInfo 1]
   set canvasright  [lindex $scrollInfo 2]
   set canvasbottom [lindex $scrollInfo 3]

   set pixeloffset [expr {($timeoffset*1000000.0)/($setting($id,lastTime)- \
	 $setting($id,firstTime))*($canvasright-$canvasleft)}]

   # puts "shifting $pixeloffset pixels"

   # puts "$setting($id,tlc) move all $pixeloffset 0"
   $setting($id,tlc) move all $pixeloffset 0
   $setting($id,tlc) config -scrollregion [list \
	 [expr $canvasleft+$pixeloffset] \
	 $canvastop \
	 [expr $canvasright+$pixeloffset] \
	 $canvasbottom]
}


proc ZoomOld {id dir {factor 2.0}} {
   # read in the direction to zoom in:  x, y or reset

   global setting

   if $setting($id,amzooming) return

   set setting($id,amzooming) 1
   foreach button $setting($id,buttons) {
      $button configure -state disabled
   }

   set c $setting($id,tlc)
   # time line canvas
   set p $setting($id,pnc)
   # proc num canvas

   # get scroll region info
   set scrollInfo [lindex [$c config -scrollregion] 4]
   for {set i 0} {$i<4} {incr i} {
      set [lindex {start(x) start(y) end(x) end(y)} $i] [lindex $scrollInfo $i]
   }
   # set coords of the viewable portion of the time line canvas

   # puts "scrollregion: $start(x) $start(y) $end(x) $end(y)"

   set width   [winfo width  $c]
   set height  [winfo height $c]

   if {$dir=="reset"} {
      set pwidth  [winfo width  $p]
      set pheight [winfo height $p]

      $c move all [expr -$start(x)] [expr -$start(y)]
      $p move all 0 [expr -$start(y)]
      $c scale all 0 0 \
	    [expr "($width +0.0)/($end(x)-$start(x))"] \
	    [expr "($height+0.0)/($end(y)-$start(y))"]	    
      $p scale all 0 0 1 \
	    [expr "($height+0.0)/($end(y)-$start(y))"]	    
      $c config -scrollregion [list 0 0 $width $height]
      $p config -scrollregion [list 0 0 $pwidth $pheight]
      # fix arrow disfiguration
      foreach messageArrow [$c find withtag message] {
	 eval $c coords $messageArrow [$c coords $messageArrow]
      }
      foreach button $setting($id,buttons) {
	 $button configure -state normal
      }
      set setting($id,amzooming) 0
      return
   }

   $c config -cursor watch
   update
   # update to get the cursor to change

   # scroll timeline canvas
   set mark $setting($id,mark,$dir)
   # puts "zooming towards $setting($id,mark,x)"
   $c scale all $setting($id,mark,x) $setting($id,mark,y) \
	 [expr ("$dir"=="x")?$factor:1.0] \
	 [expr ("$dir"=="y")?$factor:1.0]

   # recalculate the region of the canvas that can be scrolled through
   set start($dir) [expr $mark-$factor*($mark-$start($dir))]
   set end($dir)   [expr $mark-$factor*($mark-$end($dir))]
   $c config -scrollregion [list $start(x) $start(y) \
	 $end(x) $end(y)]

   # scale process numbers
   set scrollInfo [lindex [$p config -scrollregion] 4]
   set start(x) [lindex $scrollInfo 0]
   set end(x)   [lindex $scrollInfo 2]
   $p scale moveable 0 $setting($id,mark,y) 1 [expr ("$dir"=="y")?$factor:1.0]
   $p config -scrollregion [list $start(x) $start(y) $end(x) $end(y)]

   # fix arrow disfiguration
   foreach messageArrow [$c find withtag message] {
      eval $c coords $messageArrow [$c coords $messageArrow]
   }

   $c config -cursor top_left_arrow
   foreach button $setting($id,buttons) {
      $button configure -state normal
   }
   set setting($id,amzooming) 0
}


proc InvalidProcRange {firstproc lastproc} {
   set w .[GetUniqueWindowID]

   toplevel $w
   wm title $w "Invalid Entry"
   message $w.m -text  "Bad process range:\n$firstproc - $lastproc" \
	 -justify center -aspect 400 -relief raised -borderwidth 2
   button $w.b -text "Cancel" -command "destroy $w"
   pack append $w $w.m {} $w.b {pady 10}
   return
}
