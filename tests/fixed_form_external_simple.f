c compiles in gfortran with "missing symbols" error -> LFortran will
c replicate this behavior 
      subroutine outer_scope(a)
          integer a
          print *, a
      end subroutine

      program main
          external inner_scope
          integer b
          call inner_scope(b) ! this should fail in compilation, not earlier
          
      end program

