!
!  File: blurfilter.f90
!
!  Implementation of blurfilter function in Fortran 90.
!    

module pic
type pixel
  character (kind=1) r,g,b
end type pixel

end module pic

subroutine blurfilter_f(xsize, ysize, src, radius, w)
  use pic
  integer xsize, ysize, radius
  real (kind=8), dimension(1:radius+1) :: w
!  type(pixel), dimension(xsize*ysize) :: src
  type(pixel), dimension(1:xsize,1:ysize) :: src
  integer x,y,x2,y2, wi
  real (kind=8) r,g,b,n
  real (kind=8), dimension(1:radius+1) :: wc
  real (kind=8), dimension(1:xsize, 1:ysize) :: r, g, b,r1, g1, b1

  r(:,:) = w(1) * iachar(src(:,:)%r)
  g(:,:) = w(1) * iachar(src(:,:)%g)
  b(:,:) = w(1) * iachar(src(:,:)%b)
 
  do wi = 1, radius
     r(:,1+wi:ysize) = r(:,1+wi:ysize) + w(wi+1) * iachar(src(:,1:ysize-wi)%r)
     r(:,1:ysize-wi) = r(:,1:ysize-wi) + w(wi+1) * iachar(src(:,1+wi:ysize)%r)
     
     g(:,1+wi:ysize) = g(:,1+wi:ysize) + w(wi+1) * iachar(src(:,1:ysize-wi)%g)
     g(:,1:ysize-wi) = g(:,1:ysize-wi) + w(wi+1) * iachar(src(:,1+wi:ysize)%g)

     b(:,1+wi:ysize) = b(:,1+wi:ysize) + w(wi+1) * iachar(src(:,1:ysize-wi)%b)
     b(:,1:ysize-wi) = b(:,1:ysize-wi) + w(wi+1) * iachar(src(:,1+wi:ysize)%b)
  end do

  wc = sum(w(1:radius+1))

  do wi = 2, radius+1
    wc(wi) = wc(wi-1) + w(wi)
  end do

  r(:,1+radius:ysize-radius) = r(:,1+radius:ysize-radius) / wc(radius+1)
  g(:,1+radius:ysize-radius) = g(:,1+radius:ysize-radius) / wc(radius+1)
  b(:,1+radius:ysize-radius) = b(:,1+radius:ysize-radius) / wc(radius+1)

  do wi = 1, radius
     r(:,wi) = r(:,wi) / wc(wi)
     g(:,wi) = g(:,wi) / wc(wi)
     b(:,wi) = b(:,wi) / wc(wi)
     r(:,ysize-wi+1) = r(:,ysize-wi+1) / wc(wi)
     g(:,ysize-wi+1) = g(:,ysize-wi+1) / wc(wi)
     b(:,ysize-wi+1) = b(:,ysize-wi+1) / wc(wi)
  end do


  r1(:,:) = w(1) *  floor(r(:,:))
  g1(:,:) = w(1) *  floor(g(:,:))
  b1(:,:) = w(1) *  floor(b(:,:))
 
  do wi = 1, radius
     r1(1+wi:xsize,:) = r1(1+wi:xsize,:) + w(wi+1) * r(1:xsize-wi,:)
     r1(1:xsize-wi,:) = r1(1:xsize-wi,:) + w(wi+1) * r(1+wi:xsize,:)

     g1(1+wi:xsize,:) = g1(1+wi:xsize,:) + w(wi+1) * g(1:xsize-wi,:)
     g1(1:xsize-wi,:) = g1(1:xsize-wi,:) + w(wi+1) * g(1+wi:xsize,:)

     b1(1+wi:xsize,:) = b1(1+wi:xsize,:) + w(wi+1) * b(1:xsize-wi,:)
     b1(1:xsize-wi,:) = b1(1:xsize-wi,:) + w(wi+1) * b(1+wi:xsize,:)
     
  end do

  r1(1+radius:xsize-radius,:) = r1(1+radius:xsize-radius,:) / wc(radius+1)
  g1(1+radius:xsize-radius,:) = g1(1+radius:xsize-radius,:) / wc(radius+1)
  b1(1+radius:xsize-radius,:) = b1(1+radius:xsize-radius,:) / wc(radius+1)

  do wi = 1, radius
     r1(wi,:) = r1(wi,:) / wc(wi)
     g1(wi,:) = g1(wi,:) / wc(wi)
     b1(wi,:) = b1(wi,:) / wc(wi)
     r1(xsize-wi+1,:) = r1(xsize-wi+1,:) / wc(wi)
     g1(xsize-wi+1,:) = g1(xsize-wi+1,:) / wc(wi)
     b1(xsize-wi+1,:) = b1(xsize-wi+1,:) / wc(wi)
  end do

  src(:,:)%r = char( floor(r1(:,:)))
  src(:,:)%g = char( floor(g1(:,:)))
  src(:,:)%b = char( floor(b1(:,:)))

end subroutine blurfilter_f



