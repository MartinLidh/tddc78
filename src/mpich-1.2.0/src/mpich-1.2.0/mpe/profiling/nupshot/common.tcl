 
# Here are a bunch of common Tcl routines that I use.
# I don't want to try auto_load yet, since that will probably be more
# of a pain to install cleanly.
#
#  LookBusy win
#    Change the cursor to a watch, starting at the given window on down
#    the hierarchy tree.

#  LookBored win
#    Change the cursor to a top_left cursor (normal), starting at the
#    given window on down the hierarchy tree.

#  GetDefault index default
#    Look in my private default database for an item with the given
#    index.  If the given index is not found, return <default>.
#    Will look in the file 'defaultFile' (global variable)

#  and a bunch of others I don't have time to document right now...
# 
# Ed Karrels
# Argonne National Laboratory



if ![info exists package(common)] {
set package(common) yup


proc LookBusy {win} {
   $win config -cursor watch
   foreach child [winfo children $win] {
      LookBusy $child
   }
}


   
proc LookBored {win} {
   $win config -cursor top_left_arrow
   foreach child [winfo children $win] {
      LookBored $child
   }
}



proc GetUniqueWindowID {} {
   global lastWindowID

   if ![info exists lastWindowID] {
      set lastWindowID 0
   } else {
      set lastWindowID [expr $lastWindowID+1]
   }

   return $lastWindowID
}



proc EraseArrayElements {array_name idx_header} {
   upvar $array_name a

   set pattern "^$idx_header,"
   foreach idx [array names a] {
      if [regexp $pattern $idx] {
	 unset a($idx)
      }
   }
}



proc GetTextSize {canvas string} {
   set id [$canvas create text 0 0 -text $string -anchor nw]
   set sz0 [$canvas bbox $id]
   $canvas delete $id

   set id [$canvas create text 0 0 -text "WW\nWW" -anchor nw]
   set sz1 [$canvas bbox $id]
   $canvas delete $id

   # return list: {width, height}
   #set width [expr [lindex $sz1 2]-[lindex $sz0 2]]
   set width [lindex $sz0 2]   
   set height [expr [lindex $sz1 3]-[lindex $sz0 3]]
   return [list $width $height]
}



proc SplitList {l vars} {
   set i 0
   if {[llength $l] != [llength $vars]} {
      error "unequal list lengths"
      return
   }
   foreach element $l {
      uplevel "set [lindex $vars $i] {$element}"
      incr i
   }
}



proc minimum {nums} {
   set first 1
   set min 0
   foreach num $nums {
      if $first {
	 set min $num
	 set first 0
      } else {
	 if {$num < $min} {
	    set min $num
	 }
      }
   }
   return $min
}


proc maximum {nums} {
   set first 1
   set max 0
   foreach num $nums {
      if $first {
	 set max $num
	 set first 0
      } else {
	 if {$num > $max} {
	    set max $num
	 }
      }
   }
   return $max
}



proc U {lista listb} {
   if {[llength $lista]<[llength $listb]} {
      set n [llength $listb]
      for {set i 0} {$i<$n} {incr i} {
	 set el [lindex $listb $i]
	 if {[lsearch $lista $el]==-1} {
	    lappend lista $el
	 }
      }
      return $lista
   } else {
      set n [llength $lista]
      for {set i 0} {$i<$n} {incr i} {
	 set el [lindex $lista $i]
	 if {[lsearch $listb $el]==-1} {
	    lappend listb $el
	 }
      }
      return $listb
   }
}


proc N {lista listb} {
   set list {}
   if {[llength $lista]<[llength $listb]} {
      set n [llength $listb]
      for {set i 0} {$i<$n} {incr i} {
	 set el [lindex $listb $i]
	 if {[lsearch $lista $el]!=-1} {
	    lappend list $el
	 }
      }
   } else {
      set n [llength $lista]
      for {set i 0} {$i<$n} {incr i} {
	 set el [lindex $lista $i]
	 if {[lsearch $listb $el]!=-1} {
	    lappend list $el
	 }
      }
   }
   return $list
}


proc in_window {win x y} {
   set rootx [winfo rootx $win]
   set rooty [winfo rooty $win]
   if [expr $x >= $rootx && $x < $rootx + [winfo width $win] && \
	 $y >= $rooty && $y < $rooty + [winfo height $win]] {
      return 1
   } else {
      return 0
   }
}


proc swap {av bv} {
   upvar $av a
   upvar $bv b
   set c $a
   set a $b
   set b $c
}



proc is_int {x} {
   if [catch "expr int($x)" result] {
      return 0;
   } elseif [expr $x == int($x)] {
      return 1;
   } else {
      return 0;
   }
}


}
# package(common)
