      program main 

c     ILLEGAL
c       COMMON /X/ A 
c       COMMON /Y/ B 
c       EQUIVALENCE ( A, B)	

c     LEGAL
       DIMENSION A(5)
       COMMON /X/ B
       EQUIVALENCE ( B, A)
      end program

