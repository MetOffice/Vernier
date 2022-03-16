!-----------------------------------------------------------------------------
! (C) Crown copyright 2022 Met Office. All rights reserved.
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-----------------------------------------------------------------------------
program main
use profiler_mod

implicit none
    integer :: hash


    hash = profiler_start("test")
    call profiler_stop(hash)
    call profiler_write

end program main