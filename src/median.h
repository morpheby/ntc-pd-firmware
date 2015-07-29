/* 
 * File:   median.h
 * Author: morpheby
 *
 * Created on серада, 29, ліпеня 2015, 16.05
 */

#ifndef MEDIAN_H
#define	MEDIAN_H

#ifndef MEDIAN_C
typedef void Mediator;
#else
typedef struct Mediator_t Mediator;
#endif

typedef unsigned int MediatorItem;

//creates new Mediator: to calculate `nItems` running median. 
//mallocs single block of memory, caller must free.
Mediator* MediatorNew(int nItems);

//Inserts item, maintains median in O(lg nItems)
void MediatorInsert(Mediator* m, MediatorItem v);
 
//returns median item (or average of 2 when item count is even)
MediatorItem MediatorMedian(Mediator* m);

#endif	/* MEDIAN_H */

