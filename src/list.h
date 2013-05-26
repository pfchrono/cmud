/* file: list.h
 *
 * Headerfile for a basic double-linked list
 */

#ifndef _LIST_HEADER
#define _LIST_HEADER

typedef struct List      LIST;
typedef struct Iterator  ITERATOR;

LIST      *AllocList          ( void );
ITERATOR  *AllocIterator      ( LIST *pList);
void      *NextInList         ( ITERATOR *pIter );
void       AttachToList       ( void *pContent, LIST *pList );
void       DetachFromList     ( void *pContent, LIST *pList );
void       FreeIterator       ( ITERATOR *pIter );
void       FreeList           ( LIST *pList );
int        SizeOfList         ( LIST *pList );

#endif
