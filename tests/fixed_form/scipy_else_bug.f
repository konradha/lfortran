      program main 
       IF (N.GE.2.AND.N.LE.INT(0.9*X)) THEN
           F0=BJV0
           F1=BJV1
           DO 45 K=2,N
              F=2.0D0*(K+V0-1.0D0)/X*F1-F0
              F0=F1
              F1=F
45            VL(K)=F
        ELSE IF (N.GE.2) THEN
           M=MSTA1(X,200)
           IF (M.LT.N) THEN
              N=M
           ELSE
              M=MSTA2(X,N,15)
           ENDIF
           F=0.0D0
           F2=0.0D0
           F1=1.0D-100
           DO 50 K=M,0,-1
              F=2.0D0*(V0+K+1.0D0)/X*F1-F2
              IF (K.LE.N) VL(K)=F
              F2=F1
50            F1=F
           CS=0.0D0
c     cs = 0.0d0
c       if (abs(bjv0) > abs(bjv1)) cs = bjv0 / f
c       elsecs = bjv1 / f2
           IF (DABS(BJV0).GT.DABS(BJV1)) CS=BJV0/F
           ELSE CS=BJV1/F2
           DO 55 K=0,N
55            VL(K)=CS*VL(K)
        ENDIF
      end program
      
