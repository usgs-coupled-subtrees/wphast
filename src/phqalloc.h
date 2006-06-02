#if !defined (INCLUDE_PHRQALLOC_H)
#define INCLUDE_PHRQALLOC_H

extern void *PHRQ_malloc(size_t);
extern void *PHRQ_calloc(size_t, size_t);
extern void *PHRQ_realloc(void *, size_t);

extern void PHRQ_free(void *);
extern void PHRQ_free_all(void);

#define malloc(p)     PHRQ_malloc(p)
#define calloc(c, s)  PHRQ_calloc(c, s)
#define realloc(p, s) PHRQ_realloc(p, s)
#define free(p)       PHRQ_free(p)

#endif /* !defined (INCLUDE_PHRQALLOC_H) */
