.. _ref_contributing:

Contribute
----------

Overall guidance on contributing to a PyAnsys library appears in the
`Contributing <https://dev.docs.pyansys.com/overview/contributing.html>`_ topic
in the *PyAnsys developer's guide*. Ensure that you are thoroughly familiar with
this guide before attempting to contribute to the FileTransfer Tool Server.


Setup
~~~~~

* For information on how to build the FileTransfer Tool Server from source, see
  :ref:`getting_started`.

* To build the documentation, use this command:

  .. code-block:: bash

    uv run make -C doc html

* To run tests, use this command:

  .. code-block:: bash

    cd build; ctest; cd ..

* To run ``pre-commit`` style checks, run this command:

  .. code-block:: bash

    uv run pre-commit run --all-files


Internal API
~~~~~~~~~~~~

Documentation follows for the internal classes and functions of the FileTransfer Tool Server.
This documentation is intended for developers who want to understand the inner workings
of the server and possibly extend it.

.. doxygennamespace:: file_transfer
   :project: ansys-tools-filetransfer-server
   :members:
   :private-members:
