FileTransfer Tool Server
========================

The Ansys FileTransfer Tool is a minimal gRPC API for transferring files between
a client and a remote server.

This repository provides the server component of the FileTansfer Tool.

**WARNING**:

The FileTransfer Tool does not provide any security measures. Any file
on the server component can be accessed by any client. Without additional security
measures, it is unsuited for use over an untrusted network.

Documentation and issues
------------------------

Documentation for the latest release of the FileTransfer Tool Server is hosted at
`FileTransfer Tool Server documentation <https://filetransfer-server.tools.docs.pyansys.com>`_.

The FileTransfer Tool Server documentation contains these sections:

- `Getting started <https://filetransfer-server.tools.docs.pyansys.com/version/dev/intro.html>`_:
  Explains how to build and run the FileTransfer Tool Server.
- `Contribute <https://filetransfer-server.tools.docs.pyansys.com/version/dev/contribute.html>`_:
  Explains how to set up a development environment and provides documentation for the internal
  classes and functions of the FileTransfer Tool Server for developers who want to understanding
  the inner workings of the server and possibly extend it.

On the `FileTransfer Tool Server Issues <https://github.com/ansys-internal/ansys-tools-filetransfer-server/issues>`_
page, you can create issues to report bugs and request new features. On the `Discussions <https://discuss.ansys.com/>`_
page on the Ansys Developer portal, you can post questions, share ideas, and get community feedback.

To reach the project support team, email `pyansys.core@ansys.com <pyansys.core@ansys.com>`_.
