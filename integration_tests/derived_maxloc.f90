module oalib_issue2_mod
  implicit none
  type :: hsinfo_t
    real(kind=8) :: alphar, rho
  end type
  type :: bdrypt_t
    real(kind=8) :: x(2), t(2), n(2)
    real(kind=8) :: len, kappa
    type(hsinfo_t) :: hs
  end type
  type(bdrypt_t), allocatable :: top(:)
contains
  subroutine get_top_seg(r, iseg)
    real(kind=8), intent(in) :: r
    integer, intent(out) :: iseg
    integer :: isegtopt(1)
    isegtopt = maxloc(top(:)%x(1), top(:)%x(1) < r)
    iseg = isegtopt(1)
  end subroutine
end module

program oalib_issue2_derived_maxloc
  use oalib_issue2_mod
  implicit none
  integer :: iseg
  allocate(top(4))
  top(1)%x = [-1.0d6, 0.0d0]
  top(2)%x = [0.0d0, 0.0d0]
  top(3)%x = [1.0d3, 0.0d0]
  top(4)%x = [1.0d6, 0.0d0]
  call get_top_seg(500.0d0, iseg)
  if (iseg /= 2) error stop
end program
