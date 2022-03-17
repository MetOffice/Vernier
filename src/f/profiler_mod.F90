module profiler_mod

    interface

        integer(c_int) function profiler_start(name) &
          bind(C, name='c_profiler_start')
            use, intrinsic :: iso_c_binding
            implicit none
            character(c_char) :: name
        end function profiler_start

        subroutine profiler_stop(hash) &
          bind(C, name='c_profiler_stop')
            use, intrinsic :: iso_c_binding
            integer(c_int), value :: hash
        end subroutine profiler_stop

        subroutine profiler_write() &
          bind(C, name='c_profiler_write')
            use, intrinsic :: iso_c_binding
        end subroutine profiler_write

    end interface

end module profiler_mod