.. -----------------------------------------------------------------------------
    (c) Crown copyright 2025 Met Office. All rights reserved.
    The file LICENCE, distributed with this code, contains details of the terms
    under which the code may be used.
   -----------------------------------------------------------------------------

.. _review_process:

Review Process
==============

The review process for Vernier is based on the GitHub pull request system. This
allows for a collaborative review process where multiple developers can provide
feedback on a pull request before it is merged into the main branch. The
review process is designed to ensure that code changes are thoroughly reviewed
and tested before they are merged into the main branch. This helps to maintain
the quality and stability of the codebase and ensures that all changes are
consistent with the overall design and architecture of the project.
The review process is as follows:

#. **Create a pull request**: When a developer has completed a set of changes,
   they create a pull request on GitHub. This pull request should include a
   description of the changes made and any relevant information about the issue
   or feature being implemented. As well as ``closes #13`` style `keyword
   <https://docs.github.com/en/issues/tracking-your-work-with-issues/
   using-issues/linking-a-pull-request-to-an-issue#linking-a-pull-request-
   to-an-issue-using-a-keyword>`_ linking of the pull request to the issue it
   addresses. This will automatically close the associated issues and helps
   to keep track of which issues have been resolved and which are still open.
#. **Check the PR is viable** Make sure all the required checks have passed and
   the branch is up to date with the main branch. If the PR cannot be merged
   automatically, the developer should update the branch to bring it up to date.
   It is the developers job in all instances to ensure the PR is mergeable.
#. **Request a review**: The developer should request a review from one or
   more reviewers if they have permissions to do so, otherwise the PR will be
   picked up and assigned by a maintainer.
#. **Review the changes**: The reviewers should review the changes made in the
   pull request. This includes checking for code quality, correctness, and
   adherence to coding standards. Reviewers should also check that the changes
   are well documented and that any relevant tests have been added or updated.
#. **Address feedback**: The developer should address any feedback provided by
   the reviewers. Once the feedback has been addressed, the developer should
   update the pull request with the changes made once again checking the PR is
   viable. Once satisfied their change address the reviewers comments the
   developer should request another review from the same reviewer using
   the :octicon:`sync;1em` icon next to the reviewers name.
#. **Approve the pull request**: Once the reviewers are satisfied with the
   changes made in the pull request, they should approve the pull request.
   This indicates that the changes are ready to be merged into the main branch.
#. **Merge the pull request**: Once the pull request has been approved, the
   reviewer can merge the changes into the main branch using the
   **Squash and merge** button.
#. **Delete the branch**: After the pull request has been merged, the developer
   can delete their branch.
#. **Celebrate**: Congratulations! You have successfully completed the review
   process and your changes have been merged into the main branch.

