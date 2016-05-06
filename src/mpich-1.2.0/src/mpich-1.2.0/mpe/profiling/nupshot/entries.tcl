# Create bindings for entry widgets.
#
#  up arrow or ^-a beginning of entry
#  down arrow or ^-e end of entry
#  ^-k delete to end of entry
#  left arrow or ^-b left one character
#  right arrow or ^-f right one character
#  ^-d delete character right
#  M-d delete word right
#  M-b move word left
#  M-f move word right



proc EntryBindings {} {
   bind Entry <3> {
      %W insert insert [selection get]
   }
   bind Entry <Control-Key-e> {
      %W icursor end
   }
   bind Entry <Key-Down> {
      %W icursor end
   }
   bind Entry <Control-Key-a> {
      %W icursor 0
   }
   bind Entry <Key-Up> {
      %W icursor 0
   }
   bind Entry <Control-Key-k> {
      %W delete insert end
   }
   bind Entry <Key-Left> {
      %W icursor [expr [%W index insert]-1]
   }
   bind Entry <Key-Right> {
      %W icursor [expr [%W index insert]+1]
   }
   bind Entry <Control-Key-b> {
      %W icursor [expr [%W index insert]-1]
   }
   bind Entry <Control-Key-f> {
      %W icursor [expr [%W index insert]+1]
   }
   bind Entry <Control-Key-d> {
      %W delete insert
   }
   bind Entry <Control-Key-space> {
      %W select from insert
   }
   bind Entry <Key-Return> {
      EntryGotoNext %W
   }
   bind Entry <Key-Tab> {
      EntryGotoNext %W
   }
   bind Entry <Meta-Key-d> {
      set str [%W get]
      set idx [%W index insert]
      set i 0
      set char [string index $str $idx]
      while {![string match {[a-zA-Z0-9_]} $char] &&
             $idx<[string length $str]} {
	 incr i
	 incr idx
	 set char [string index $str $idx]
      }
      while {[string match {[a-zA-Z0-9_]} $char] &&
             $idx<[string length $str]} {
	 incr i
	 incr idx
	 set char [string index $str $idx]
      }
      for {set idx 0} {$idx < $i} {incr idx} {
	 %W delete insert
      }
   }

   bind Entry <Meta-Key-b> {
      set str [%W get]
      set idx [expr {[%W index insert]-1}]
      set i 0
      set char [string index $str $idx]
      while {![string match {[a-zA-Z0-9_]} $char] &&
             $idx>=0} {
	 incr i
	 incr idx -1
	 set char [string index $str $idx]
      }
      while {[string match {[a-zA-Z0-9_]} $char] &&
             $idx>=0} {
	 incr i
	 incr idx -1
	 set char [string index $str $idx]
      }
      %W icursor [expr {$idx + 1}]
   }

   bind Entry <Meta-Key-f> {
      set str [%W get]
      set idx [%W index insert]
      set i 0
      set char [string index $str $idx]
      while {![string match {[a-zA-Z0-9_]} $char] &&
             $idx<[string length $str]} {
	 incr i
	 incr idx
	 set char [string index $str $idx]
      }
      while {[string match {[a-zA-Z0-9_]} $char] &&
             $idx<[string length $str]} {
	 incr i
	 incr idx
	 set char [string index $str $idx]
      }
      %W icursor [expr [%W index insert]+$i]
   }

}

proc EntryNextInsertAfter {this prev} {
   global entrynext
   if {[info exists entrynext($prev)]} {
      set entrynext($this) $entrynext($prev)
      set entrynext($prev) $this
   } else {
      set entrynext($this) $this
   }
}


proc EntrySetNext {this next} {
   global entrynext
   set entrynext(this) next
}

proc EntryGetNext {this} {
   global entrynext
   if {[info exists entrynext($this)]} {
      return $entrynext($this)
   } else {
      return {}
   }
}

proc EntryGotoNext {w} {
   global entrynext
   if {[info exists entrynext($w)]} {
      focus $entrynext($w)
   }
}

