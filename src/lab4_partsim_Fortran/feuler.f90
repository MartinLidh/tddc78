subroutine feuler(part, timeStep)
  ! Asuming part has (x,y,vx,vy)
  implicit none
  double precision, intent(inout), dimension(4) :: part
  double precision, intent(in) :: timeStep
!  write(*,*) part, timeStep
  part(1) = part(1) + timeStep*part(3)
  part(2) = part(2) + timeStep*part(4)

end subroutine feuler
