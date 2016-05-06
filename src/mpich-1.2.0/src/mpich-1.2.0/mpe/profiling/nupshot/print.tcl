proc Print_Canvas {log canvas x y width height} {
   global printOpts

   # if for some reason the print options haven't been set,
   # set them to the defaults
   if ![info exists printOpts(set)] Print_GetOpts

   # calculate the page size.  Make sure to abide by the most restrictive of
   # pageHeight and pageWidth.

   if {($height+0.0)/$width < \
	 $printOpts(pageHeight)/$printOpts(pageWidth)} {
      # wide picture
      if $printOpts(isLandscape) {
	 set shortSize $printOpts(pageHeight)$printOpts(units)
      } else {
	 set shortSize $printOpts(pageWidth)$printOpts(units)
      }
      set pagesize "-pagewidth $shortSize"
   } else {
      # tall picture
      if $printOpts(isLandscape) {
	 set shortSize $printOpts(pageWidth)$printOpts(units)
      } else {
	 set shortSize $printOpts(pageHeight)$printOpts(units)
      }
      set pagesize "-pageheight $shortSize"
   }
   
   if {$printOpts(colormode) == "gray"} {
      Print_CreateGrayColormap $log colormap
      # set colormap() entries
      set colormapping "-colormap colormap"
   } else {
      set colormapping ""
   }

   set options "-x $x -width $width -rotate $printOpts(isLandscape)\
	 -y $y -height $height -colormode $printOpts(colormode)\
	 $pagesize $colormapping"

   if ($printOpts(tofile)) {
      eval $canvas postscript $options -file {$printOpts(filename)}
   } else {
      global programName
      exec lpr -C $programName -T {$setting(logfilename)} \
	    -P$printOpts(lprname) << [eval $canvas postscript $options]
   }
}


proc Print_CreateGrayColormap {log colormap_var} {
   upvar $colormap_var colormap

   set nstates [$log nstatedefs]
   set interval [expr {1.0 / ($nstates-1)}]
   set level 0.0

   for {set state_no 0} {$state_no < $nstates} {incr state_no} {
      set colormap([lindex [$log getstatedef $state_no] 1]) \
	    "$level $level $level setrgbcolor"
      set level [expr {$level + $interval}]
      if {$level > 1.0} {
	 set level 1.0
      }
   }
}


proc Print_GetOpts {} {
   global printOpts
   set printOpts(set) 1
   set printOpts(tofile)      [GetDefault print_tofile 1]
   global env
   if [info exists env(PRINTER)] {
      set printOpts(lprname) $env(PRINTER)
   } else {
      set printOpts(lprname)  [GetDefault print_lprname ""]
   }
   global progName
   set printOpts(filename)    [GetDefault print_file $progName.ps]
   set printOpts(colormode)   [GetDefault print_colormode color]
   set printOpts(isLandscape) [GetDefault print_isLandscape 1]
   set printOpts(pageWidth)   [GetDefault print_pageWidth 7.5]
   set printOpts(pageHeight)  [GetDefault print_pageHeight 10]
   set printOpts(units)       [GetDefault print_units i]
}



proc Print_SaveOpts {} {
   global printOpts
   UpdateDefaults [list \
	 print_tofile $printOpts(tofile) \
	 print_lprname $printOpts(lprname) \
	 print_file $printOpts(filename) \
	 print_colormode $printOpts(colormode) \
	 print_isLandscape $printOpts(isLandscape)\
	 print_pageWidth $printOpts(pageWidth) \
	 print_pageHeight $printOpts(pageHeight) \
	 print_units $printOpts(units) ]
}



