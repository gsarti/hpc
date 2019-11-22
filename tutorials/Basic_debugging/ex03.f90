
PROGRAM to_be_improved
  IMPLICIT NONE

  INTEGER :: i,j
  INTEGER, PARAMETER :: n=1000, m=1000
  REAL(KIND=8) :: a(n,m), b(2*n,2*m)
  REAL(KIND=8) :: t1,t2, sa, sb
  REAL(KIND=8), EXTERNAL :: dasum
  COMMON /align/  a,b

  CALL azzero(a,n*m)
  CALL azzero(b,4*n*m)
  sa=dasum(n*m,a,1)
  sb=dasum(4*n*m,b,1)
  PRINT*, 'sum0: a = ', sa, '  b = ', sb
  CALL cpu_time(t1)
  DO I=1, 1010
     DO J=1, 1010
        A(I,J) = I + J
     ENDDO
  ENDDO
  sa=dasum(n*m,a,1)
  DO I=1, 1010
     DO J=1, 1010
        B(I,J) = I - J
     ENDDO
  ENDDO
  sb=dasum(4*n*m,b,1)
  PRINT*, 'sum1: a = ', sa, '  b = ', sb

  CALL azzero(a,n*m)
  CALL azzero(b,4*n*m)
  DO I=1, 1010
     DO J=1, 1010
        B(I,J) = I - J
     ENDDO
  ENDDO
  sb=dasum(4*n*m,b,1)
  DO I=1, 1010
     DO J=1, 1010
        A(I,J) = I + J
     ENDDO
  ENDDO
  sa=dasum(n*m,a,1)
  CALL cpu_time(t2)
  PRINT*, 'sum2: a = ', sa, '  b = ', sb
  
  PRINT*,'total time=',t2-t1
END PROGRAM to_be_improved
