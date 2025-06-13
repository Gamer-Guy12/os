#ifndef DECLS_H
#define DECLS_H

#define PACKED
#define CONST
#define NORETURN
#define CONSTRUCTOR
#define DESTRUCTOR

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

#endif

#endif
