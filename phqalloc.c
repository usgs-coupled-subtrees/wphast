#include <stdlib.h>
#include <string.h>
#include <assert.h>
static char const svnid[] = "$Id: phqalloc.c,v 1.1 2005/04/01 02:27:53 charlton Exp $";

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

typedef struct PHRQMemHeader {
	struct PHRQMemHeader *pNext;	/* memory allocated just after this one */
	struct PHRQMemHeader *pPrev;	/* memory allocated just prior to this one */
} PHRQMemHeader;

/*
 * This pointer points to the last allocated block
 * Note: s_pTail->pNext should always be NULL
 */
static PHRQMemHeader *s_pTail = NULL;


/* ---------------------------------------------------------------------- */
void *PHRQ_malloc( size_t size )
/* ---------------------------------------------------------------------- */
{
	PHRQMemHeader *p;

	assert((s_pTail == NULL) || (s_pTail->pNext == NULL));

	p = (PHRQMemHeader*)malloc(sizeof(PHRQMemHeader) + size);

	if (p == NULL) return NULL;

	p->pNext = NULL;
	
	if ( (p->pPrev = s_pTail) != NULL ) {
		s_pTail->pNext = p;
	}

	s_pTail = p;
	p++;
	return ((void *)(p));
}

/* ---------------------------------------------------------------------- */
void PHRQ_free( void *ptr )
/* ---------------------------------------------------------------------- */
{
	PHRQMemHeader *p;

	assert((s_pTail == NULL) || (s_pTail->pNext == NULL));

	if (ptr == NULL) return;

	p = (PHRQMemHeader *)ptr - 1;

	if (p->pNext != NULL) {
		p->pNext->pPrev = p->pPrev;
	} else {
		/* Handle special case when (p == s_pTail) */
		assert(s_pTail != NULL);
		assert(p == s_pTail);
		s_pTail = p->pPrev;
	}	
	
	if (p->pPrev) {
		p->pPrev->pNext = p->pNext;
	}
	
	free(p);
}

/* ---------------------------------------------------------------------- */
void PHRQ_free_all( void )
/* ---------------------------------------------------------------------- */
{
	assert((s_pTail == NULL) || (s_pTail->pNext == NULL));

	if (s_pTail == NULL) {
		return;
	}
	while (s_pTail->pPrev != NULL)
	{
		s_pTail = s_pTail->pPrev;
		free(s_pTail->pNext);
	}
	free(s_pTail);
	s_pTail = NULL;
}

/* ---------------------------------------------------------------------- */
void *PHRQ_calloc( size_t num, size_t size )
/* ---------------------------------------------------------------------- */
{
	PHRQMemHeader *p;

	assert((s_pTail == NULL) || (s_pTail->pNext == NULL));

	p = (PHRQMemHeader*)malloc(sizeof(PHRQMemHeader) + size * num);

	if (p == NULL) return NULL;

	p->pNext = NULL;
	
	if ( (p->pPrev = s_pTail) != NULL ) {
		s_pTail->pNext = p;
	}

	s_pTail = p;
	p++;
	return memset(p, 0, size * num);
}

/* ---------------------------------------------------------------------- */
void *PHRQ_realloc( void *ptr, size_t size )
/* ---------------------------------------------------------------------- */
{
	PHRQMemHeader *p;
	size_t new_size;

	if (ptr == NULL)
	{
		return PHRQ_malloc(size);
	}

	assert((s_pTail == NULL) || (s_pTail->pNext == NULL));

	p = (PHRQMemHeader*)ptr - 1;

	new_size = sizeof(PHRQMemHeader) + size;

	p = (PHRQMemHeader*)realloc(p, new_size);

	if (p == NULL) return NULL;

	if (p->pPrev != NULL) {
		p->pPrev->pNext = p;
	}

	if (p->pNext != NULL) {
		p->pNext->pPrev = p;
	} else {
		s_pTail = p;
	}

	p++;
	return ((void *)(p));
}

