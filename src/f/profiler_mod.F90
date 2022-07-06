module profiler_mod
  use, intrinsic :: iso_c_binding, only: c_char, c_long
  implicit none

  !------------------------------------------------------------------------------
  ! Public parameters
  !------------------------------------------------------------------------------

  integer, parameter :: pik = c_long
  
  !------------------------------------------------------------------------------
  ! Public interfaces
  !------------------------------------------------------------------------------

  interface

    subroutine profiler_start(hash_out, name)             &
        bind(C, name='c_profiler_start')
          import :: c_char, c_long
          character(kind=c_char, len=1), intent(in)  :: name
          integer(kind=c_long),          intent(out) :: hash_out
    end subroutine profiler_start

    subroutine profiler_stop(hash_in)                   &
      bind(C, name='c_profiler_stop')
        import :: c_long
        integer(kind=c_long), intent(in) :: hash_in
    end subroutine profiler_stop

    subroutine profiler_write()                         &
      bind(C, name='c_profiler_write')
    end subroutine profiler_write

  end interface

end module profiler_mod
