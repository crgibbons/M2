#ifndef NEWDELETE_H
#define NEWDELETE_H 1

#define M2_ASSERT assert

#define GC_REDIRECT_TO_LOCAL
// get declarations of outofmem and getmem
#include "../d/M2mem.h"
#include "../d/debug.h"

#include <gc/gc.h>

// these replace all uses of the construction "new T[n]" (unless constructors have to be run!):
#define newarray(T,len) reinterpret_cast<T*>(getmem((len) * sizeof(T)))
#define newarray_clear(T,len) reinterpret_cast<T*>(getmem_clear((len) * sizeof(T)))
// this replaces all uses of the construction "new T":
#define newitem(T) reinterpret_cast<T*>(getmem(sizeof(T)))
#define newitem_clear(T) reinterpret_cast<T*>(getmem_clear(sizeof(T)))
// this replaces all uses of the construction "delete [] x",
// except it doesn't delete the individual elements for you, if they happen to be pointers
#ifdef DEBUG
#define deletearray(x) (trapchk(x), GC_FREE(x))
#else
#define deletearray(x) GC_FREE(x)
#endif
// this replaces all uses of the construction "delete x" (unless a destructor has to be run!):
#ifdef DEBUG
#define deleteitem(x) (TRAPCHK(x), GC_FREE(x))
#else
#define deleteitem(x) GC_FREE(x)
#endif

// this replaces all uses of the construction "new T[n]", with T containing NO pointers
#define newarray_atomic(T,len) reinterpret_cast<T*>(getmem_atomic((len) * sizeof(T)))
#define newarray_atomic_clear(T,len) reinterpret_cast<T*>(getmem_atomic_clear((len) * sizeof(T)))
// this replaces all uses of the construction "new T":
#define newitem_atomic(T) reinterpret_cast<T*>(getmem_atomic(sizeof(T)))

// This is used instead of newitem(T) when the size is not known to the c++ compiler
// Caution: this uses the pointer type, not the struct type.
#define GETMEM(T,size) reinterpret_cast<T>(getmem(size))
#define GETMEM_ATOMIC(T,size) reinterpret_cast<T>(getmem_atomic(size))

struct our_new_delete {
  static inline void* operator new    ( size_t size ) { void *p = GC_MALLOC( size ); if (p == NULL) outofmem2(size); TRAPCHK(p); return p; }
  static inline void* operator new [] ( size_t size ) { void *p = GC_MALLOC( size ); if (p == NULL) outofmem2(size); TRAPCHK(p); return p; }

  static inline void operator delete    ( void* obj ) { TRAPCHK(obj); if (obj != NULL) GC_FREE( obj ); }
  static inline void operator delete [] ( void* obj ) { TRAPCHK(obj); if (obj != NULL) GC_FREE( obj ); }

  static inline void* operator new    ( size_t size, void *existing_memory ) { return existing_memory; }
  static inline void* operator new [] ( size_t size, void *existing_memory ) { return existing_memory; }

  static inline void operator delete    ( void *obj, void *existing_memory ) { TRAPCHK(obj); }
  static inline void operator delete [] ( void *obj, void *existing_memory ) { TRAPCHK(obj); }

#if !defined(__GNUC__) || defined(__INTEL_COMPILER)
  // see Scott Meyers, Effective C++, item 14!  This avoids something really bad in the c++ standard.
  // ... but it slows down destuctors in every class inheriting from this one
  // gnu cc does it right, running all the destructors, so we don't bother with this.
#if 0
      // But this will put a pointer to the table of virtual methods at the start of every instance,
      // and we don't want that, because we want to pass these pointers to C routines.
      // So we can't have any virtual methods here.
      virtual ~our_new_delete() {}
#endif
#endif

};

#include <gc/gc_allocator.h>

// struct gc_malloc_alloc {
//   static void* allocate(size_t n) { void* p = GC_MALLOC(n); if (p == NULL) outofmem2(n); return p; }
//   static void deallocate(void* p, size_t /* n */) { GC_FREE(p); }
//   static void* reallocate(void* p, size_t /* old size */, size_t newsize) { void* r = GC_REALLOC(p, newsize); if (NULL == r) outofmem2(newsize); return r; }
// };

#endif

// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
// indent-tabs-mode: nil
// End:
