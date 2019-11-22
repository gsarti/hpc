      include 'helper.f'

      program to_be_improved
      implicit none
      integer i,j,n,m
      parameter (n=1000,m=1000)
      real*8  a(n,m), b(2*n,2*m), sa, sb
      real*8  t1,t2,dasum
      common  /align/ a,b

      call azzero(a,n*m)
      call azzero(b,4*n*m)
      sa=dasum(n*m,a,1)
      sb=dasum(4*n*m,b,1)
      print*, 'sum0: a = ', sa, '  b = ', sb
      call cpu_time(t1)
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
      print*, 'sum1: a = ', sa, '  b = ', sb

      call azzero(a,n*m)
      call azzero(b,4*n*m)
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
      call cpu_time(t2)
      print*, 'sum2: a = ', sa, '  b = ', sb
  
      print*,'total time=',t2-t1
      end 
