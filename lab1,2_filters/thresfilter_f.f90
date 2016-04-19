!
!  File: thresfilter_f.f90
!
!  Implementation of thresfilter function in Fortran 90.
!    

module pic
type pixel
  character (kind=1) r,g,b
end type pixel

end module pic

subroutine thresfilter_f(xsize, ysize, src)
  use pic
  integer (kind=4) xsize, ysize, pixsum, size
  type(pixel), dimension(1:xsize*ysize) :: src
  
  pixsum = sum(iachar(src(:)%r)+iachar(src(:)%g)+iachar(src(:)%b))

  pixsum = pixsum / (xsize * ysize)

  WHERE (iachar(src(:)%r)+iachar(src(:)%g)+iachar(src(:)%b) < pixsum)
     src(:)%r = char(0)
     src(:)%g = char(0)
     src(:)%b = char(0)
  elsewhere
     src(:)%r = char(255)
     src(:)%g = char(255)
     src(:)%b = char(255)
  end where

end subroutine thresfilter_f



