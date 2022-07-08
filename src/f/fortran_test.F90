!-------------------------------------------------------------------------------
! (c) Crown copyright 2022 Met Office. All rights reserved.
!
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

program test_profiler_f
use profiler_mod
use omp_lib
implicit none

integer, parameter :: dp = 8

!Timer declarations
real(kind=dp) :: t1, t2, actual_time

!real(kind=dp) :: prof_time

!Handle declarations
integer(kind=pik) :: prof_main

!Start timing: noddy way, and using Profiler.
call profiler_start(prof_main, 'MAIN')
t1 = omp_get_wtime()

! Time a region
block 
  integer(kind=pik) :: prof_sub
  call profiler_start(prof_sub, 'MAIN_SUB')
  call sleep(1);
  call profiler_stop(prof_sub)
end block

! Time nested regions on many threads.
!$OMP PARALLEL
block
  integer(kind=pik) :: prof_sub
  integer(kind=pik) :: prof_sub2
  call profiler_start(prof_sub, "MAIN_SUB")
  call sleep(1);

  ! Time nested region
  call profiler_start(prof_sub2, "MAIN_SUB2")
  call sleep(1);
  call profiler_stop(prof_sub2)

  ! Outer region end.
  call profiler_stop(prof_sub)
end block
!$OMP END PARALLEL

!Give the main regions some substantial execution time.
call sleep(2);

! End of profiling; record t2 immediately afterwards.
call profiler_stop(prof_main)
t2 = omp_get_wtime();

!Write the profile
call profiler_write()

!Check that the total time measured by the profiler is within some tolerance
!of the actual time measured by simple t2-t1.  This only tests the top-level
!timing, not individual subroutine timings.
!double const time_tolerance = 0.0001;

actual_time = t2 - t1

print *, "Actual timing: ",   actual_time

end program test_profiler_f

