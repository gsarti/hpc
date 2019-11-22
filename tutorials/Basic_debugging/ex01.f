      program to_be_improved

      integer i,j,n,m
      parameter (n=1000,m=1000)
      real*8  a(n,m)
      real*8  t1,t2

      call cpu_time(t1)
      DO I=1, 1000
         DO J=1, 1000
           A(I,J) = I + J
         ENDDO
      ENDDO
      call cpu_time(t2)
  
      print*,'total time=',t2-t1
      end 