proc Print_Options {w} {
   # w is the frame to create for the print options
   # item is the item being printed: mainwin or hist

   # Print_Options $w.options hist
   # pack $w.options

   global printOpts
      # printOpts:
      #    $parentWin,tofile (boolean)
      #    $parentWin,filename
      #    $parentWin,incolor (boolean)
      #    lprname

   if ![info exists printOpts(set)] {
      Print_GetOpts
   }

   set title $w.title
   frame $w
   frame $w.dev -relief ridge -borderwidth 2
   pack $w.dev -fill both -expand 1 -padx 5 -pady 5
   frame $w.dev.2
   pack $w.dev.2 -padx 3 -pady 3 -expand 1 -fill x
      set output_dev_lbl $w.dev.2.lbl
      frame $w.dev.2.p
      pack $w.dev.2.p -fill x -expand 1 -side bottom -pady 3
         set printer_button $w.dev.2.p.b
         set printer_entry $w.dev.2.p.e
      frame $w.dev.2.f
      pack $w.dev.2.f -fill x -expand 1 -side bottom -pady 3
         set file_button $w.dev.2.f.b
         set file_entry $w.dev.2.f.e
   frame $w.orient -relief ridge -borderwidth 2
   pack $w.orient -fill both -expand 1 -pady 5 -padx 5
   frame $w.orient.2
   pack $w.orient.2 -padx 3 -pady 3 -expand 1
      set orient_lbl $w.orient.2.l
      set orients_frame $w.orient.2.b
      frame $orients_frame
         set orient_portrait $w.orient.2.b.p
         set orient_landscape $w.orient.2.b.l
   frame $w.color -relief ridge -borderwidth 2
   pack $w.color -fill both -expand 1 -pady 5 -padx 5
   frame $w.color.2
   pack $w.color.2 -padx 3 -pady 3 -expand 1
      set color_lbl $w.color.2.l
      set colors_frame $w.color.2.b
      frame $colors_frame
         set color_bw $w.color.2.b.b
         set color_gray $w.color.2.b.g
         set color_color $w.color.2.b.c
   frame $w.size -relief ridge -borderwidth 2
   pack $w.size -fill both -expand 1 -pady 5 -padx 5
   frame $w.size.2
   pack $w.size.2 -padx 3 -pady 3 -expand 1 -fill x
      set size_lbl $w.size.2.lbl
      set size_dims $w.size.2.dims
      frame $size_dims
         set size_width_frame $size_dims.width
         frame $size_width_frame
            set size_width_lbl $size_width_frame.lbl
            set size_width_entry $size_width_frame.entry
         set size_height_frame $size_dims.height
         frame $size_height_frame
            set size_height_lbl $size_height_frame.lbl
            set size_height_entry $size_height_frame.entry
      set size_units $w.size.2.units
      frame $size_units
         set size_units_lbl $size_units.lbl
         set size_units_cm $size_units.cm
         set size_units_in $size_units.in
         set size_units_mm $size_units.mm
         set size_units_pt $size_units.pt


   # title
   label $title -text "Print Options"

   label $output_dev_lbl -text "Output form:"

   # printer selection
   radiobutton $printer_button \
	 -text Printer: \
	 -variable printOpts(tofile) \
	 -value 0
   entry $printer_entry \
	 -width 20 \
	 -relief sunken \
	 -textvariable printOpts(lprname)
   
   # file selection
   radiobutton $file_button \
	 -text File: \
	 -variable printOpts(tofile) \
	 -value 1
   entry $file_entry \
	 -width 20 \
	 -relief sunken \
	 -textvariable printOpts(filename)

   # orientation
   label $orient_lbl -text "Orientation: "
   radiobutton $orient_portrait \
	 -text "portrait (tall)" \
	 -variable printOpts(isLandscape) \
	 -value 0
   radiobutton $orient_landscape \
	 -text "landscape (wide)" \
	 -variable printOpts(isLandscape) \
	 -value 1
   # trace var printOpts(isLandscape) w \
	 "Print_Trace isLandscape $orient_portrait $orient_landscape" w

   label $color_lbl -text "Color mode: "
   radiobutton $color_bw \
	 -text "b&w" \
	 -variable printOpts(colormode) \
	 -value mono
   radiobutton $color_gray \
	 -text "grays" \
	 -variable printOpts(colormode) \
	 -value gray
   radiobutton $color_color \
	 -text "color" \
	 -variable printOpts(colormode) \
	 -value color

   label $size_lbl -text "Output size: "
   label $size_width_lbl -text "width: "
   entry $size_width_entry -textvariable printOpts(pageWidth)
   label $size_height_lbl -text "height: "
   entry $size_height_entry -textvariable printOpts(pageHeight)
   label $size_units_lbl -text "Units:"
   radiobutton $size_units_cm \
	 -text "cm" \
	 -variable printOpts(units) \
	 -value c
   radiobutton $size_units_in \
	 -text "inches" \
	 -variable printOpts(units) \
	 -value i
   radiobutton $size_units_mm \
	 -text "mm" \
	 -variable printOpts(units) \
	 -value m
   radiobutton $size_units_pt \
	 -text "points" \
	 -variable printOpts(units) \
	 -value p

   pack $output_dev_lbl
   pack $printer_button $file_button -side left -padx 3
   pack $printer_entry $file_entry -side right -anchor w -fill x -expand 1
   pack $orient_lbl -side left -anchor e
   pack $orients_frame -side left
   pack $orient_portrait $orient_landscape -anchor w
   pack $color_lbl -side left -anchor e
      pack $colors_frame -side left -fill x
   pack $color_bw $color_gray $color_color -anchor w
   
   pack $size_lbl -anchor w -fill both -expand 1
   pack $size_dims $size_units -side left -padx 4 -pady 4
   pack $size_width_frame -padx 3 -pady 3
   pack $size_width_lbl $size_width_entry -side left
   pack $size_height_frame -padx 3 -pady 3
   pack $size_height_lbl $size_height_entry -side left
   pack $size_units_lbl $size_units_cm $size_units_in $size_units_mm \
	 $size_units_pt -anchor w

   #update
   #set minwidth [expr {[winfo reqwidth $w.printer.btn]+\
	 [winfo reqwidth $w.printer.entry]}]
   #set minheight [expr {[winfo reqheight $w.printer] + \
	 [winfo reqheight $w.file] + [winfo reqheight $w.buttons] + \
	 $blackWhite?0:[winfo reqheight $w.hues]}]
   #wm minsize $w $minwidth $minheight
}

