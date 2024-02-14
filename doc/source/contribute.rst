.. _ref_contributing:

Contribute
----------

Overall guidance on contributing to a PyAnsys library appears in the
`Contributing <https://dev.docs.pyansys.com/overview/contributing.html>`_ topic
in the *PyAnsys Developer's Guide*. Ensure that you are thoroughly familiar with
it and all `Guidelines and Best Practices
<https://dev.docs.pyansys.com/guidelines/index.html>`_ before attempting to
contribute to the Filetransfer Tool Server repository.


Setup
~~~~~

* See the :ref:`getting_started` section for instructions on how to build the server from source.
* The documentation can be built using the following command:

  .. code-block:: bash

    poetry run -C doc html

* The tests can be run using the following command:

  .. code-block:: bash

    cd build; ctest; cd ..

* Pre-commit style checks can be run using the following command:

  .. code-block:: bash

    poetry run pre-commit run --all-files


Internal API
~~~~~~~~~~~~

This section contains the documentation of the internals of the file transfer server. It is intended for developers who
want to understand the inner workings of the server and possibly extend it.

.. doxygennamespace:: file_transfer
   :project: ansys-tools-filetransfer-server
   :members:
   :private-members:
