#
# fileselect.tcl --
# simple file selector.
#
# Mario Jorge Silva			          msilva@cs.Berkeley.EDU
# University of California Berkeley                 Ph:    +1(510)642-8248
# Computer Science Division, 571 Evans Hall         Fax:   +1(510)642-5775
# Berkeley CA 94720                                 
# 
# Layout:
#
#  file:                  +----+
#  ____________________   | OK |
#                         +----+
#
#  +------------------+    Cancel
#  | ..               |S
#  | file1            |c
#  | file2            |r
#  |                  |b
#  | filen            |a
#  |                  |r
#  +------------------+
#  currrent-directory
#
# Copyright 1993 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.
#


# names starting with "fileselect" are reserved by this module
# no other names used.

# use the "option" command for further configuration

option add *Listbox*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
option add *Entry*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
option add *Label*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile


# this is the default proc  called when "OK" is pressed
# to indicate yours, give it as the first arg to "fileselect"

proc fileselect.default.cmd {f} {
  puts stderr "selected file $f"
}


# this is the default proc called when error is detected
# indicate your own pro as an argument to fileselect

proc fileselect.default.errorHandler {errorMessage} {
   puts stdout "error: $errorMessage"
   catch { cd ~ }
}

# this is the proc that creates the file selector box

proc fileselect {
   {cmd fileselect.default.cmd} 
   {purpose "Open file:"} 
   {w .fileSelectWindow} 
   {errorHandler fileselect.default.errorHandler}} {
      
   catch {destroy $w}
      
   toplevel $w
   grab $w
   wm title $w "Select File"
   global select_format
   
   # path independent names for the widgets
   global fileselect
   
   set fileselect(entry) $w.file.eframe.entry
   set fileselect(list) $w.file.sframe.list
   set fileselect(scroll) $w.file.sframe.scroll
   set fileselect(ok) $w.bframe.okframe.ok
   set fileselect(cancel) $w.bframe.cancel
   set fileselect(dirlabel) $w.file.dirlabel
   
   # widgets
   frame $w.file -bd 10 
   frame $w.bframe -bd 10
   pack append $w \
	 $w.bframe {right frame n} \
	 $w.file {left expand fill}
   
   frame $w.file.eframe
   frame $w.file.sframe
   label $w.file.dirlabel -anchor e -width 24 -text [pwd] 
   
   pack append $w.file \
	 $w.file.eframe {top frame w fillx} \
	 $w.file.sframe {top fill expand} \
	 $w.file.dirlabel {top frame w fillx}
   
   
   label $w.file.eframe.pattern_lbl -anchor w -width 24 -text "Pattern:"
   entry $w.file.eframe.pattern_entry -relief sunken 
   $w.file.eframe.pattern_entry insert 0 \
	 [GetDefault logfile_name_glob_pattern "*.alog *.log *.trf"]
   # $w.file.eframe.pattern_entry insert 0 \
	 [GetDefault logfile_name_glob_pattern "*.log"]
   
   label $w.file.eframe.label -anchor w -width 24 -text $purpose
   entry $w.file.eframe.entry -relief sunken 
   
   pack append $w.file.eframe \
	 $w.file.eframe.pattern_lbl {top expand frame w} \
	 $w.file.eframe.pattern_entry {top fillx frame w} \
	 $w.file.eframe.label {top expand frame w} \
	 $w.file.eframe.entry {top fillx frame w} 
   
   
   scrollbar $w.file.sframe.yscroll -relief sunken \
	 -command "$w.file.sframe.list yview"
   listbox $w.file.sframe.list -relief sunken \
	 -yscroll "$w.file.sframe.yscroll set" 
   
   pack append $w.file.sframe \
	 $w.file.sframe.yscroll {right filly} \
	 $w.file.sframe.list {left expand fill} 
   
   # buttons
   frame $w.bframe.okframe -borderwidth 2 -relief sunken
   
   button $w.bframe.okframe.ok -text OK -relief raised -padx 10 \
	 -command [list fileselect.ok.cmd $w $cmd $errorHandler]
   
   button $w.bframe.cancel -text cancel -relief raised -padx 10 \
	 -command "fileselect.cancel.cmd $w"
   
   label $w.bframe.format -text "File format:"

   set select_format($w) [GuessFormat ""]

   proc print_trace {name1 name2 op} {
      puts "trace: $name1 $name2 $op"
      set n [info level]
      for {set i 0} {$i < $n} {incr i} {
	 puts "   $i: [info level $i]"
      }
   }
   # trace variable select_format rwu print_trace


   set select_format($w) alog
   # puts "set select_format($w) [GuessFormat {}]"

   radiobutton $w.bframe.alog -text "Alog" -variable select_format($w) \
	 -value alog
   radiobutton $w.bframe.picl -text "Picl-1" -variable select_format($w) \
	 -value picl_1
   if [string compare $select_format($w) picl] {
      $w.bframe.alog select
   } else {
      $w.bframe.picl select
   }
   
   pack append $w.bframe.okframe $w.bframe.okframe.ok {padx 10 pady 10}
   
   pack append $w.bframe $w.bframe.okframe {expand padx 20 pady 20}\
	 $w.bframe.cancel {top} \
	 $w.bframe.format {top pady 15 frame w} \
	 $w.bframe.alog {top frame e} \
	 $w.bframe.picl {top frame e}
   
   LookBusy $w
   update
   
   foreach file [lsort [eval glob -nocomplain \
	 [$w.file.eframe.pattern_entry get]]] {
      $fileselect(list) insert end "$file [file size $file]"
   }
   
   $fileselect(list) insert end "../"
   foreach directory [lsort [eval glob -nocomplain */]] {
      $fileselect(list) insert end $directory
   }

   LookBored $w
   update

   wm minsize $w \
	 [expr "[winfo reqwidth  $w.bframe] + [winfo reqwidth  $w.file]"] \
	 [winfo reqheight $w.bframe]
   
   
   # Set up bindings for the browser.
   bind $fileselect(entry) <Return> {eval $fileselect(ok) invoke}
   bind $fileselect(entry) <Control-c> {eval $fileselect(cancel) invoke}
   
   bind $w <Control-c> {eval $fileselect(cancel) invoke}
   bind $w <Return> {eval $fileselect(ok) invoke}
   
   
   tk_listboxSingleSelect $fileselect(list)
   
   
   bind $fileselect(list) <Button-1> [format {
      # puts stderr "button 1 release"
      %%W select from [%%W nearest %%y]
      $fileselect(entry) delete 0 end
      $fileselect(entry) insert 0 [lindex [%%W get [%%W nearest %%y]] 0]
      set select_format(%s) [GuessFormat [$fileselect(entry) get] ]
   } $w]
      
   bind $fileselect(list) <Key> {
      %W select from [%W nearest %y]
      $fileselect(entry) delete 0 end
      $fileselect(entry) insert 0 [lindex [%W get [%W nearest %y]] 0]
   }
   
   bind $fileselect(list) <Double-ButtonPress-1> {
      # puts stderr "double button 1"
      %W select from [%W nearest %y]
      $fileselect(entry) delete 0 end
      $fileselect(entry) insert 0 [lindex [%W get [%W nearest %y]] 0]
      $fileselect(ok) invoke
   }
   
   bind $fileselect(list) <Return> {
      %W select from [%W nearest %y]
      $fileselect(entry) delete 0 end
      $fileselect(entry) insert 0 [lindex [%W get [%W nearest %y]] 0]
      $fileselect(ok) invoke
   }
   
   # set kbd focus to entry widget
   
   focus $fileselect(entry)
   
}


