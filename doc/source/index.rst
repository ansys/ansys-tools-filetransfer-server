.. toctree::
   :hidden:
   :maxdepth: 2

   intro
   contribute

Filetransfer tool server
========================

The Filetransfer Tool is a minimal gRPC API for transferring files between a client and a remote server.

This documentation describes the server component of the Filetransfer Tool.

.. warning::

   The Filetransfer Tool does not provide any security measures. Any file
   on the server component can be accessed by any client. Without additional security
   measures, it is unsuited for use over an untrusted network.


.. grid:: 1 1 2 2
    :gutter: 2

    .. grid-item-card::  Getting started :fa:`person-running`
        :link: intro
        :link-type: doc

        Explains how to build and run the Filetransfer Tool Server.

    .. grid-item-card:: Contribute :fa:`people-group`
        :link: contribute
        :link-type: doc

        Explains how to set up a development environment, and describes the internal classes and functions of the Filetransfer Tool Server.
