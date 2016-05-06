
# The names of the windows used in this procedure are highly
# dependent on the names given in mainwin.tcl.  Change these if
# you change those.


proc Print_Hist_Dialog {log w} {
   
   set pwin .[GetUniqueWindowID]
   toplevel $pwin
   wm title $pwin "Print Histogram"

   frame $pwin.printbtn_frame -relief groove -borderwidth 4
   button $pwin.printbtn_frame.btn -text "Print" \
	 -command "Print_Hist $log $w $pwin; destroy $pwin"
   # button $pwin.printbtn_frame.btn -text "Print" \
	 -command "Print_Hist $log $w $pwin"
   pack $pwin.printbtn_frame.btn -padx 4 -pady 4

   button $pwin.cancel -text "Cancel" \
	 -command "destroy $pwin"

   Print_Options $pwin.options
   pack $pwin.options -fill both -expand 1 -side right
   pack $pwin.printbtn_frame $pwin.cancel -expand 1 -padx 10 -pady 10

   update idletasks
   wm minsize $pwin [winfo reqwidth $pwin] [winfo reqheight $pwin]
   bind $pwin <Destroy> "Print_SaveOpts"
}



proc Print_Hist {log w pwin} {
   global win_info color hist printOpts

   LookBusy $pwin
   update

   set pc $pwin.pcanv
   canvas $pc
   if {$printOpts(colormode) == "mono"} {
      set fg $color(bw_fg)
      set bg $color(bw_bg)
   } else {
      set fg $color(color_fg)
      set bg $color(color_bg)
   }

   set fg black
   set bg white

   set bg_rect [$pc create rect 0 0 0 0 -outline $bg -fill $bg]

   # histogram canvas
   set canvas $w.r.c
   set width [winfo width $canvas]
   set hist_height [winfo height $canvas]
   set time_lbl $w.r.t

   set title_id [$pc create text [expr $width / 2] 5 -anchor n -fill $fg\
	 -text "`$hist($w,stateName)' state length distribution"]
   set y [lindex [$pc bbox $title_id] 3]

   CopyCanvas $canvas [list 0 0 [expr $width - 1] [expr $hist_height - 1]] \
	 $pc [list 1 [expr $y+1]] color_disp_bg
   $pc create rect 0 $y [expr $width + 1] [expr $y + $hist_height + 1] \
	 -outline $fg -fill ""

   set y [expr $y + $hist_height]

   # copy time_lbl
   set time_lbl_height [winfo height $time_lbl]
   $time_lbl copy $pc 1 [expr $y + 1]
   set y [expr $y + $time_lbl_height + 1]

   set units_id [$pc create text 5 [expr $y+5] -text "(in seconds)" \
	 -anchor nw -fill $fg]
   set y [lindex [$pc bbox $units_id] 3]

   set info_id [$pc create text 5 [expr $y+5] -anchor nw -fill $fg \
	 -text "$hist($w,vis_n) states of $hist($w,n) ($hist($w,percent))"]
   set y [lindex [$pc bbox $info_id] 3]

   $pc coords $bg_rect 0 0 [expr $width + 1] $y
   $pc config -height [expr $y+1] -width [expr $width + 2]

   global printOpts
   Print_Hist_Recolor $pc $log [expr {$printOpts(colormode) == "mono"}]

   Print_Canvas $log $pc 0 0 [expr $width + 1] \
	 [expr $y + 1]

   destroy $pc
   # pack $pc

   LookBored $pwin
}




proc Print_Hist_Recolor {canvas log bw} {
   global color

   set fg black
   set bg white
   $canvas config -bg white

   if {$bw} {
      # set fg $color(bw_fg)
      # set bg $color(bw_bg)

      # $canvas config -bg $color(bw_timelinebg)

      set n [$log nstatedefs]
      for {set i 0} {$i < $n} {incr i} {
	 set st_bitmap [lindex [$log getstatedef $i] 2]
	 $canvas itemconfig color_$i -stipple $st_bitmap -fill $fg
      }
   } else {
      # set fg $color(color_fg)
      # set bg $color(color_bg)
      # set timelinefg $color(color_timelinefg)

      # $canvas config -bg $color(color_timelinebg)

      set n [$log nstatedefs]
      for {set i 0} {$i < $n} {incr i} {
	 set st_color [lindex [$log getstatedef $i] 1]
	 $canvas itemconfig color_$i -stipple "" -fill $st_color
      }
   }
   $canvas itemconfig color_fg -stipple "" -fill $fg
   $canvas itemconfig color_bg -stipple "" -fill $bg
   
}
