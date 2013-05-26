/*
 * Immortal only actions and commands..
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> /* unlink() */

/* include main header file */
#include "mud.h"

D_MOBILE *get_char(D_MOBILE * ch, char *argument)
{
  D_MOBILE *wch;
  ITERATOR *pIter;
  char arg[MIL];
//  char buf[MSL];
  pIter = AllocIterator(dmobile_list);
  while ((wch = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (wch == ch) continue;
     else if (str_cmp(arg,wch->name))
	return wch;
     else
	return NULL;
   }
      FreeIterator(pIter);
  return NULL;  
}

void cmd_debug(D_MOBILE * ch, char * argument)
{
 char buf[MSL];
 char buf2[MSL];
 struct tm * datetime;

 datetime = localtime( &current_time );
 strftime( buf, sizeof( buf ), "%m/%d/%Y", datetime );
 strftime( buf2, sizeof( buf2 ), "%r", datetime );

 stcf(ch,"Name     : %s\n\r",ch->name);
 stcf(ch,"Hostname : %s\n\r",ch->socket->hostname);
 stcf(ch,"Level    : %d\n\r",ch->level);
 stcf(ch,"Date     : %s\n\r",buf);
 stcf(ch,"Time     : %s\n\r",buf2);
 return;
}
void cmd_goto(D_MOBILE * ch, char *argument)
{
    D_MOBILE *victim;
    char arg[MSL];
    char *string;

    string = one_argument(argument, arg);

    if (argument == NULL)
   {
        stcf(ch,"Goto where?\n\r");
        return;
    }
    victim = get_char(ch,string);
    if(victim == NULL)
    {
	stcf(ch,"Victim not found!\n\r");
	return;
    }
    ch->coordx = victim->coordx;
    ch->coordy = victim->coordy;
    ch->coordz = victim->coordz;
    stcf(ch,"You leave in a swirling mist.\n\r");
    stcf(victim,"%s appears in a swirling mist.\n\r",ch->name);
    cmd_look(ch,"");
    return;
}

