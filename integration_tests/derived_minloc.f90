module oalib_issue1_mod
  implicit none
  type :: arrival_t
    integer :: ntopbnc, nbotbnc
    real :: a, phase
    complex :: delay
  end type
  type(arrival_t), allocatable :: arr(:, :, :)
  integer, allocatable :: narr(:, :)
  integer :: maxnarr
contains
  subroutine add_arr(id, ir, amp)
    integer, intent(in) :: id, ir
    real, intent(in) :: amp
    integer :: iarr(1), nt
    nt = narr(id, ir)
    if (nt >= maxnarr) then
      iarr = minloc(arr(id, ir, :)%a)
      if (amp > arr(id, ir, iarr(1))%a) then
        arr(id, ir, iarr(1))%a = amp
      end if
    end if
  end subroutine
end module

program oalib_issue1_derived_minloc
  use oalib_issue1_mod
  implicit none
  maxnarr = 5
  allocate(arr(2, 2, maxnarr), narr(2, 2))
  narr = 0
  arr%a = 1.0
  narr(1, 1) = 5
  call add_arr(1, 1, 2.0)
  if (abs(arr(1, 1, 1)%a - 2.0) > 1.0e-6) error stop
end program
