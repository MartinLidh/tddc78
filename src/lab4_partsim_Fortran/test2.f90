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



