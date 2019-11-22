PROGRAM to_be_improved

  INTEGER :: i,j
  INTEGER, PARAMETER :: n=2000, m=2000
  REAL(KIND=8) ::  a(n,m)
  REAL(KIND=8) ::  t1,t2

  CALL cpu_time(t1)
  DO I=1, 2000
     DO J=1, 2000
        A(I,J) = I + J
     ENDDO
  ENDDO
  CALL cpu_time(t2)
  
  PRINT*,'total time=',t2-t1
END PROGRAM to_be_improved
