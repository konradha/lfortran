      subroutine dt1()
      double precision coef(5,4)
      !DATA (coef(i,1),i=1,5)/1.0D0,1.0D0,3*0.0D0/
      !DATA (coef(i,1),i=1,5)/1.0D0,1.0D0,0.0D0,0.0D0, 0.0D0/
      ! invalid: no actual expression as factors, only constants -- DATA (coef(i,1),i=1,5)/(1+1)*0.0D0, 1*1.0D0, 2*2.0D0/
      ! invalid (error case): DATA (coef(J,1),i=1,5)/2*0.0D0, 1*1.0D0, 2*2.0D0/ 
      ! the below is valid, too:
      !double precision coef(5,5)
      !DATA (coef(i,i),i=1,5)/2*0.0D0, 1*1.0D0, 2*2.0D0/

      integer i
      DATA (coef(i,1),i=1,5)/2*0.0D0, 1*1.0D0, 2*2.0D0/
      do i=1,5
        print *, coef(i, 1)
      end do
      END


      program main 
      call dt1()
      end program