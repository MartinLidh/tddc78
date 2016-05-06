#!/usr/local/tcl/bin/wish -f

proc CopyCanvasAdj {coords_var xoffset yoffset} {
   upvar $coords_var coords

   set new_coords {}
   set ncoords [llength $coords]
   for {set i 0} {$i < $ncoords} {incr i 2} {
      set x [lindex $coords $i]
      set y [lindex $coords [expr $i + 1]]
      lappend new_coords [expr $x + $xoffset]
      lappend new_coords [expr $y + $yoffset]
   }
   set coords $new_coords
}



proc CopyCanvas {from_c bbox to_c coord bgtag} {
   set xoffset [expr [lindex $coord 0] - [lindex $bbox 0]]
   set yoffset [expr [lindex $coord 1] - [lindex $bbox 1]]

   set bg [lindex [$from_c config -bg] 4]
   set bg_coords $bbox
   CopyCanvasAdj bg_coords $xoffset $yoffset
   eval $to_c create rect $bg_coords -fill $bg -outline {""} \
	 -tags {$bgtag}

      # get the ids of all the objects to be copied
   eval set ids "\[$from_c find overlap $bbox]"

      # copy each item
   foreach item $ids {
         # get the item type, like "rect" or "oval"
      set type [$from_c type $item]
         # get the tags for the item
      set tags [$from_c gettags $item]
         # get the list of coordinate that define the object
      set coords [$from_c coords $item]
         # adjust the coordinates for the new window
      CopyCanvasAdj coords $xoffset $yoffset
         # get the option list
      set options [$from_c itemconfigure $item]
      if {$tags == ""} {
	 set tagList ""
      } else {
	 set tagList "-tags {$tags}"
      }
      set optionList {}
      foreach option $options {
	    # only copy over options that are not blank
	 if {[lindex $option 4] != ""} {
	    lappend optionList [lindex $option 0] [lindex $option 4]
	 }
      }

      if {$type == "window"} {
	 CopyCanvasWindow $from_c $item $to_c $coords $tagList $optionList
      } else {

	 # create the object
	 set id [eval "$to_c create $type $coords $optionList"]
	 # puts "Create $type $coords $optionList: $id"
      }
   }
}


proc CopyCanvasAnchorAdj {canvas item left_var top_var \
      right_var bottom_var} {
   upvar $left_var left
   upvar $top_var top
   upvar $right_var right
   upvar $bottom_var bottom

   set width [expr $right - $left]
   set height [expr $bottom - $top]

   set anchor [lindex [$canvas itemconfig $item -anchor] 4]

   if {$anchor == "center" || \
	 $anchor == "n" || \
	 $anchor == "s"} {
      set left [expr $left - $width / 2]
      set right [expr $right - $width / 2]
   } elseif {$anchor == "e" || \
	 $anchor == "ne" || \
	 $anchor == "se"} {
      set left [expr $left - $width]
      set right [expr $right - $width]
   }
   if {$anchor == "w" || \
	 $anchor == "center" || \
	 $anchor == "e"} {
      set top [expr $top - $height / 2]
      set bottom [expr $bottom - $height / 2]
   } elseif {$anchor == "sw" || \
	 $anchor == "s" || \
	 $anchor == "se"} {
      set top [expr $top - $height]
      set bottom [expr $bottom - $height]
   }
}


proc CopyCanvasWindow {from_c item to_c coords tagList options} {
   set get_window 0
   set window 0
   foreach option $options {
      if {$get_window} {
	 set window $option
	 break
      }
      if {$option == "-window"} {
	 set get_window 1
      }
   }

   set left [lindex $coords 0]
   set top  [lindex $coords 1]

   if {$window != "0"} {
      set class [winfo class $window]
      set width [winfo width $window]
      set height [winfo height $window]
      set right [expr $left + $width]
      set bottom [expr $top + $height]
      if {$class == "Button"} {
	 set name [lindex [$window config -text] 4]
	 # set font [lindex [$window config -font] 4]
	 set fg [lindex [$window config -fg] 4]
	 set bg [lindex [$window config -bg] 4]
	 CopyCanvasAnchorAdj $from_c $item left top right bottom
	 set x [expr ($left + $right)/2]
	 set y [expr ($top + $bottom)/2]
	 # $to_c create rect $left $top $right $bottom -fill $bg \
	       -outline $fg
	 eval $to_c create text $x $y -text {$name} \
	       -fill {$fg} $tagList
	    # -font {$font} 

	 return
      }
   } else {
      set right [expr $left + 10]
      set bottom [expr $top + 10]
   }

   $to_c create rect $left $top $right $bottom
}


if 0 {
canvas .c1 -background green -width 400 -height 400
canvas .c2 -height 200 -width 200
pack .c1 .c2

.c2 create rect 10 10 30 30 -fill blue
.c2 create oval 50 60 100 120 -fill red -outline blue
.c2 create text 100 100 -text {Yo Mon} -font 7x13bold -fill skyblue
.c2 create line 190 10 10 190 -arrow last -fill red
.c2 create poly 10 10 20 20 30 10 40 20 50 50 -fill yellow
button .c2.hey -text Hey
.c2 create window 120 130 -window .c2.hey

button .copy -text "Copy" -command \
      {CopyCanvas .c2 {0 0 200 200} .c1 {0 0}}
button .close -text "Close" -command {exit}
pack .copy .close
}
