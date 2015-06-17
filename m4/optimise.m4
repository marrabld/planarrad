AC_DEFUN([OPTIMISE],
[

# -pg for profiling (use command line run only!) -g for debugging
# -DFASTTABLE effects only spherical partitions and forces then to have hn in the form 2^n
# sse stuff = -O2 -ftree-vect1orize -msse2 -ftree-vectorizer-verbose=6
# spectral band config options: (* default):
# -DSPECTRALDATA_TYPE=FLOAT,DOUBLE*
# -DSPECTRALDATA_MODE=MONO,RGB,MULTI*,FIXED  - RGB and MONO probably defunct
# -DFIXED_BANDS=34
# cublas -DUSE_CUBLAS and need to find cuda to link like -L/usr/local/cuda/lib -lcublas

NORMAL_FLAGS=" -Wall -g"

OPTIMISED_FLAGS=" -Wall -DNDEBUG -O2 -ffast-math -fno-math-errno -funsafe-math-optimizations -ffinite-math-only -fno-trapping-math -fno-strict-aliasing"

#AX_CHECK_COMPILER_FLAGS([-O2], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -O2"], )
#AX_CHECK_COMPILER_FLAGS([-ffast-math], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -ffast-math"], )
#AX_CHECK_COMPILER_FLAGS([-fno-math-errno], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -fno-math-errno"], )
#AX_CHECK_COMPILER_FLAGS([-funsafe-math-optimizations], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -funsafe-math-optimizations"], )
#AX_CHECK_COMPILER_FLAGS([-ffinite-math-only], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -ffinite-math-only"], )
#AX_CHECK_COMPILER_FLAGS([-fno-trapping-math], [OPTIMISED_FLAGS="$OPTIMISED_FLAGS -fno-trapping-math"], )

AC_MSG_CHECKING([optimised compile (--enable-optimised)])

AC_ARG_ENABLE(optimised, 
  [  --enable-optimised=yes   Optimised compile],
  [
    CXXFLAGS=$OPTIMISED_FLAGS
    AC_MSG_RESULT(yes)
  ], [
    CXXFLAGS=$NORMAL_FLAGS
    AC_MSG_RESULT(no)
  ]
)

])
