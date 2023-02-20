program main
    use profiler_mod
    use omp_lib
    use mpi

    implicit none

    integer :: ierr
    integer :: comm
    integer :: crank
    integer :: cthread

    integer (kind=pik) :: prof_print

    ! Initialise MPI
    call MPI_Init(ierr)
    comm = MPI_COMM_WORLD

    ! Get current MPI rank
    call MPI_Comm_rank(comm, crank, ierr)

    ! Begin OpenMP region
    !$OMP PARALLEL DEFAULT(NONE) SHARED(crank,prof_print) PRIVATE(cthread)
        
        ! Get current OMP thread
        cthread = omp_get_thread_num()

        ! Profile a simple print statement
        call profiler_start(prof_print, "print statement")
        !$OMP CRITICAL
            print *, 'MPI rank ', crank, ', OMP thread ', cthread
        !$OMP END CRITICAL
        call profiler_stop(prof_print)
        
    !$OMP END PARALLEL

    ! Finish
    call MPI_Finalize(ierr)
    call profiler_write()

end program main
