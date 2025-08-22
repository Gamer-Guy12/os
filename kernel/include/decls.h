#ifndef DECLS_H
#define DECLS_H

#define PACKED
#define CONST
#define NORETURN
#define CONSTRUCTOR
#define DESTRUCTOR
#define ALIGN(to)
#define WUNUSED

#define MAX_8 0xFF
#define MAX_16 0xFFFF
#define MAX_32 0xFFFFFFFF
#define MAX_64 0xFFFFFFFFFFFFFFFF

#define LOOP while (1)

#ifdef _x86_64_

#undef PACKED
#define PACKED __attribute__((packed))

#undef CONST
#define CONST __attribute__((const))

#undef NORETURN
#define NORETURN __attribute__((noreturn))

#undef CONSTRUCTOR
#define CONSTRUCTOR __attribute__((constructor))

#undef DESTRUCTOR
#define DESTRUCTOR __attribute__((destructor))

#undef ALIGN
#define ALIGN(to) __attribute__((aligned(to)))

#undef WUNUSED
#define WUNUSED __attribute__((warn_unused_result))

#endif

#endif
