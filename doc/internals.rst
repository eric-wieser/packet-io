Internals
=========

Callback functions
------------------

The :class:`LambdaPointer` class is used to provide contextfull callback functions, using c++11 lambdas

.. doxygenclass:: packetio::LambdaPointer< Out(In...)>
    :members:

Mirror of the Arduino interface
-------------------------------

These classes, found in :file:`_compat`, allow testing on the desktop, and potentially execution on
non-Arduino platforms

.. doxygenclass:: Print
    :members:
    :undoc-members:

.. doxygenclass:: Stream
    :members:
    :undoc-members:
