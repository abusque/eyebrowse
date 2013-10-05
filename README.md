# eyebrowse

Eye tracking UI control

## Compiling and Running

To compile and run eyebrowse, you will need CMake, make, and OpenCV 2.4.
From the root of this repo, execute the following commands:

       $ mkdir build
       $ cd build/
       $ cmake ../
       $ make

This  will produce the executable *eyebrowse* under *build/bin/*. However, you
will need to copy the *haarcascade_frontalface_alt.xml* from the root of the
repo to  the directory where your executable is for it to run. This step will
be automated eventually.
