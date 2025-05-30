#ifndef DECLS_H
#define DECLS_H

#define PACKED
#define CONST
#define NORETURN

#ifdef _x86_64_

#undef PACKED
#define PACKED __attribute__((packed))

#undef CONST
#define CONST __attribute__((const))

#undef NORETURN
#define NORETURN __attribute__((noreturn))

#endif

#endif
