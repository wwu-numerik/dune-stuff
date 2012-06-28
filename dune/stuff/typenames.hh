#ifndef DUNE_STUFF_TYPENAMES_HH
#define DUNE_STUFF_TYPENAMES_HH

#define STUFF_TYPENAME(NAME) \
    template <> \
    struct Typename<NAME> { \
        static const char* value() { \
            return #NAME;  \
        } \
    };

namespace Stuff {
    template <typename T>
    struct Typename {
        static const char* value() {
            #if defined(__GNUC__) && defined(__GXX_RTTI)
                return typeid(T).name();
            #else
                return "unknown";
            #endif
        }
    };

    STUFF_TYPENAME(int);
    STUFF_TYPENAME(double);
    STUFF_TYPENAME(float);
    STUFF_TYPENAME(long);
    STUFF_TYPENAME(unsigned int);
    STUFF_TYPENAME(unsigned long);
    STUFF_TYPENAME(char);
}

#endif // DUNE_STUFF_TYPENAMES_HH
