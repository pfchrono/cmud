#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "mud.h"
#include "version.h"
#include "build.h"

const char *versionToString( int version )
{
   static char buf[64];

   xprintf( buf, "%d.%02d.%02d", version / 10000, ( version - ( ( 10000 * ( version / 10000 ) ) ) ) / 100, version % 100 );

   return buf;
}

void do_version( D_MOBILE * dMob, char *argument )
{
   char buf[MSL];

   text_to_mobile(dMob, "--------------------------------------------------------------------------------\n\r");
   text_to_mobile(dMob, "CMud\n\r");
   xprintf( buf, "This Version %s. Build: %d Compiled: %s.\n\r", versionToString( VERSION ), LINKCOUNT, COMPILE_TIME );
   text_to_mobile(dMob, buf);
   text_to_mobile(dMob, "by Chrono (chrono@grevin.sytes.net)\n\r");
   text_to_mobile(dMob, "--------------------------------------------------------------------------------\n\r");
   return;
}

