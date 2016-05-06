#
# Trim the items in a canvas to a given rectangle.
# Right now it just trims rectangle and lines of any angle.
#
# trim_canvas canvas left top right bottom
#   trim the items in the given canvas to the given range,
#   return a special list of 'untrim' data for restoring the items
#
# untrim_canvas canvas untrim_data
#   restore the items in a trimmed canvas
#
#
# Dependencies:
#   SplitList
#
# Ed Karrels
# Argonne National Laboratory
#



if ![info exists package(trim_canvas)] {


proc trim_canvas {canvas left top right bottom} {

   # $canvas create rect $left $top $right $bottom -outline yellow

   # get the IDs of anything that lays on the border
   set lside [$canvas find overlapping $left $top $left $bottom]
   set bside [$canvas find overlapping $left $bottom $right $bottom]
   set rside [$canvas find overlapping $right $top $right $bottom]
   set tside [$canvas find overlapping $left $top $right $top]

   # puts "lside: $lside\nbside: $bside\nrside: $rside\ntside: $tside\n"
   set ids [U $lside [U $bside [U $rside $tside]]]

   # puts "overflowing legend lines: $ids"
   set trimList {}
   set trimthis 0
   foreach id $ids {
      # puts "$id type: [$canvas type $id]"
      # if the object is a state rectangle...
      if {[$canvas type $id]=="rectangle"} {
	 set coords [$canvas coords $id]
	 #puts "coords = [$canvas coords $id]"
	 SplitList $coords {rleft rtop rright rbottom}

	 if {($rleft<$left)||($rtop<$top)||($rright>$right)||($rbottom>$bottom)} {
	    lappend trimList [list $id $rleft $rtop $rright $rbottom]
	    # puts "Trimmed $id from ($rleft $rtop $rright $rbottom)"
	    if {$rleft<$left} {
	       set rleft $left
	    }
	    if {$rtop<$top} {
	       set rtop $top
	    }
	    if {$rright>$right} {
	       set rright $right
	    }
	    if {$rbottom>$bottom} {
	       set rbottom $bottom
	    }
	    # puts "     to ($rleft $rtop $rright $rbottom)"
	    $canvas coords $id $rleft $rtop $rright $rbottom
	 }
      } elseif {[$canvas type $id]=="line"} {
	 set coords [$canvas coords $id]
	 SplitList $coords {x1 y1 x2 y2}
	 SplitList $coords {l t r b}

	 set trimmed 0

	 if {$x1 == $x2} {
	    # vertical line
	    if [trim_canvas(trim_points) y1 y2 $top $bottom] {
	       set trimmed 1
	    }
	 } elseif {$t == $b} {
	    # horizontal line
	    if [trim_canvas(trim_points) x1 x2 $left $right] {
	       set trimmed 1
	    }
	 } else {
	    # diagonal line

	    # must be enclosed in "" so everyone gets evaluated
	    if [trim_canvas(trim_line) x1 y1 x2 y2 $left $right $top $bottom] {
	       set trimmed 1
	    }
	 }
	  
	 if {$trimmed} {
	    lappend trimList [list $id $l $t $r $b]
	    $canvas coords $id $x1 $y1 $x2 $y2
	 }

      }
   }
   #puts "trim list: $trimList"
   return $trimList
}


proc trim_canvas(trim_points) {a_var b_var left right} {
   set changed 0
   foreach c "$a_var $b_var" {
      upvar $c x
      if {$x < $left} {
	 set x $left
	 set changed 1
      } elseif {$x > $right} {
	 set x $right
	 set changed 1
      }
   }
   return $changed
}


proc trim_canvas(trim_line) {x1_var y1_var x2_var y2_var xmin xmax ymin ymax} {
   upvar $x1_var x1
   upvar $y1_var y1
   upvar $x2_var x2
   upvar $y2_var y2

   return [expr \
	 [trim_canvas(trim_line1) x1 y1 $x2 $y2 $xmin $xmax] || \
	 [trim_canvas(trim_line1) y1 x1 $y2 $x2 $ymin $ymax] || \
	 [trim_canvas(trim_line1) x2 y2 $x1 $y1 $xmin $xmax] || \
	 [trim_canvas(trim_line1) y2 x2 $y1 $x1 $ymin $ymax]]
}


proc trim_canvas(trim_line1) {x1_var y1_var x2 y2 xmin xmax} {
   upvar $x1_var x1
   upvar $y1_var y1

   if {$x1 < $xmin} {
      set y1 [trim_canvas(trim_line2) x1 y1 $x2 $y2 $xmin]
      set x1 $xmin
      return 1
   } elseif {$x1 > $xmax} {
      set y1 [trim_canvas(trim_line2) x1 y1 $x2 $y2 $xmax]
      set x1 $xmax
      return 1
   }
   return 0
}

proc trim_canvas(trim_line2) {x1_var y1_var x2 y2 x} {
   upvar $x1_var x1
   upvar $y1_var y1

   return [expr $y2 + ($x-$x2) * ($y1-$y2) / ($x1-$x2)]
}



proc untrim_canvas {canvas trimList} {
   foreach rect $trimList {
      eval $canvas coords $rect
   }
}

}

# package(trim_canvas)




