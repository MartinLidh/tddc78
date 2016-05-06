#
# A system of picking colors simply for Upshot
#
# Ed Karrels
# Argonne National Laboratory
#


# only include this source code once

if ![info exists package(colors)] {
set package(colors) yup

source $progdir/defaults.tcl

# if no one else cares, set #colors automatically
if {![info exists bw]} {set bw [expr \
      "[winfo depth .] < 4"]}

set color(bw_fg)         [GetDefault bw_fg black]
set color(bw_bg)         [GetDefault bw_bg white]
set color(bw_activefg)   [GetDefault bw_activefg white]
set color(bw_activebg)   [GetDefault bw_activebg black]
set color(bw_arrowfg)    [GetDefault bw_arrowfg black]
set color(bw_menubar)    [GetDefault bw_menubar white]
set color(bw_disp_bg)    [GetDefault bw_disp_bg white]
set color(bw_timeline)   [GetDefault bw_timeline black]

set color(color_fg)         [GetDefault color_fg white]
set color(color_bg)         [GetDefault color_bg SteelBlue]
set color(color_activefg)   [GetDefault color_activefg white]
set color(color_activebg)   [GetDefault color_activebg SteelBlue2]
set color(color_arrowfg)    [GetDefault color_arrowfg white]
set color(color_menubar)    [GetDefault color_menubar SteelBlue4]
set color(color_disp_bg)    [GetDefault color_disp_bg black]
set color(color_timeline)   [GetDefault color_timeline red]

if {$bw} {
   set color(fg)         $color(bw_fg)
   set color(bg)         $color(bw_bg)
   set color(activefg)   $color(bw_activefg)
   set color(activebg)   $color(bw_activebg)
   set color(arrowfg)    $color(bw_arrowfg)
   set color(menubar)    $color(bw_menubar)
   set color(disp_bg)    $color(bw_disp_bg)
   set color(timeline)   $color(bw_timeline)
   
   option add *foreground $color(fg) 30
   option add *background $color(bg) 30
   option add *selectForeground white 30
   option add *selectBackground black 30
   option add *activeForeground white 30
   option add *activeBackground $color(activebg) 30
   
} else {
   set color(fg)         $color(color_fg)
   set color(bg)         $color(color_bg)
   set color(activefg)   $color(color_activefg)
   set color(activebg)   $color(color_activebg)
   set color(arrowfg)    $color(color_arrowfg)
   set color(menubar)    $color(color_menubar)
   set color(disp_bg)    $color(color_disp_bg)
   set color(timeline)   $color(color_timeline)
   
   # Why doesn't GhostView like 'White' ?
   
   option add *foreground $color(fg) 30
   option add *background $color(bg) 30
   option add *activeForeground $color(activefg) 30
   option add *activeBackground $color(activebg) 30
}

}
# set package(colors) yup
