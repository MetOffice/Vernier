!-----------------------------------------------------------------------------
! (C) Crown copyright 2022 Met Office. All rights reserved.
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-----------------------------------------------------------------------------
program main

use, intrinsic::iso_c_binding
use profiler_mod
use OMP_LIB

implicit none
    integer :: hash, i

!$OMP PARALLEL
    hash = profiler_start("test name")

    do i= 0, 10000
        write(*,*) hash + i
    end do

    call profiler_stop(hash)
!$OMP END PARALLEL
    call profiler_write

end program main