# auxiliary button procedures

proc fileselect.cancel.cmd {w} {
   # puts stderr "Cancel"
   global select_format
   UpdateDefaults [list logfile_name_glob_pattern \
	 [$w.file.eframe.pattern_entry get]]

   destroy $w
}

proc fileselect.ok.cmd {w cmd errorHandler} {
    global fileselect select_format

    set selected [$fileselect(entry) get]

    # some nasty file names may cause "file isdirectory" to return an error
    set sts [catch { 
	file isdirectory $selected
    }  errorMessage ]

    if { $sts != 0 } then {
	$errorHandler $errorMessage
	destroy $w
	return

    }

    # clean the text entry and prepare the list
    $fileselect(entry) delete 0 end
    $fileselect(list) delete 0 end


    if {[string length $selected] && [file isdirectory $selected] != 0} {
       cd $selected
       set dir [pwd]
       $fileselect(dirlabel) configure -text $dir

       LookBusy $w
       update

       foreach file [lsort [eval glob -nocomplain \
	     [$w.file.eframe.pattern_entry get]]] {
	  $fileselect(list) insert end "$file [file size $file]"
       }

       $fileselect(list) insert end "../"
       foreach directory [lsort [glob -nocomplain */]] {
	  $fileselect(list) insert end $directory
       }

       LookBored $w
       update
       
       return
    }

    UpdateDefaults [list logfile_name_glob_pattern \
	  [$w.file.eframe.pattern_entry get] logfileformat $select_format($w) \
	  logfile $selected ]

    destroy $w

    eval $cmd $selected $select_format($w)

    if {[info exists select_format($w)]} {
       unset select_format($w)
    }
}
