module profiler_mod

    implicit none

    public :: profiler_start, profiler_stop

contains

    integer function profiler_start(name) result(hash)
        character(*), intent(in) :: name
        integer c_profiler_start
        external c_profiler_start
        write(*,*) "Start profiling"
        hash = c_profiler_start(name)
        write(*,*) hash
    end function profiler_start

    subroutine profiler_stop(hash)
        integer, intent(in) :: hash
        write(*,*) hash
        call c_profiler_stop(%VAL(hash))
    end subroutine profiler_stop

    subroutine profiler_write()
        call c_profiler_write()
    end subroutine profiler_write

end module profiler_mod