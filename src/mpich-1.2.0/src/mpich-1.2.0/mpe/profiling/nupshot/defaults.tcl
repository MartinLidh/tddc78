#
# A system of simple default-picking for Upshot
#
# Be sure to set the variable 'defaultFile' before using this package.
# If it is not set, it will default to ~/.tcl_defaults
#
# Ed Karrels
# Argonne National Laboratory
#


if ![info exists package(defaults)] {
set package(defaults) yup


source $progdir/common.tcl


proc GetDefault {index default} {
   global defaults defaultFile

   if ![info exists defaultFile] {
      set defaultFile ~/.tcl_defaults
   }

   if ![info exists defaults($index)] {
      if [file readable $defaultFile] {
	 set fileHandle [open $defaultFile r]
	 while {[gets $fileHandle str]>=0} {

	    # skip blank lines
	    if {[llength $str] == 0} continue

	    #  skip comment lines
	    if {[string index $str 0] == "#"} continue

	    if {[llength $str] != 2} {
	       puts stderr "Format error in $defaultFile"
	       return $default
	    }
	    SplitList $str {readIndex readValue}
	    set defaults($readIndex) $readValue
	 }
	 close $fileHandle
      } else {
	 # puts "Cannot read $defaultFile"
	 return $default
      }	
      if [info exists defaults($index)] {
	 return $defaults($index)
      } else {
	 return $default
      }
   } else {
      return $defaults($index)
   }
}



proc UpdateDefaults {{newValueList {}}} {
      # use this function to update the list of defaults and write them
      # out to the file

   global defaults defaultFile

   if ![info exists defaultFile] {
      set defaultFile ~/.tcl_defaults
   }

   set numValues [expr [llength $newValueList]/2]
   for {set i 0} {$i<$numValues} {incr i} {
      set defaults([lindex $newValueList [expr $i*2]]) \
	    [lindex $newValueList [expr $i*2+1]]
   }

      # if we have read rights to the file, read it
   if [file readable $defaultFile] {
      set fileHandle [open $defaultFile r]
      while {[gets $fileHandle str]>=0} {

	 # skip blank lines
	 if {[llength $str] == 0} continue

	 #  skip comment lines
	 if {[string index $str 0] == "#"} continue

	 SplitList $str {readIndex readValue}
	 if ![info exists defaults($readIndex)] {
	    set defaults($readIndex) $readValue
	 }
      }
      close $fileHandle
   }

   if {[file exists $defaultFile] ? [file writable $defaultFile] : \
	 [file writable [file dirname $defaultFile]]} {
      set fileHandle [open $defaultFile w]
      foreach idx [array names defaults] {
	 puts $fileHandle [list $idx $defaults($idx)]
      }
      close $fileHandle
   }
}


}
# set package(defaults) yup
