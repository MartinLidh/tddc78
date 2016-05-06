
# The names of the windows used in this procedure are highly
# dependent on the names given in mainwin.tcl.  Change these if
# you change those.


proc Print_MainWin_Dialog {log id} {
   
   set pwin .[GetUniqueWindowID]
   toplevel $pwin
   wm title $pwin "Print Logfile Display"

   frame $pwin.printbtn_frame -relief groove -borderwidth 4
   button $pwin.printbtn_frame.btn -text "Print" \
	 -command "Print_MainWin $log $id $pwin"
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



proc Print_MainWin {log id pwin} {
   global mainwin color printOpts procnums

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

   # set fg black
   # set bg white

   set title_y 20
   set bg_rect [$pc create rect 0 0 0 0 -outline "" -fill $bg]

   set legend_height [Legend_Squeeze $id.legend]
   # set left_gap [GetDefault display_left_gap 40]
   set left_gap $mainwin($id,procWidth)
   set y [expr $legend_height + 2]
   set y [ expr $y + $title_y ]
   set display_width [winfo width $id.displays]
   set display_list [$id.displays list]
   foreach disp $display_list {
      set this_height [winfo height $disp]
         # tell each display to copy itself over
      $disp copy $pc 0 $y
         # scrollbars are 19 pixels wide -- thats the gap on the right
         # create border for each display
      $pc create rect $left_gap $y [expr $display_width - 19] \
	    [expr $y + $this_height + 1] -outline $fg -tags color_outline \
	    -fill ""

      incr y [expr 1 + $this_height]
   }

      # Copy logfile title
    # Make a canvas containing the data
    $pc create text $left_gap $title_y -text "Logfile Title:" \
            -anchor sw -fill "white" -tags "label"
    set bbox [$pc bbox "label"]
    set label_width [ expr [ lindex $bbox 2 ] - [ lindex $bbox 0 ] ]
    $pc create text [ expr $left_gap + $label_width + 4 ] $title_y \
            -text [ $id.titlelegend.val get 1.0 end ] \
	    -anchor sw -fill "white"

      # copy legend
      # legend might have to resize and readjust, will return the temporary
      # height needed

   set legend_y 1
   set legend_y $title_y
   Legend_Copy $id.legend $pc [expr $left_gap + 1] $legend_y
      # create border for legend
   $pc create rect $left_gap $title_y [expr $display_width - 19] \
	 [expr $legend_height + 2 + $title_y] \
	 -outline $fg -fill ""
      # the legend was squeezed to fit between the left and right gaps;
      # unqueeze it
   Legend_Unsqueeze $id.legend

   set time_lbl_height [winfo height $id.bottom.tlbl]


   # copy over time_lbl
   
   # $pc create rect 0 $y [expr $display_width + 3] \
	 [expr $time_lbl_height + $y + 3] -fill $bg -outline ""
      # the time_lbl will copy a little more or less so that the
      # labels at the far right and far left are chopped in the right way
   $id.bottom.tlbl copy $pc $left_gap [expr $y + 1]
      # put line between time_lbl and the displays
   $pc create line $left_gap $y [expr $display_width - 19] $y -fill $fg

   $pc coords $bg_rect 0 0 [expr $display_width + 3] \
	 [expr $y + $time_lbl_height + 3]
   $pc create rect 0 0 [expr $display_width + 3] \
	 [expr $y + $time_lbl_height + 3] -fill "" -outline $fg


   Print_Mainwin_Recolor $pc $log $printOpts(colormode)

   if [catch "Print_Canvas $log $pc 0 0 [expr $display_width + 4] \
	 [expr $y + $time_lbl_height + 3]" result] {
      LookBored $pwin
      error $result
   }

   set preview 0
   if $preview {
      $pc config -width [expr $display_width + 4] \
	    -height [expr $y + $time_lbl_height + 4]
      pack $pc
      LookBored $pwin
   } else {
      destroy $pwin
   }
}


#
# Recolor canvas items based on color_ tags.
#
# colormode - mono, gray, or color
#
# Tags:
#  color_%d - %d is the state number.  If printing in color or gray, change the
#             fill color and remove stippling.  If printing in black&white,
#             change the stipple pattern and change the fill color to
#             $color(bg).
#  color_outline - change the outline of these items to $color(fg)
#  color_timeline - change the fill of these items to $color(timelinefg)
#  color_arrow - change the fill of these items to $color(arrowfg)
proc Print_Mainwin_Recolor {canvas log colormode} {
   global color

   # set fg black
   # set bg white
   # set timelinefg black
   # $canvas config -bg white

   if {$colormode == "mono"} {
      set fg $color(bw_fg)
      set bg $color(bw_bg)
      set timeline $color(bw_timeline)
      set disp_bg $color(bw_disp_bg)
      set arrowfg $color(bw_arrowfg)

      # $canvas config -bg $color(bw_disp_bg)

      set n [$log nstatedefs]
      for {set i 0} {$i < $n} {incr i} {
	 set st_bitmap [lindex [$log getstatedef $i] 2]
	 $canvas itemconfig color_$i -stipple $st_bitmap -fill $fg
      }
   } else {
      set fg $color(color_fg)
      set bg $color(color_bg)
      if {$colormode == "gray"} {
	 set timeline $color(color_fg)
      } else {
	 set timeline $color(color_timeline)
      }
      set disp_bg $color(color_disp_bg)
      set arrowfg $color(color_arrowfg)

      # $canvas config -bg $color(color_disp_bg)

      set n [$log nstatedefs]
      for {set i 0} {$i < $n} {incr i} {
	 set st_color [lindex [$log getstatedef $i] 1]
	 $canvas itemconfig color_$i -stipple "" -fill $st_color
      }
   }
   $canvas itemconfig color_fg -stipple "" -fill $fg
   $canvas itemconfig color_bg -stipple "" -fill $bg
   $canvas itemconfig color_disp_bg -stipple "" -fill $disp_bg
   $canvas itemconfig color_outline -outline $fg
   $canvas itemconfig color_timeline -fill $timeline
   $canvas itemconfig color_arrow -fill $arrowfg
   
}

