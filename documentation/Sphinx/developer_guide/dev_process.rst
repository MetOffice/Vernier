.. -----------------------------------------------------------------------------
    (c) Crown copyright 2025 Met Office. All rights reserved.
    The file LICENCE, distributed with this code, contains details of the terms
    under which the code may be used.
   -----------------------------------------------------------------------------

Development Guidelines
======================

Below are details on the development process to be used when working on Vernier.

If you have not used Git version control for development before, it is
recommended you check out `Version Control with Git <https://metoffice.
github.io/git-novice/index.html>`_ and `Git and GitHub Working Practices
<https://metoffice.github.io/git-working-practices/index.html>`_.

Issues and/or Pull Requests
---------------------------

The development of Vernier can be tracked via GitHub issues and/or pull requests.
Having an issue for every pull request (PR) is not mandatory, but a pull request
is required for merging changes to the main branch.

For instance, a simple bug fix can be provided directly via a PR. Larger pieces
of work should be discussed in an issue first, and then a PR can be created
once the changes are ready for review. If discussion of a feature is in its
earliest stages it may be more appropriate to start the discussion on the
Vernier GitHub `Discussions page <https://github.com/MetOffice/Vernier/
discussions>`_.

Development Process
-------------------

All development **Must** be done on a fork of the main repository. It is
recommended that you create a new branch for each issue or PR. This allows you
to work on multiple issues at the same time, and also allows you to keep your
main branch clean.

#. Create an issue to document the needed changes if appropriate. If in doubt
   it is better to create an issue than not.
#. Create a branch on your fork of the repository.
#. Develop the changes on your branch in accordance with the Vernier code
   style (This is enforced via Clang Format as part of the CI testing).
#. Make sure the changes are tested appropriately. This may include
   unit tests and/or system tests. See the :ref:`testing` section for more
   information.
#. Update of add any documentation as needed. This may include updating the
   Sphinx documentation and/or Doxygen API documentation in the source code.
#. Once you're happy your changes are ready to be merged into main please
   follow the guidelines in the :ref:`review_process` section.