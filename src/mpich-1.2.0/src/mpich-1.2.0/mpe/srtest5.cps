%!PS-Adobe-3.0 EPSF-3.0
%%Creator: Tk Canvas Widget
%%For: Rusty Lusk,
%%Title: Window .3.pcanv
%%CreationDate: Mon Dec  9 22:33:30 1996
%%BoundingBox: 211 18 401 775
%%Pages: 1
%%DocumentData: Clean7Bit
%%Orientation: Landscape
%%DocumentNeededResources: font Helvetica-Bold
%%EndComments

% This file contains the standard Postscript prolog used when
% generating Postscript from canvas widgets.
%
% $Header: /home/MPI/cvsMaster/mpich/mpe/srtest5.cps,v 1.1.1.1 1997/09/17 20:38:26 gropp Exp $ SPRITE (Berkeley);

%%BeginProlog
50 dict begin

% The definitions below just define all of the variables used in
% any of the procedures here.  This is needed for obscure reasons
% explained on p. 716 of the Postscript manual (Section H.2.7,
% "Initializing Variables," in the section on Encapsulated Postscript).

/baseline 0 def
/stipimage 0 def
/height 0 def
/justify 0 def
/maxwidth 0 def
/spacing 0 def
/stipple 0 def
/strings 0 def
/xoffset 0 def
/yoffset 0 def
/tmpstip null def
/encoding {ISOLatin1Encoding} def

% Override setfont to automatically encode the font in the style defined by 
% by 'encoding' (ISO Latin1 by default).

systemdict /encodefont known {
    /realsetfont /setfont load def
    /setfont {
	encoding encodefont realsetfont
    } def
} if

% desiredSize EvenPixels closestSize
%
% The procedure below is used for stippling.  Given the optimal size
% of a dot in a stipple pattern in the current user coordinate system,
% compute the closest size that is an exact multiple of the device's
% pixel size.  This allows stipple patterns to be displayed without
% aliasing effects.

/EvenPixels {
    % Compute exact number of device pixels per stipple dot.
    dup 0 matrix currentmatrix dtransform
    dup mul exch dup mul add sqrt

    % Round to an integer, make sure the number is at least 1, and compute
    % user coord distance corresponding to this.
    dup round dup 1 lt {pop 1} if
    exch div mul
} bind def

% width height string filled StippleFill --
%
% Given a path and other graphics information already set up, this
% procedure will fill the current path in a stippled fashion.  "String"
% contains a proper image description of the stipple pattern and
% "width" and "height" give its dimensions.  If "filled" is true then
% it means that the area to be stippled is gotten by filling the
% current path (e.g. the interior of a polygon); if it's false, the
% area is gotten by stroking the current path (e.g. a wide line).
% Each stipple dot is assumed to be about one unit across in the
% current user coordinate system.

/StippleFill {
    % Turn the path into a clip region that we can then cover with
    % lots of images corresponding to the stipple pattern.  Warning:
    % some Postscript interpreters get errors during strokepath for
    % dashed lines.  If this happens, turn off dashes and try again.

    1 index /tmpstip exch def %% Works around NeWSprint bug

    gsave
    {eoclip}
    {{strokepath} stopped {grestore gsave [] 0 setdash strokepath} if clip}
    ifelse

    % Change the scaling so that one user unit in user coordinates
    % corresponds to the size of one stipple dot.
    1 EvenPixels dup scale

    % Compute the bounding box occupied by the path (which is now
    % the clipping region), and round the lower coordinates down
    % to the nearest starting point for the stipple pattern.

    pathbbox
    4 2 roll
    5 index div cvi 5 index mul 4 1 roll
    6 index div cvi 6 index mul 3 2 roll

    % Stack now: width height string y1 y2 x1 x2
    % Below is a doubly-nested for loop to iterate across this area
    % in units of the stipple pattern size, going up columns then
    % across rows, blasting out a stipple-pattern-sized rectangle at
    % each position

    6 index exch {
	2 index 5 index 3 index {
	    % Stack now: width height string y1 y2 x y

	    gsave
	    1 index exch translate
	    5 index 5 index true matrix tmpstip imagemask
	    grestore
	} for
	pop
    } for
    pop pop pop pop pop
    grestore
    newpath
} bind def

% -- AdjustColor --
% Given a color value already set for output by the caller, adjusts
% that value to a grayscale or mono value if requested by the CL
% variable.

/AdjustColor {
    CL 2 lt {
	currentgray
	CL 0 eq {
	    .5 lt {0} {1} ifelse
	} if
	setgray
    } if
} bind def

% x y strings spacing xoffset yoffset justify stipple stipimage DrawText --
% This procedure does all of the real work of drawing text.  The
% color and font must already have been set by the caller, and the
% following arguments must be on the stack:
%
% x, y -	Coordinates at which to draw text.
% strings -	An array of strings, one for each line of the text item,
%		in order from top to bottom.
% spacing -	Spacing between lines.
% xoffset -	Horizontal offset for text bbox relative to x and y: 0 for
%		nw/w/sw anchor, -0.5 for n/center/s, and -1.0 for ne/e/se.
% yoffset -	Vertical offset for text bbox relative to x and y: 0 for
%		nw/n/ne anchor, +0.5 for w/center/e, and +1.0 for sw/s/se.
% justify -	0 for left justification, 0.5 for center, 1 for right justify.
% stipple -	Boolean value indicating whether or not text is to be
%		drawn in stippled fashion.
% stipimage -	Image for stippling, if stipple is True.
%
% Also, when this procedure is invoked, the color and font must already
% have been set for the text.

