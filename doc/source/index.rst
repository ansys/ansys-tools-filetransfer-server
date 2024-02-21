.. toctree::
   :hidden:
   :maxdepth: 2

   intro
   contribute

Ansys FileTransfer Tool Server documentation
============================================

The Ansys FileTransfer Tool is a simple gRPC API for moving files between a
client and a remote server. Its target use case are local Docker deployments or
simple remote deployments.

This documentation describes the server component of the FileTransfer Tool. For
information on the tool itself, see the
`FileTransfer Tool documentation <https://filetransfer.tools.docs.pyansys.com/version/dev/index.html>.

.. warning::

   The FileTransfer Tool does not provide any security measures. Any file
   on the server component can be accessed by any client. Without additional security
   measures, it is unsuited for use over an untrusted network.


.. grid:: 1 1 2 2
    :gutter: 2

    .. grid-item-card::  Getting started :fa:`person-running`
        :link: intro
        :link-type: doc

        Explains how to build and run the FileTransfer Tool Server.
    .. grid-item-card:: Contribute :fa:`people-group`
        :link: contribute
        :link-type: doc

        Explains how to set up a development environment and provides documentation for
        the internal classes and functions of the FileTransfer Tool Server for developers
        who want to understanding the inner workings of the server and possibly extend it.
