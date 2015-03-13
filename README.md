**matlab_obj - Jonathan's Matlab obj file reader**
---------
---------

**Overview**
--------

This is just a mex wrapper around a C++ obj file reader.  I found that all obj readers on the Mathworks website were really slow, so I wrapped a fast C++ version.

**Compilation**
---------------

Run "compile_mex.m".

**Running**
---------------

Run "test_mex.m" for a usage demo.  obj_read(filename) returns a struct with the loaded channels from the obj file (faces, vertices, normals, etc). 

**Style**
---------

Where possible this project follows the Google C++ style conventions: 

<http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml>