/DrawText {
    /stipimage exch def
    /stipple exch def
    /justify exch def
    /yoffset exch def
    /xoffset exch def
    /spacing exch def
    /strings exch def

    % First scan through all of the text to find the widest line.

    /maxwidth 0 def
    strings {
	stringwidth pop
	dup maxwidth gt {/maxwidth exch def} {pop} ifelse
	newpath
    } forall

    % Compute the baseline offset and the actual font height.

    0 0 moveto (TXygqPZ) false charpath
    pathbbox dup /baseline exch def
    exch pop exch sub /height exch def pop
    newpath

    % Translate coordinates first so that the origin is at the upper-left
    % corner of the text's bounding box. Remember that x and y for
    % positioning are still on the stack.

    translate
    maxwidth xoffset mul
    strings length 1 sub spacing mul height add yoffset mul translate

    % Now use the baseline and justification information to translate so
    % that the origin is at the baseline and positioning point for the
    % first line of text.

    justify maxwidth mul baseline neg translate

    % Iterate over each of the lines to output it.  For each line,
    % compute its width again so it can be properly justified, then
    % display it.

    strings {
	dup stringwidth pop
	justify neg mul 0 moveto
	show
	0 spacing neg translate
    } forall
} bind def

%%EndProlog
%%BeginSetup
/CL 2 def
%%IncludeResource: font Helvetica-Bold
%%EndSetup

%%Page: 1 1
save
306.0 396.0 translate
90 rotate
0.8832 0.8832 scale
-428 -107 translate
0 214 moveto 856 214 lineto 856 0 lineto 0 0 lineto closepath clip newpath
gsave
0 214 moveto 855 0 rlineto 0 -214 rlineto -855 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
grestore
gsave
0 159 moveto 33 0 rlineto 0 -125 rlineto -33 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
28 146.3992 [
    (0)
] 14 -1 0.5 0 () false DrawText
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
28 121.1976 [
    (1)
] 14 -1 0.5 0 () false DrawText
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
28 95.996 [
    (2)
] 14 -1 0.5 0 () false DrawText
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
28 70.7944 [
    (3)
] 14 -1 0.5 0 () false DrawText
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
28 45.593 [
    (4)
] 14 -1 0.5 0 () false DrawText
grestore
gsave
34 158 moveto 798 0 rlineto 0 -124 rlineto -798 0 rlineto closepath
0.000 0.000 0.000 setrgbcolor AdjustColor
fill
grestore
gsave
34 145.5 moveto
832 145.5 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 120.5 moveto
832 120.5 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 95.5 moveto
832 95.5 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 70.5 moveto
832 70.5 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 45.5 moveto
832 45.5 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 80.5 moveto 23.06 0 rlineto 0 -20 rlineto -23.06 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
34 80.5 moveto 23.06 0 rlineto 0 -20 rlineto -23.06 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 80.5 moveto 23.06 0 rlineto 0 -20 rlineto -23.06 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
34 80.5 moveto 23.06 0 rlineto 0 -20 rlineto -23.06 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 130.5 moveto 27.33 0 rlineto 0 -20 rlineto -27.33 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
34 130.5 moveto 27.33 0 rlineto 0 -20 rlineto -27.33 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 130.5 moveto 27.33 0 rlineto 0 -20 rlineto -27.33 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
34 130.5 moveto 27.33 0 rlineto 0 -20 rlineto -27.33 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 155.5 moveto 32.65 0 rlineto 0 -20 rlineto -32.65 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
34 155.5 moveto 32.65 0 rlineto 0 -20 rlineto -32.65 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 155.5 moveto 32.65 0 rlineto 0 -20 rlineto -32.65 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
34 155.5 moveto 32.65 0 rlineto 0 -20 rlineto -32.65 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 55.5 moveto 36.49 0 rlineto 0 -20 rlineto -36.49 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
34 55.5 moveto 36.49 0 rlineto 0 -20 rlineto -36.49 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 55.5 moveto 36.49 0 rlineto 0 -20 rlineto -36.49 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
34 55.5 moveto 36.49 0 rlineto 0 -20 rlineto -36.49 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 105.5 moveto 38.11 0 rlineto 0 -20 rlineto -38.11 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
34 105.5 moveto 38.11 0 rlineto 0 -20 rlineto -38.11 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
34 105.5 moveto 38.11 0 rlineto 0 -20 rlineto -38.11 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
34 105.5 moveto 38.11 0 rlineto 0 -20 rlineto -38.11 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
72.6599999999999 155.5 moveto 5.78999999999998 0 rlineto 0 -20 rlineto -5.78999999999998 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
72.6599999999999 155.5 moveto 5.78999999999998 0 rlineto 0 -20 rlineto -5.78999999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
72.6599999999999 155.5 moveto 5.78999999999998 0 rlineto 0 -20 rlineto -5.78999999999998 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
72.6599999999999 155.5 moveto 5.78999999999998 0 rlineto 0 -20 rlineto -5.78999999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
152.23 130.5 moveto 93.3499999999999 0 rlineto 0 -20 rlineto -93.3499999999999 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
152.23 130.5 moveto 93.3499999999999 0 rlineto 0 -20 rlineto -93.3499999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
152.23 130.5 moveto 93.3499999999999 0 rlineto 0 -20 rlineto -93.3499999999999 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
152.23 130.5 moveto 93.3499999999999 0 rlineto 0 -20 rlineto -93.3499999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
246.07 130.5 moveto 29.1799999999998 0 rlineto 0 -20 rlineto -29.1799999999998 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
246.07 130.5 moveto 29.1799999999998 0 rlineto 0 -20 rlineto -29.1799999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
246.07 130.5 moveto 29.1799999999998 0 rlineto 0 -20 rlineto -29.1799999999998 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
246.07 130.5 moveto 29.1799999999998 0 rlineto 0 -20 rlineto -29.1799999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
217.53 105.5 moveto 126.02 0 rlineto 0 -20 rlineto -126.02 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
217.53 105.5 moveto 126.02 0 rlineto 0 -20 rlineto -126.02 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
217.53 105.5 moveto 126.02 0 rlineto 0 -20 rlineto -126.02 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
217.53 105.5 moveto 126.02 0 rlineto 0 -20 rlineto -126.02 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
344.53 105.5 moveto 6.44999999999982 0 rlineto 0 -20 rlineto -6.44999999999982 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
344.53 105.5 moveto 6.44999999999982 0 rlineto 0 -20 rlineto -6.44999999999982 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
344.53 105.5 moveto 6.44999999999982 0 rlineto 0 -20 rlineto -6.44999999999982 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
344.53 105.5 moveto 6.44999999999982 0 rlineto 0 -20 rlineto -6.44999999999982 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
123.38 80.5 moveto 290.86 0 rlineto 0 -20 rlineto -290.86 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
123.38 80.5 moveto 290.86 0 rlineto 0 -20 rlineto -290.86 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
123.38 80.5 moveto 290.86 0 rlineto 0 -20 rlineto -290.86 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
123.38 80.5 moveto 290.86 0 rlineto 0 -20 rlineto -290.86 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
414.74 80.5 moveto 37.8800000000001 0 rlineto 0 -20 rlineto -37.8800000000001 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
414.74 80.5 moveto 37.8800000000001 0 rlineto 0 -20 rlineto -37.8800000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
414.74 80.5 moveto 37.8800000000001 0 rlineto 0 -20 rlineto -37.8800000000001 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
414.74 80.5 moveto 37.8800000000001 0 rlineto 0 -20 rlineto -37.8800000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
183.41 55.5 moveto 334.2 0 rlineto 0 -20 rlineto -334.2 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
183.41 55.5 moveto 334.2 0 rlineto 0 -20 rlineto -334.2 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
183.41 55.5 moveto 334.2 0 rlineto 0 -20 rlineto -334.2 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
183.41 55.5 moveto 334.2 0 rlineto 0 -20 rlineto -334.2 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
518.24 55.5 moveto 4.0800000000005 0 rlineto 0 -20 rlineto -4.0800000000005 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
518.24 55.5 moveto 4.0800000000005 0 rlineto 0 -20 rlineto -4.0800000000005 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
518.24 55.5 moveto 4.0800000000005 0 rlineto 0 -20 rlineto -4.0800000000005 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
518.24 55.5 moveto 4.0800000000005 0 rlineto 0 -20 rlineto -4.0800000000005 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
80.0799999999999 155.5 moveto 447.49 0 rlineto 0 -20 rlineto -447.49 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
80.0799999999999 155.5 moveto 447.49 0 rlineto 0 -20 rlineto -447.49 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
80.0799999999999 155.5 moveto 447.49 0 rlineto 0 -20 rlineto -447.49 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
80.0799999999999 155.5 moveto 447.49 0 rlineto 0 -20 rlineto -447.49 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
616.27 155.5 moveto 4.44999999999982 0 rlineto 0 -20 rlineto -4.44999999999982 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
616.27 155.5 moveto 4.44999999999982 0 rlineto 0 -20 rlineto -4.44999999999982 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
616.27 155.5 moveto 4.44999999999982 0 rlineto 0 -20 rlineto -4.44999999999982 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
616.27 155.5 moveto 4.44999999999982 0 rlineto 0 -20 rlineto -4.44999999999982 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
275.86 130.5 moveto 347.51 0 rlineto 0 -20 rlineto -347.51 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
275.86 130.5 moveto 347.51 0 rlineto 0 -20 rlineto -347.51 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
275.86 130.5 moveto 347.51 0 rlineto 0 -20 rlineto -347.51 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
275.86 130.5 moveto 347.51 0 rlineto 0 -20 rlineto -347.51 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
623.85 130.5 moveto 4.95000000000016 0 rlineto 0 -20 rlineto -4.95000000000016 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
623.85 130.5 moveto 4.95000000000016 0 rlineto 0 -20 rlineto -4.95000000000016 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
623.85 130.5 moveto 4.95000000000016 0 rlineto 0 -20 rlineto -4.95000000000016 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
623.85 130.5 moveto 4.95000000000016 0 rlineto 0 -20 rlineto -4.95000000000016 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
352.07 105.5 moveto 279.87 0 rlineto 0 -20 rlineto -279.87 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
352.07 105.5 moveto 279.87 0 rlineto 0 -20 rlineto -279.87 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
352.07 105.5 moveto 279.87 0 rlineto 0 -20 rlineto -279.87 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
352.07 105.5 moveto 279.87 0 rlineto 0 -20 rlineto -279.87 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
632.89 105.5 moveto 8.84000000000026 0 rlineto 0 -20 rlineto -8.84000000000026 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
632.89 105.5 moveto 8.84000000000026 0 rlineto 0 -20 rlineto -8.84000000000026 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
632.89 105.5 moveto 8.84000000000026 0 rlineto 0 -20 rlineto -8.84000000000026 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
632.89 105.5 moveto 8.84000000000026 0 rlineto 0 -20 rlineto -8.84000000000026 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
453.24 80.5 moveto 189.6 0 rlineto 0 -20 rlineto -189.6 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
453.24 80.5 moveto 189.6 0 rlineto 0 -20 rlineto -189.6 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
453.24 80.5 moveto 189.6 0 rlineto 0 -20 rlineto -189.6 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
453.24 80.5 moveto 189.6 0 rlineto 0 -20 rlineto -189.6 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
643.34 80.5 moveto 4.9699999999998 0 rlineto 0 -20 rlineto -4.9699999999998 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
643.34 80.5 moveto 4.9699999999998 0 rlineto 0 -20 rlineto -4.9699999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
643.34 80.5 moveto 4.9699999999998 0 rlineto 0 -20 rlineto -4.9699999999998 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
643.34 80.5 moveto 4.9699999999998 0 rlineto 0 -20 rlineto -4.9699999999998 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
523.03 55.5 moveto 129.23 0 rlineto 0 -20 rlineto -129.23 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
523.03 55.5 moveto 129.23 0 rlineto 0 -20 rlineto -129.23 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
523.03 55.5 moveto 129.23 0 rlineto 0 -20 rlineto -129.23 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
523.03 55.5 moveto 129.23 0 rlineto 0 -20 rlineto -129.23 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
652.87 55.5 moveto 6.11000000000013 0 rlineto 0 -20 rlineto -6.11000000000013 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
652.87 55.5 moveto 6.11000000000013 0 rlineto 0 -20 rlineto -6.11000000000013 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
652.87 55.5 moveto 6.11000000000013 0 rlineto 0 -20 rlineto -6.11000000000013 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
652.87 55.5 moveto 6.11000000000013 0 rlineto 0 -20 rlineto -6.11000000000013 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
634.7 155.5 moveto 29.8200000000003 0 rlineto 0 -20 rlineto -29.8200000000003 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
634.7 155.5 moveto 29.8200000000003 0 rlineto 0 -20 rlineto -29.8200000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
634.7 155.5 moveto 29.8200000000003 0 rlineto 0 -20 rlineto -29.8200000000003 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
634.7 155.5 moveto 29.8200000000003 0 rlineto 0 -20 rlineto -29.8200000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
666.27 155.5 moveto 4.34999999999991 0 rlineto 0 -20 rlineto -4.34999999999991 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
666.27 155.5 moveto 4.34999999999991 0 rlineto 0 -20 rlineto -4.34999999999991 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
666.27 155.5 moveto 4.34999999999991 0 rlineto 0 -20 rlineto -4.34999999999991 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
666.27 155.5 moveto 4.34999999999991 0 rlineto 0 -20 rlineto -4.34999999999991 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
629.39 130.5 moveto 43.7000000000003 0 rlineto 0 -20 rlineto -43.7000000000003 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
629.39 130.5 moveto 43.7000000000003 0 rlineto 0 -20 rlineto -43.7000000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
629.39 130.5 moveto 43.7000000000003 0 rlineto 0 -20 rlineto -43.7000000000003 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
629.39 130.5 moveto 43.7000000000003 0 rlineto 0 -20 rlineto -43.7000000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
673.57 130.5 moveto 3.28999999999996 0 rlineto 0 -20 rlineto -3.28999999999996 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
673.57 130.5 moveto 3.28999999999996 0 rlineto 0 -20 rlineto -3.28999999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
673.57 130.5 moveto 3.28999999999996 0 rlineto 0 -20 rlineto -3.28999999999996 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
673.57 130.5 moveto 3.28999999999996 0 rlineto 0 -20 rlineto -3.28999999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
642.83 105.5 moveto 40.6100000000001 0 rlineto 0 -20 rlineto -40.6100000000001 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
642.83 105.5 moveto 40.6100000000001 0 rlineto 0 -20 rlineto -40.6100000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
642.83 105.5 moveto 40.6100000000001 0 rlineto 0 -20 rlineto -40.6100000000001 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
642.83 105.5 moveto 40.6100000000001 0 rlineto 0 -20 rlineto -40.6100000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
684.39 105.5 moveto 5.48000000000002 0 rlineto 0 -20 rlineto -5.48000000000002 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
684.39 105.5 moveto 5.48000000000002 0 rlineto 0 -20 rlineto -5.48000000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
684.39 105.5 moveto 5.48000000000002 0 rlineto 0 -20 rlineto -5.48000000000002 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
684.39 105.5 moveto 5.48000000000002 0 rlineto 0 -20 rlineto -5.48000000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
648.9 80.5 moveto 45.5000000000001 0 rlineto 0 -20 rlineto -45.5000000000001 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
648.9 80.5 moveto 45.5000000000001 0 rlineto 0 -20 rlineto -45.5000000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
648.9 80.5 moveto 45.5000000000001 0 rlineto 0 -20 rlineto -45.5000000000001 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
648.9 80.5 moveto 45.5000000000001 0 rlineto 0 -20 rlineto -45.5000000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
694.89 80.5 moveto 3.34000000000026 0 rlineto 0 -20 rlineto -3.34000000000026 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
694.89 80.5 moveto 3.34000000000026 0 rlineto 0 -20 rlineto -3.34000000000026 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
694.89 80.5 moveto 3.34000000000026 0 rlineto 0 -20 rlineto -3.34000000000026 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
694.89 80.5 moveto 3.34000000000026 0 rlineto 0 -20 rlineto -3.34000000000026 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
659.73 55.5 moveto 44.13 0 rlineto 0 -20 rlineto -44.13 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
659.73 55.5 moveto 44.13 0 rlineto 0 -20 rlineto -44.13 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
659.73 55.5 moveto 44.13 0 rlineto 0 -20 rlineto -44.13 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
659.73 55.5 moveto 44.13 0 rlineto 0 -20 rlineto -44.13 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
704.48 55.5 moveto 4.07000000000028 0 rlineto 0 -20 rlineto -4.07000000000028 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
704.48 55.5 moveto 4.07000000000028 0 rlineto 0 -20 rlineto -4.07000000000028 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
704.48 55.5 moveto 4.07000000000028 0 rlineto 0 -20 rlineto -4.07000000000028 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
704.48 55.5 moveto 4.07000000000028 0 rlineto 0 -20 rlineto -4.07000000000028 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
673.45 155.5 moveto 40.1500000000001 0 rlineto 0 -20 rlineto -40.1500000000001 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
673.45 155.5 moveto 40.1500000000001 0 rlineto 0 -20 rlineto -40.1500000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
673.45 155.5 moveto 40.1500000000001 0 rlineto 0 -20 rlineto -40.1500000000001 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
673.45 155.5 moveto 40.1500000000001 0 rlineto 0 -20 rlineto -40.1500000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
715.23 155.5 moveto 4.28999999999996 0 rlineto 0 -20 rlineto -4.28999999999996 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
715.23 155.5 moveto 4.28999999999996 0 rlineto 0 -20 rlineto -4.28999999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
715.23 155.5 moveto 4.28999999999996 0 rlineto 0 -20 rlineto -4.28999999999996 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
715.23 155.5 moveto 4.28999999999996 0 rlineto 0 -20 rlineto -4.28999999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
677.45 130.5 moveto 44.4900000000004 0 rlineto 0 -20 rlineto -44.4900000000004 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
677.45 130.5 moveto 44.4900000000004 0 rlineto 0 -20 rlineto -44.4900000000004 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
677.45 130.5 moveto 44.4900000000004 0 rlineto 0 -20 rlineto -44.4900000000004 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
677.45 130.5 moveto 44.4900000000004 0 rlineto 0 -20 rlineto -44.4900000000004 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
722.43 130.5 moveto 3.24000000000035 0 rlineto 0 -20 rlineto -3.24000000000035 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
722.43 130.5 moveto 3.24000000000035 0 rlineto 0 -20 rlineto -3.24000000000035 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
722.43 130.5 moveto 3.24000000000035 0 rlineto 0 -20 rlineto -3.24000000000035 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
722.43 130.5 moveto 3.24000000000035 0 rlineto 0 -20 rlineto -3.24000000000035 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
690.95 105.5 moveto 39.0500000000003 0 rlineto 0 -20 rlineto -39.0500000000003 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
690.95 105.5 moveto 39.0500000000003 0 rlineto 0 -20 rlineto -39.0500000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
690.95 105.5 moveto 39.0500000000003 0 rlineto 0 -20 rlineto -39.0500000000003 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
690.95 105.5 moveto 39.0500000000003 0 rlineto 0 -20 rlineto -39.0500000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
730.97 105.5 moveto 5.32000000000028 0 rlineto 0 -20 rlineto -5.32000000000028 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
730.97 105.5 moveto 5.32000000000028 0 rlineto 0 -20 rlineto -5.32000000000028 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
730.97 105.5 moveto 5.32000000000028 0 rlineto 0 -20 rlineto -5.32000000000028 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
730.97 105.5 moveto 5.32000000000028 0 rlineto 0 -20 rlineto -5.32000000000028 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
698.82 80.5 moveto 42.1299999999995 0 rlineto 0 -20 rlineto -42.1299999999995 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
698.82 80.5 moveto 42.1299999999995 0 rlineto 0 -20 rlineto -42.1299999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
698.82 80.5 moveto 42.1299999999995 0 rlineto 0 -20 rlineto -42.1299999999995 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
698.82 80.5 moveto 42.1299999999995 0 rlineto 0 -20 rlineto -42.1299999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
741.43 80.5 moveto 3.30999999999995 0 rlineto 0 -20 rlineto -3.30999999999995 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
741.43 80.5 moveto 3.30999999999995 0 rlineto 0 -20 rlineto -3.30999999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
741.43 80.5 moveto 3.30999999999995 0 rlineto 0 -20 rlineto -3.30999999999995 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
741.43 80.5 moveto 3.30999999999995 0 rlineto 0 -20 rlineto -3.30999999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
709.28 55.5 moveto 41.0299999999996 0 rlineto 0 -20 rlineto -41.0299999999996 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
709.28 55.5 moveto 41.0299999999996 0 rlineto 0 -20 rlineto -41.0299999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
709.28 55.5 moveto 41.0299999999996 0 rlineto 0 -20 rlineto -41.0299999999996 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
709.28 55.5 moveto 41.0299999999996 0 rlineto 0 -20 rlineto -41.0299999999996 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
750.92 55.5 moveto 4.57999999999981 0 rlineto 0 -20 rlineto -4.57999999999981 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
750.92 55.5 moveto 4.57999999999981 0 rlineto 0 -20 rlineto -4.57999999999981 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
750.92 55.5 moveto 4.57999999999981 0 rlineto 0 -20 rlineto -4.57999999999981 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
750.92 55.5 moveto 4.57999999999981 0 rlineto 0 -20 rlineto -4.57999999999981 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
721.95 155.5 moveto 38.1400000000003 0 rlineto 0 -20 rlineto -38.1400000000003 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
721.95 155.5 moveto 38.1400000000003 0 rlineto 0 -20 rlineto -38.1400000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
721.95 155.5 moveto 38.1400000000003 0 rlineto 0 -20 rlineto -38.1400000000003 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
721.95 155.5 moveto 38.1400000000003 0 rlineto 0 -20 rlineto -38.1400000000003 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
761.72 155.5 moveto 4.33000000000038 0 rlineto 0 -20 rlineto -4.33000000000038 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
761.72 155.5 moveto 4.33000000000038 0 rlineto 0 -20 rlineto -4.33000000000038 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
761.72 155.5 moveto 4.33000000000038 0 rlineto 0 -20 rlineto -4.33000000000038 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
761.72 155.5 moveto 4.33000000000038 0 rlineto 0 -20 rlineto -4.33000000000038 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
726.26 130.5 moveto 42.1799999999997 0 rlineto 0 -20 rlineto -42.1799999999997 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
726.26 130.5 moveto 42.1799999999997 0 rlineto 0 -20 rlineto -42.1799999999997 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
726.26 130.5 moveto 42.1799999999997 0 rlineto 0 -20 rlineto -42.1799999999997 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
726.26 130.5 moveto 42.1799999999997 0 rlineto 0 -20 rlineto -42.1799999999997 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
769.26 130.5 moveto 3.27999999999975 0 rlineto 0 -20 rlineto -3.27999999999975 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
769.26 130.5 moveto 3.27999999999975 0 rlineto 0 -20 rlineto -3.27999999999975 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
769.26 130.5 moveto 3.27999999999975 0 rlineto 0 -20 rlineto -3.27999999999975 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
769.26 130.5 moveto 3.27999999999975 0 rlineto 0 -20 rlineto -3.27999999999975 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
737.36 105.5 moveto 39.5499999999997 0 rlineto 0 -20 rlineto -39.5499999999997 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
737.36 105.5 moveto 39.5499999999997 0 rlineto 0 -20 rlineto -39.5499999999997 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
737.36 105.5 moveto 39.5499999999997 0 rlineto 0 -20 rlineto -39.5499999999997 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
737.36 105.5 moveto 39.5499999999997 0 rlineto 0 -20 rlineto -39.5499999999997 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
778.47 105.5 moveto 5.32000000000016 0 rlineto 0 -20 rlineto -5.32000000000016 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
778.47 105.5 moveto 5.32000000000016 0 rlineto 0 -20 rlineto -5.32000000000016 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
778.47 105.5 moveto 5.32000000000016 0 rlineto 0 -20 rlineto -5.32000000000016 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
778.47 105.5 moveto 5.32000000000016 0 rlineto 0 -20 rlineto -5.32000000000016 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
745.31 80.5 moveto 43.4 0 rlineto 0 -20 rlineto -43.4 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
745.31 80.5 moveto 43.4 0 rlineto 0 -20 rlineto -43.4 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
745.31 80.5 moveto 43.4 0 rlineto 0 -20 rlineto -43.4 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
745.31 80.5 moveto 43.4 0 rlineto 0 -20 rlineto -43.4 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
789.55 80.5 moveto 4.54999999999961 0 rlineto 0 -20 rlineto -4.54999999999961 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
789.55 80.5 moveto 4.54999999999961 0 rlineto 0 -20 rlineto -4.54999999999961 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
789.55 80.5 moveto 4.54999999999961 0 rlineto 0 -20 rlineto -4.54999999999961 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
789.55 80.5 moveto 4.54999999999961 0 rlineto 0 -20 rlineto -4.54999999999961 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
756.21 55.5 moveto 43.25 0 rlineto 0 -20 rlineto -43.25 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
756.21 55.5 moveto 43.25 0 rlineto 0 -20 rlineto -43.25 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
756.21 55.5 moveto 43.25 0 rlineto 0 -20 rlineto -43.25 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
756.21 55.5 moveto 43.25 0 rlineto 0 -20 rlineto -43.25 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
800.5 55.5 moveto 4.04999999999995 0 rlineto 0 -20 rlineto -4.04999999999995 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
800.5 55.5 moveto 4.04999999999995 0 rlineto 0 -20 rlineto -4.04999999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
800.5 55.5 moveto 4.04999999999995 0 rlineto 0 -20 rlineto -4.04999999999995 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
800.5 55.5 moveto 4.04999999999995 0 rlineto 0 -20 rlineto -4.04999999999995 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
768.96 155.5 moveto 45.9099999999999 0 rlineto 0 -20 rlineto -45.9099999999999 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
768.96 155.5 moveto 45.9099999999999 0 rlineto 0 -20 rlineto -45.9099999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
768.96 155.5 moveto 45.9099999999999 0 rlineto 0 -20 rlineto -45.9099999999999 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
768.96 155.5 moveto 45.9099999999999 0 rlineto 0 -20 rlineto -45.9099999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
773.08 130.5 moveto 54.9099999999999 0 rlineto 0 -20 rlineto -54.9099999999999 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
773.08 130.5 moveto 54.9099999999999 0 rlineto 0 -20 rlineto -54.9099999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
773.08 130.5 moveto 54.9099999999999 0 rlineto 0 -20 rlineto -54.9099999999999 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
773.08 130.5 moveto 54.9099999999999 0 rlineto 0 -20 rlineto -54.9099999999999 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
816.47 155.5 moveto 12.8700000000002 0 rlineto 0 -20 rlineto -12.8700000000002 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
816.47 155.5 moveto 12.8700000000002 0 rlineto 0 -20 rlineto -12.8700000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
816.47 155.5 moveto 12.8700000000002 0 rlineto 0 -20 rlineto -12.8700000000002 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
816.47 155.5 moveto 12.8700000000002 0 rlineto 0 -20 rlineto -12.8700000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
784.77 105.5 moveto 44.77 0 rlineto 0 -20 rlineto -44.77 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
784.77 105.5 moveto 44.77 0 rlineto 0 -20 rlineto -44.77 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
784.77 105.5 moveto 44.77 0 rlineto 0 -20 rlineto -44.77 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
784.77 105.5 moveto 44.77 0 rlineto 0 -20 rlineto -44.77 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
794.67 80.5 moveto 37.3300000000002 0 rlineto 0 -20 rlineto -37.3300000000002 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
794.67 80.5 moveto 37.3300000000002 0 rlineto 0 -20 rlineto -37.3300000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
794.67 80.5 moveto 37.3300000000002 0 rlineto 0 -20 rlineto -37.3300000000002 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
794.67 80.5 moveto 37.3300000000002 0 rlineto 0 -20 rlineto -37.3300000000002 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
805.22 55.5 moveto 26.78 0 rlineto 0 -20 rlineto -26.78 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
805.22 55.5 moveto 26.78 0 rlineto 0 -20 rlineto -26.78 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
0.000 0.000 0.000 setrgbcolor AdjustColor
stroke
grestore
gsave
805.22 55.5 moveto 26.78 0 rlineto 0 -20 rlineto -26.78 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
805.22 55.5 moveto 26.78 0 rlineto 0 -20 rlineto -26.78 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
73.4000000000001 145.5 moveto
240.353503992289 121.199594784541 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
245.16 120.5 moveto
235.768392696515 125.403846679611 lineto
237.032700391138 122.188212750893 lineto
236.888666170791 121.198640043423 lineto
234.760153154088 118.476837727322 lineto
245.16 120.5 lineto
fill
grestore
gsave
246.56 120.5 moveto
338.019128775896 96.7221483007757 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
342.72 95.5 moveto
333.922401402404 101.403580177789 lineto
334.826676119256 98.0687544332162 lineto
334.575057351449 97.1009280047243 lineto
332.161070027757 94.6287951783459 lineto
342.72 95.5 lineto
fill
grestore
gsave
345.41 95.5 moveto
409.248021485854 72.167389807802 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
413.81 70.5 moveto
405.619192773556 77.2201694159624 lineto
406.199444426671 73.8139862838831 lineto
405.856158289771 72.8747554133236 lineto
403.216189815253 70.6455533220458 lineto
413.81 70.5 lineto
fill
grestore
gsave
415.22 70.5 moveto
512.362856004999 46.6577518149915 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
517.08 45.5 moveto
508.202495288741 51.28272514491 lineto
509.152287636541 47.9605767427062 lineto
508.913926968748 46.989400037853 lineto
506.533970614194 44.4844882109374 lineto
517.08 45.5 lineto
fill
grestore
gsave
518.86 45.5 moveto
526.624970125899 140.658947621306 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
527.02 145.5 moveto
522.71829780985 135.817781335497 lineto
525.847781881667 137.282399017503 lineto
526.844469136104 137.201069337541 lineto
529.695108590909 135.248473575762 lineto
527.02 145.5 lineto
fill
grestore
gsave
617.02 145.5 moveto
621.845411887599 125.225160136137 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
622.97 120.5 moveto
624.059566337274 131.038635831865 lineto
621.537998528154 128.676333714393 lineto
620.565171441302 128.444800867722 lineto
617.249776729311 129.41790590517 lineto
622.97 120.5 lineto
fill
grestore
gsave
624.34 120.5 moveto
629.914710090039 100.184001129595 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
631.2 95.5 moveto
631.929050999348 106.069696525461 lineto
629.489623211055 103.62266412357 lineto
628.525270037314 103.358045612695 lineto
625.178578783167 104.217366949341 lineto
631.2 95.5 lineto
fill
grestore
gsave
633.74 95.5 moveto
640.83525373785 75.0878776241366 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
642.43 70.5 moveto
642.452669512966 81.0947857974186 lineto
640.181832014111 78.490544532866 lineto
639.237268973848 78.1622144200705 lineto
635.840728231122 78.7964750078496 lineto
642.43 70.5 lineto
fill
grestore
gsave
643.83 70.5 moveto
650.283107761839 50.1303416608622 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
651.75 45.5 moveto
652.066497471172 56.0900816498619 lineto
649.724307235285 53.5498217401051 lineto
648.771001599225 53.2478145146013 lineto
645.393358018753 53.9760310713355 lineto
651.75 45.5 lineto
fill
grestore
gsave
653.49 45.5 moveto
660.769511086822 140.657007670881 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
661.14 145.5 moveto
656.887425412181 135.796103392192 lineto
660.009446173052 137.276563414918 lineto
661.006532829047 137.200286285734 lineto
663.867032004146 135.262163487906 lineto
661.14 145.5 lineto
fill
grestore
gsave
666.97 145.5 moveto
671.606679628477 125.23479183358 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
672.69 120.5 moveto
673.871469821062 131.028728748615 lineto
671.329387937329 128.688516107293 lineto
670.354577853945 128.465479560215 lineto
667.047799237372 129.467472919067 lineto
672.69 120.5 lineto
fill
grestore
gsave
674.06 120.5 moveto
681.080014261353 100.092981798393 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
682.66 95.5 moveto
682.71673683398 106.094658131892 lineto
680.437537160378 103.497732188001 lineto
679.491923260709 103.172441006515 lineto
676.097439536297 103.817619861489 lineto
682.66 95.5 lineto
fill
grestore
gsave
685.24 95.5 moveto
692.377074758506 75.0850264344794 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
693.98 70.5 moveto
693.983775904123 81.0948093773578 lineto
691.717586132765 78.4865226925011 lineto
690.773610102137 78.1565086721935 lineto
687.375943689726 78.7847112352046 lineto
693.98 70.5 lineto
fill
grestore
gsave
695.38 70.5 moveto
701.883019005753 50.1271334406226 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
703.36 45.5 moveto
703.653414697 56.090746329489 lineto
701.316766746349 53.5453874422345 lineto
700.364121626221 53.2413031198896 lineto
696.984898856102 53.9621560730746 lineto
703.36 45.5 lineto
fill
grestore
gsave
705.1 45.5 moveto
712.655552799935 140.65809571707 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
713.04 145.5 moveto
708.759471795991 135.808401664602 lineto
711.885746982529 137.279856376185 lineto
712.882609629015 137.200705482054 lineto
715.73751032139 135.254345405686 lineto
713.04 145.5 lineto
fill
grestore
gsave
715.9 145.5 moveto
720.471092227362 125.238066368076 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
721.54 120.5 moveto
722.7535024098 131.025084888086 lineto
720.204311219861 128.692618428018 lineto
719.22882696761 128.47254918071 lineto
715.925112644044 129.484600156931 lineto
721.54 120.5 lineto
fill
grestore
gsave
722.91 120.5 moveto
728.064255210907 100.207656649975 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
729.26 95.5 moveto
730.190454490819 106.053873906794 lineto
727.704811779632 103.653799778246 lineto
726.735588351696 103.40761702755 lineto
723.405890495267 104.330594651924 lineto
729.26 95.5 lineto
fill
grestore
gsave
731.82 95.5 moveto
738.940344519663 75.0861682349125 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
740.54 70.5 moveto
740.551330533286 81.0948039915336 lineto
738.283281498901 78.4881338766502 lineto
737.339070391713 78.158793042463 lineto
733.941852782971 78.7894181522237 lineto
740.54 70.5 lineto
fill
grestore
gsave
741.93 70.5 moveto
748.349845235 50.1324707011423 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
749.81 45.5 moveto
750.141902900823 56.08961002419 lineto
747.796019737765 53.5527600689338 lineto
746.842275769882 53.2521399702574 lineto
743.465695125647 53.9852693334546 lineto
749.81 45.5 lineto
fill
grestore
gsave
751.55 45.5 moveto
759.172184980481 140.658364300639 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
759.56 145.5 moveto
755.272731588215 135.811381441851 lineto
758.400029527063 137.280661235452 lineto
759.396836876932 137.200816966727 lineto
762.250383037295 135.252471560779 lineto
759.56 145.5 lineto
fill
grestore
gsave
762.4 145.5 moveto
766.979289019929 125.237659203853 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
768.05 120.5 moveto
769.25949652616 131.025545978865 lineto
766.711193246159 128.69210947805 lineto
765.735792821836 128.471668982153 lineto
762.431693555902 129.482462507587 lineto
768.05 120.5 lineto
fill
grestore
gsave
769.76 120.5 moveto
774.963650720593 100.204950387705 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
776.17 95.5 moveto
777.07667161589 106.055943661319 lineto
774.596443180922 103.650274851907 lineto
773.627776924629 103.401908823793 lineto
770.296007821844 104.317381464526 lineto
776.17 95.5 lineto
fill
grestore
gsave
779.31 95.5 moveto
786.648028952802 75.0711888841808 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
788.29 70.5 moveto
788.203416304664 81.0944562514412 lineto
785.959554128152 78.4669368808141 lineto
785.018427004938 78.1288840181557 lineto
781.615526442168 78.7280862128327 lineto
788.29 70.5 lineto
fill
grestore
gsave
790.06 70.5 moveto
797.322629618489 50.0764071471052 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
798.95 45.5 moveto
798.897236717598 56.0946786660111 lineto
796.644998320213 53.4743356137468 lineto
795.70279684875 53.1392887704946 lineto
792.301826417358 53.7493507632458 lineto
798.95 45.5 lineto
fill
grestore
gsave
801.12 45.5 moveto
813.693897548859 140.684689998936 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
814.33 145.5 moveto
809.550521570415 136.044494411131 lineto
812.749190377357 137.35112877992 lineto
813.740577730517 137.220166510568 lineto
816.490233042536 135.127758525664 lineto
814.33 145.5 lineto
fill
grestore
gsave
33 159 moveto 800 0 rlineto 0 -126 rlineto -800 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
33 159 moveto 800 0 rlineto 0 -126 rlineto -800 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
33 194 [
    (Logfile Title:)
] 14 0 1 0 () false DrawText
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
113 194 [
    (Me)
] 14 0 1 0 () false DrawText
grestore
gsave
34 194 moveto 793 0 rlineto 0 -33 rlineto -793 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
grestore
gsave
41 185 moveto 45 0 rlineto 0 -15 rlineto -45 0 rlineto closepath
1.000 1.000 0.000 setrgbcolor AdjustColor
fill
41 185 moveto 45 0 rlineto 0 -15 rlineto -45 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
127 178 [
    (BARRIER)
] 14 -0.5 0.5 0 () false DrawText
grestore
gsave
175 185 moveto 45 0 rlineto 0 -15 rlineto -45 0 rlineto closepath
0.000 1.000 0.000 setrgbcolor AdjustColor
fill
175 185 moveto 45 0 rlineto 0 -15 rlineto -45 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
248 178 [
    (RECV)
] 14 -0.5 0.5 0 () false DrawText
grestore
gsave
282 185 moveto 45.0000000000001 0 rlineto 0 -15 rlineto -45.0000000000001 0 rlineto closepath
0.000 0.000 1.000 setrgbcolor AdjustColor
fill
282 185 moveto 45.0000000000001 0 rlineto 0 -15 rlineto -45.0000000000001 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
354 178 [
    (SEND)
] 14 -0.5 0.5 0 () false DrawText
grestore
gsave
33 194 moveto 800 0 rlineto 0 -35 rlineto -800 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
49 32 moveto 776 0 rlineto 0 -30 rlineto -776 0 rlineto closepath
0.275 0.510 0.706 setrgbcolor AdjustColor
fill
grestore
gsave
69.73 32 moveto
69.73 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
69.73 19 [
    (0.59)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
110.57 32 moveto
110.57 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
110.57 19 [
    (0.60)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
151.42 32 moveto
151.42 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
151.42 19 [
    (0.61)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
192.26 32 moveto
192.26 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
192.26 19 [
    (0.62)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
233.1 32 moveto
233.1 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
233.1 19 [
    (0.63)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
273.94 32 moveto
273.94 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
273.94 19 [
    (0.64)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
314.79 32 moveto
314.79 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
314.79 19 [
    (0.65)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
355.63 32 moveto
355.63 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
355.63 19 [
    (0.66)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
396.47 32 moveto
396.47 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
396.47 19 [
    (0.67)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
437.32 32 moveto
437.32 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
437.32 19 [
    (0.68)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
478.16 32 moveto
478.16 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
478.16 19 [
    (0.69)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
519 32 moveto
519 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
519 19 [
    (0.70)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
559.84 32 moveto
559.84 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
559.84 19 [
    (0.71)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
600.69 32 moveto
600.69 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
600.69 19 [
    (0.72)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
641.53 32 moveto
641.53 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
641.53 19 [
    (0.73)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
682.37 32 moveto
682.37 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
682.37 19 [
    (0.74)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
723.22 32 moveto
723.22 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
723.22 19 [
    (0.75)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
764.06 32 moveto
764.06 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
764.06 19 [
    (0.76)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
804.9 32 moveto
804.9 22 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
/Helvetica-Bold findfont 12 scalefont setfont
1.000 1.000 1.000 setrgbcolor AdjustColor
804.9 19 [
    (0.77)
] 14 -0.5 0 0 () false DrawText
grestore
gsave
33 33 moveto
833 33 lineto
1 setlinewidth
0 setlinecap
1 setlinejoin
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
gsave
0 214 moveto 855 0 rlineto 0 -214 rlineto -855 0 rlineto closepath
1 setlinewidth 0 setlinejoin 2 setlinecap
1.000 1.000 1.000 setrgbcolor AdjustColor
stroke
grestore
restore showpage

%%Trailer
end
%%EOF
