c compiles in gfortran with "missing symbols" error -> LFortran will
c replicate this behavior 
      program main
          external fn
          integer a
          call fn(a)
      end program

