#ifndef STUFF_DEPRECATED_HH
#define STUFF_DEPRECATED_HH

#include <dune/common/deprecated.hh>

#ifndef DUNE_DEPRECATED_MSG

#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define DUNE_DEPRECATED_MSG(text)                        \
  __attribute__((deprecated(text)))
#else
#define DUNE_DEPRECATED_MSG(text)        DUNE_DEPRECATED
#endif /* __GNUC__ */

#endif

#endif // DEPRECATED_HH
