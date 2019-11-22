PROGRAM to_be_improved
  IMPLICIT NONE

  INTEGER :: i,j
  INTEGER, PARAMETER :: n=10, m=10
  REAL(KIND=8) ::  a(n,m), b(n,m)
  REAL(KIND=8) ::  t1,t2

  CALL cpu_time(t1)
  DO I=1, 10
     DO J=1, 11
        A(I,J) = I + J
        print*, a(i,j),i,j
     ENDDO
  ENDDO
  CALL cpu_time(t2)
  
  PRINT*,'total time=',t2-t1
END PROGRAM to_be_improved
