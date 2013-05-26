/*
Basic Communication and other commands
*/
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "mud.h"

char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
        argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *arg_first = LOWER(*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
        argument++;

    return argument;
}

void stcf(D_MOBILE *dMob,const char *fmt,...)
{
    char buf[MSL];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    text_to_mobile(dMob,buf);
}

void cmd_chat(D_MOBILE * dMob, char *argument)
{
  D_MOBILE *xMob;
  ITERATOR *pIter;
  char arg[MIL];
  char buf[MSL];

  one_argument(argument, arg);

  if (argument[0] == '\0') {
  stcf(dMob,"How about saying something, %s?\n\r",dMob->name);
  return;
  }

  stcf(dMob,"You chat '#C%s#n'\n\r",arg);
  xprintf(buf,"#W%s#n chats '#C%s#n'\n\r",dMob->name,arg);

  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
     text_to_mobile(xMob, buf);
   }
      FreeIterator(pIter);
  append_dfile(dMob, LOG_DIR CHAT_LOG, argument);
  return;
}
