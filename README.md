
PlanarRad 0.9.1beta (Jude 2.1.0 library)
(c) 1999-2013 John Hedley

Prerequisites
-------------

The tools automake, autoconf and libtool must be installed to build PlanarRad.
These are usually already installed on most systems.

The Boost library must also be installed, again it most likely already is.

Qt is required for the GUI tools to be built, but not for the command line versions.

On Ubuntu libsm-dev might need to be installed.


Fixing Compilation On Newer Distros
-----------------------------------

The compilation process needs to know where various components of QT are, the autoconf scripts are not very good at finding these. The easiest way is to make a directory "qt4" and in it make symlinks to required components. For example on Ubuntu 12.10:

mkdir qt4
cd qt4
ln -s /usr/bin bin
ln -s /usr/include/qt4 include
ln -s /usr/lib/x86_64-linux-gnu lib

So it looks like this:

$ ls -l qt4
total 0
lrwxrwxrwx 1 user user  8 Jul 12 10:59 bin -> /usr/bin
lrwxrwxrwx 1 user user 16 Jul 12 11:14 include -> /usr/include/qt4
lrwxrwxrwx 1 user user 25 Jul 12 11:08 lib -> /usr/lib/x86_64-linux-gnu

Then edit 'example_build' so that the configuration points to the full path to qt4. For example there should be a line like this:

../configure --prefix=/usr/local/jude_test --with-qtdir=/home/user/jude2/qt4

Be sure to use the full path starting with a '/' otherwise it won't work.

On Fedora 19 the layout is a bit different and the QT executable tools are called something different so must be invidually symlinked inside a directory called "bin". Should look like this:

$ ls -l /usr/local/qt4/
total 4
drwxr-xr-x. 2 root root 4096 Sep  2 00:30 bin
lrwxrwxrwx. 1 root root   12 Sep  2 00:19 include -> /usr/include
lrwxrwxrwx. 1 root root   10 Sep  2 00:21 lib -> /usr/lib64
$ ls -l /usr/local/qt4/bin/
total 0
lrwxrwxrwx. 1 root root 16 Sep  2 00:23 moc -> /usr/bin/moc-qt4
lrwxrwxrwx. 1 root root 18 Sep  2 00:22 qmake -> /usr/bin/qmake-qt4
lrwxrwxrwx. 1 root root 12 Sep  2 00:30 rcc -> /usr/bin/rcc
lrwxrwxrwx. 1 root root 16 Sep  2 00:23 uic -> /usr/bin/uic-qt4


Very Quick installation
-----------------------

The script file 'example_build' will build and test the code ready for installation.
To change where the files will be installed edit 'example_build' and change the configure option

--prefix=/usr/local/jude_test

to whatever directory you want to be the base of the installation.
All installed files will be put there.

After running example_build you need to type:

cd build
make install

to actually copy the built files to the installation location.
This may need to be done with superuser priviliges depending on the installion directory location.


Quick installation
------------------

Change to the top level directory (i.e the one where this file is).
Then type:

./autogen.sh
mkdir build
cd build
../configure
make
make install

To specify where things are installed, and also to specify the location of QT arguments 
such as the following can be supplied to ./configure instead of what is shown above.

./configure --prefix=/usr/local --bindir=/usr/local/bin --with-qtdir=/usr/local/qt

To build the command line tools only without the requiremnet for QT supply the option

--disable-qt

instead of the --with-qtdir option.

if you have doxygen installed the source code documentation can be built by typing:

doxygen doxygen.cfg

It can then be found in in the directory 'docs'. At the time of writing it is not very complete.


Testing
-------

Each library or tool may have an associated test program which can be found in directory 
'build/<libname>/testing'. So for example, to run tests for jude::base library.

cd build/base/testing
./test_base

A script is provided to run all the provided test programs at once.

cd build/testing
./test_script

Sometimes tests will fail but it's not a problem. This is especially true of test in the base 
library that are to do with number representations. Usually the failures are just due to 
rounding errors on the least-significant bits.


Building HydroLight Comparison Document
---------------------------------------

This requires that the system has gnuplot and latex installed, on Ubuntu install texlive-latex-extra.

First make sure the environmental variable JUDE2DIR is set to where the bin directory containg the planarrad_free executable can be found.
For example do this:

export JUDE2DIR=/usr/local/jude_test

(Or you can add this to your .bash_profile file)

Which would mean the executable is at /usr/local/jude_test/bin/planarrad_free

Then find the testing directory in the planarrad source files and run the process script:

cd testing/slabtool_test02
./process

It takes about an hour or so but the result is the file "pl_hr_compare.pdf" which shows many plots of a set of HydroLight runs, found under testing/slabtool_test02/hl_original_data, and the equivalent PlanarRad runs. They should show more or less exact correspondence, except where values are very small. There are some small differences for wind blown water surfaces (reason unknown).
