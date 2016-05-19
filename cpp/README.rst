C++ Sudoku solver
=================

Build and test by doing for example::

    $ mkdir build
    $ cd build/
    $ cmake -D CMAKE_BUILD_TYPE=Release ..
    -- The CXX compiler identification is GNU 6.1.1
    -- Check for working CXX compiler: /usr/bin/c++
    -- Check for working CXX compiler: /usr/bin/c++ -- works
    -- Detecting CXX compiler ABI info
    -- Detecting CXX compiler ABI info - done
    -- Detecting CXX compile features
    -- Detecting CXX compile features - done
    -- Configuring done
    -- Generating done
    -- Build files have been written to: [...]
    $ make
    Scanning dependencies of target solver
     Building CXX object CMakeFiles/solver.dir/main.cpp.o
     Linking CXX executable solver
     Built target solver
    $ ./solver < ../../example7.txt 
    1 4 5 3 2 7 6 9 8 
    8 3 9 6 5 4 1 2 7 
    6 7 2 9 1 8 5 4 3 
    4 9 6 1 8 5 3 7 2 
    2 1 8 4 7 3 9 5 6 
    7 5 3 2 9 6 4 8 1 
    3 6 7 5 4 2 8 1 9 
    9 8 4 7 6 1 2 3 5 
    5 2 1 8 3 9 7 6 4 
    milliseconds: 0.092816
    per second: 10774


