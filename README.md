# cutil
[![Build Status](https://travis-ci.org/matthewcpp/cutil.svg?branch=master)](https://travis-ci.org/matthewcpp/cutil) [![Build status](https://ci.appveyor.com/api/projects/status/skgkjns111qnmxr9/branch/master?svg=true)](https://ci.appveyor.com/project/matthewcpp/cutil/branch/master) [![Coverage Status](https://coveralls.io/repos/github/matthewcpp/cutil/badge.svg?branch=master)](https://coveralls.io/github/matthewcpp/cutil?branch=master) [![Documentation Status](https://readthedocs.org/projects/cutil/badge/?version=master)](https://cutil.readthedocs.io/en/master/?badge=master)


### About
This is a small C utility library inspired by the c++ stl that can be used in C 89/90 projects.  The library uses the concept of traits which describe the data that is being stored in the containers.

#### Containers:
- [list](https://cutil.readthedocs.io/en/master/list_8h.html): Doubly linked list
- [vector](https://cutil.readthedocs.io/en/master/vector_8h.html): Dynamic vector
- [btree](https://cutil.readthedocs.io/en/master/btree_8h.html): Self balancing search tree

### Building
In order to build the library, CMake 3.11 or greater is required.  To easily build the library run the following commands in your terminal:
```bash
mkdir build
cd build
cmake ..
```
