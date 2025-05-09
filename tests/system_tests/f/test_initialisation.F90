!-------------------------------------------------------------------------------
! (c) Crown copyright 2025 Met Office. All rights reserved.
! The file LICENCE, distributed with this code, contains details of the terms
! under which the code may be used.
!-------------------------------------------------------------------------------

program test_initialisation
  use vernier_mod, only: vik, &
                         vernier_init, vernier_finalize, &
                         vernier_start, vernier_stop
  use mpi

  implicit none

  integer :: ierr
  integer(vik) :: hash_out

  ! Initialise MPI (Must happen before initialising Vernier)
  call MPI_Init(ierr)

  ! Initialise Vernier with the default communicator (i.e. pass no communicator)
  call vernier_init()

  call vernier_start(hash_out, "test_initialisation")
  ! Do some work here
  call sleep(1)
  call vernier_stop(hash_out)

  Call vernier_finalize()
call mpi_finalize(ierr)

end program test_initialisation