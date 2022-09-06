      program main
      integer a
      a = 1

      if (a == 1) return
      if (a .le. 2) then
          print *, 1
      else if (a .le. 3) then
          print *, 2
      else if (a .le. 4) then
          print *, 3
      else
          print *, 4
  100     continue
  200     continue
      endif
      end 

