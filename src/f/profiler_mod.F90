module profiler_mod
    implicit none

    public :: test

contains

    subroutine test()
        write(*,*) "Test program"
    end subroutine test

end module profiler_mod