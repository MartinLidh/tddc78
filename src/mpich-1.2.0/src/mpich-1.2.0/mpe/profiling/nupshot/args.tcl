#
# A system of switch-based simple argument parsing.
#
# Ed Karrels
# Argonne National Laboratory
#


if ![info exists package(args)] {
set package(args) yup


proc ArgOpt {arg_list_var opt dest_var} {
   set grab_next 0
   set found 0

   upvar $arg_list_var arg_list
   upvar $dest_var dest

   foreach arg $arg_list {
      if {$grab_next} {
	 set dest $arg
	 set grab_next 0
	 set found 1
      } elseif {"$arg" == "$opt"} {
	 set grab_next 1
      }
   }

   return $found
}

proc ArgPresent {arg_list_var opt} {
   upvar $arg_list_var arg_list

   foreach arg $arg_list {
      if {"$arg" == "$opt"} {
	 return 1
      }
   }

   return 0;
}


proc ArgBool {arg_list_var opt dest_var} {
   upvar $arg_list_var arg_list
   upvar $dest_var dest

   if [ArgOpt arg_list $opt temp] {
      if {$temp} {
	 set dest 1
      } else {
	 set dest 0
      }
   } else {
      return 0
   }
}

}
# set package(args) yup
