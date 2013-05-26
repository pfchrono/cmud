/*
 * This file handles non-fighting player actions.
 */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* include main header file */
#include "mud.h"

/* eventually needs loaded via file */
#define HI_XLIMIT 20
#define HI_YLIMIT 20
#define HI_ZLIMIT 3
#define LO_XLIMIT -20
#define LO_YLIMIT -20
#define LO_ZLIMIT -3

void do_movement(D_MOBILE *dMob, int dir)
{
  D_MOBILE *xMob;
  ITERATOR *pIter;
  bool leftr = FALSE;

  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
     if ((dMob->coordx != xMob->coordx) && (dMob->coordz != xMob->coordz) && (dMob->coordy != xMob->coordy))
	leftr = TRUE;
   }
  switch (dir)
  {
    case NORTH:
      /* North */
      stcf(dMob, "You leave for the north.\n\r");
      dMob->coordy++; 
      break; 
    case SOUTH: 
      /* South */
      stcf(dMob, "You leave for the south.\n\r");
      dMob->coordy--;
      break; 
    case EAST: 
      /* East */
      stcf(dMob, "You head eastward.\n\r");
      dMob->coordx++;
      break; 
    case WEST:
      /* West */
      stcf(dMob, "You head westward.\n\r");
      dMob->coordx--;
      break;
    case UP:
      /* Up */
      stcf(dMob, "You fly upwards.\n\r");
      dMob->coordz++;
      break;
    case DOWN:
      /* Down */
      stcf(dMob, "You you head downwards.\n\r");
      dMob->coordz--;
      break;
    case NORTHEAST:
      /* NorthEast */
      dMob->coordy++; 
      dMob->coordx++;
      break;
    case SOUTHEAST:
      /* SouthEast */
      dMob->coordy--; 
      dMob->coordx++;
      break;
    case SOUTHWEST:
      /* SouthWest */
      dMob->coordy--; 
      dMob->coordx--;
      break;
    case NORTHWEST:
      /* NorthWest */
      dMob->coordy++; 
      dMob->coordx--;
      break;
  } 

  if (dMob->coordx > HI_XLIMIT) 
  {
  dMob->coordx--; 
  text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (dMob->coordy > HI_YLIMIT) 
  {
  dMob->coordy--;
  text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (dMob->coordz > HI_ZLIMIT) 
  {
    dMob->coordz--;
    text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (dMob->coordx < LO_XLIMIT) 
  {
    dMob->coordx++;
    text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (dMob->coordy < LO_YLIMIT) 
  {
    dMob->coordy++;
    text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (dMob->coordz < LO_ZLIMIT) 
  {
    dMob->coordz++;
    text_to_mobile(dMob, "You can't move there!\n\r");
  return;
  }
  if (leftr)
     stcf(xMob, "%s has left the room.\n\r",dMob->name);

  cmd_look(dMob, NULL);
  return;
}

