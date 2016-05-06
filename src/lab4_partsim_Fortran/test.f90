program test
  implicit none
  double precision, dimension(4) :: part
  double precision t
  
  t = 0.5
  part = (/1.0, 1.0, 1.0, -1.0/)
  write(*,*) part
  call feuler(part, t)
  write(*,*) part

end program test

subroutine feuler(part, timeStep)
  ! Asuming part has (x,y,vx,vy)
  implicit none
  double precision, intent(inout), dimension(4) :: part
  double precision, intent(in) :: timeStep
!  write(*,*) part, timeStep
  part(1) = part(1) + timeStep*part(3)
  part(2) = part(2) + timeStep*part(4)

end subroutine feuler


