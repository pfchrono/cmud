/*
 * This file handles non-fighting player actions.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>

/* include main header file */
#include "mud.h"

size_t mudstrlcpy( char *dst, const char *src, size_t siz )
{
   register char *d = dst;
   register const char *s = src;
   register size_t n = siz;

   /*
    * Copy as many bytes as will fit
    */
   if( n != 0 && --n != 0 )
   {
      do
      {
         if( ( *d++ = *s++ ) == 0 )
            break;
      }
      while( --n != 0 );
   }

   /*
    * Not enough room in dst, add NUL and traverse rest of src
    */
   if( n == 0 )
   {
      if( siz != 0 )
         *d = '\0';  /* NUL-terminate dst */
      while( *s++ )
         ;
   }
   return ( s - src - 1 ); /* count does not include NUL */
}

char *str_dup( char const *str )
{
    static char *ret;
    int len;

    if ( !str )
        return NULL;

    len = strlen(str)+1;

    CREATE( ret, char, len );
    strcpy( ret, str );
    return ret;
}

bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
        bug( "Str_cmp: null astr.", 0 );
        return TRUE;
    }

    if ( bstr == NULL )
    {
        bug( "Str_cmp: null bstr.", 0 );
        return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
        if ( LOWER(*astr) != LOWER(*bstr) )
            return TRUE;
    }

    return FALSE;
}

void cmd_say(D_MOBILE *dMob, char *arg)
{
  if (arg[0] == '\0')
  {
    text_to_mobile(dMob, "Say what?\n\r");
    return;
  }
  communicate(dMob, arg, COMM_LOCAL);
}

void cmd_quit(D_MOBILE *dMob, char *arg)
{
  char buf[MAX_BUFFER];

  /* log the attempt */
  sprintf(buf, "%s has left the game.", dMob->name);
  log_string(buf);

  save_player(dMob);

  dMob->socket->player = NULL;
  free_mobile(dMob);
  close_socket(dMob->socket, FALSE);
}
void cmd_relevel(D_MOBILE *dMob, char *arg)
{

  if(!str_cmp(dMob->name, "Chrono"))
  dMob->level = LEVEL_GOD;

  return;
}

void cmd_shutdown(D_MOBILE *dMob, char *arg)
{
  shut_down = TRUE;
}

void cmd_commands(D_MOBILE *dMob, char *arg)
{
  BUFFER *buf = buffer_new(MAX_BUFFER);
  int i, col = 0;

  bprintf(buf, "    - - - - ----==== The full command list ====---- - - - -\n\n\r");
  for (i = 0; tabCmd[i].cmd_name[0] != '\0'; i++)
  {
    if (dMob->level < tabCmd[i].level) continue;

    bprintf(buf, " %-16.16s", tabCmd[i].cmd_name);
    if (!(++col % 4)) bprintf(buf, "\n\r");
  }
  if (col % 4) bprintf(buf, "\n\r");
  text_to_mobile(dMob, buf->data);
  buffer_free(buf);
}

void cmd_who(D_MOBILE *dMob, char *arg)
{
  D_MOBILE *xMob;
  D_SOCKET *dsock;
  ITERATOR *pIter;
  BUFFER *buf = buffer_new(MAX_BUFFER);

  bprintf(buf, " #D- - - - #n----#R==== #CWho's Online #R====#n----#D - - - -#n\n\r");

  bprintf(buf, " #D- - - - #n----#R======================#n---- #D- - - -#n\n\r");
  bprintf(buf, " Name       Title     Level\n\r");
  bprintf(buf, " #D- - - - #n----#R======================#n---- #D- - - -#n\n\r");
  pIter = AllocIterator(dsock_list);
  while ((dsock = (D_SOCKET *) NextInList(pIter)) != NULL)
  {
    if (dsock->state != STATE_PLAYING) continue;
    if ((xMob = dsock->player) == NULL) continue;

    bprintf(buf, " %-12s   %s         #D(#W%d#D)#n\n\r", xMob->name, xMob->title, xMob->level);
  }
  FreeIterator(pIter);

  bprintf(buf, " #D- - - - #n----#R======================#n---- #D- - - -#n\n\r");
  text_to_mobile(dMob, buf->data);

  buffer_free(buf);
}

void cmd_help(D_MOBILE *dMob, char *arg)
{
  if (arg[0] == '\0')
  {
    HELP_DATA *pHelp;
    ITERATOR *pIter;
    BUFFER *buf = buffer_new(MAX_BUFFER);
    int col = 0;

    bprintf(buf, "      - - - - - ----====//// HELP FILES  \\\\\\\\====---- - - - - -\n\n\r");

    pIter = AllocIterator(help_list);
    while ((pHelp = (HELP_DATA *) NextInList(pIter)) != NULL)
    {
      bprintf(buf, " %-19.18s", pHelp->keyword);
      if (!(++col % 4)) bprintf(buf, "\n\r");
    }
    FreeIterator(pIter);

    if (col % 4) bprintf(buf, "\n\r");
    bprintf(buf, "\n\r Syntax: help <topic>\n\r");
    text_to_mobile(dMob, buf->data);
    buffer_free(buf);

    return;
  }

  if (!check_help(dMob, arg))
    text_to_mobile(dMob, "Sorry, no such helpfile.\n\r");
}

void cmd_compress(D_MOBILE *dMob, char *arg)
{
  /* no socket, no compression */
  if (!dMob->socket)
    return;

  /* enable compression */
  if (!dMob->socket->out_compress)
  {
    text_to_mobile(dMob, "Trying compression.\n\r");
    text_to_buffer(dMob->socket, (char *) compress_will2);
    text_to_buffer(dMob->socket, (char *) compress_will);
  }
  else /* disable compression */
  {
    if (!compressEnd(dMob->socket, dMob->socket->compressing, FALSE))
    {
      text_to_mobile(dMob, "Failed.\n\r");
      return;
    }
    text_to_mobile(dMob, "Compression disabled.\n\r");
  }
}

void cmd_save(D_MOBILE *dMob, char *arg)
{
  save_player(dMob);
  text_to_mobile(dMob, "Saved.\n\r");
}

void cmd_copyover(D_MOBILE *dMob, char *arg)
{ 
  FILE *fp;
  ITERATOR *pIter;
  D_SOCKET *dsock;
  char buf[100], buf2[100];
  
  if ((fp = fopen(COPYOVER_FILE, "w")) == NULL)
  {
    text_to_mobile(dMob, "Copyover file not writeable, aborted.\n\r");
    return;
  }

  sprintf(buf, "\n\r <*>            The world starts spinning             <*>\n\r");

  /* For each playing descriptor, save its state */
  pIter = AllocIterator(dsock_list);
  while ((dsock = (D_SOCKET *) NextInList(pIter)) != NULL)
  {
    compressEnd(dsock, dsock->compressing, FALSE);

    if (dsock->state != STATE_PLAYING)
    {
      text_to_socket(dsock, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r");
      close_socket(dsock, FALSE);
    }
    else
    {
      fprintf(fp, "%d %s %s\n",
        dsock->control, dsock->player->name, dsock->hostname);

      /* save the player */
      save_player(dsock->player);

      text_to_socket(dsock, buf);
    }
  }
  FreeIterator(pIter);

  fprintf (fp, "-1\n");
  fclose (fp);

  /* close any pending sockets */
  recycle_sockets();

#ifdef IMC
   imc_hotboot();
#endif
  
  /* exec - descriptors are inherited */
  sprintf(buf, "%d", control);
#ifdef IMC
   if( this_imcmud )
      snprintf( buf2, 100, "%d", this_imcmud->desc );
   else
      strncpy( buf2, "-1", 100 );
#else
   strncpy( buf2, "-1", 100 );
#endif
  execl(EXE_FILE, "SocketMud", "copyover", buf, buf2, (char *) NULL, (char *) NULL);

  /* Failed - sucessful exec will not return */
  text_to_mobile(dMob, "Copyover FAILED!\n\r");
}

void cmd_linkdead(D_MOBILE *dMob, char *arg)
{
  D_MOBILE *xMob;
  ITERATOR *pIter;
  char buf[MAX_BUFFER];
  bool found = FALSE;

  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
  {
    if (!xMob->socket)
    {
      sprintf(buf, "%s is linkdead.\n\r", xMob->name);
      text_to_mobile(dMob, buf);
      found = TRUE;
    }
  }
  FreeIterator(pIter);

  if (!found)
    text_to_mobile(dMob, "Noone is currently linkdead.\n\r");
}

void safe_printf( const char *file, const char *function, int line, int size, char *str, char *fmt, ... )
{
   char buf[MAS];
   va_list args;
   va_start( args, fmt );
   vsprintf( buf, fmt, args );
   va_end( args );

   /*
    * Max Alloc Size is allot!
    */
   if( size > MAS )
   {
      return;
   }

   if( ( unsigned )size < strlen( buf ) + 1 )
   {
   }
   else
   {
      mudstrlcpy( str, buf, size );

      /*
       * Just double checking.
       */
      if( strlen( str ) > ( unsigned )size - 1 )
      {
         char egbug[MSL];
         /*
          * Yes, this is a potential loop bug if infact the xprintf does collapse in on itself..
          */
         xprintf( egbug, "Memcheck: System memory corrupted by overflow through xprintf: File: %s Function: %s Line: %d",
                  file, function, line );
      }
   }
}

void cmd_north(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, NORTH);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s arrives from the south\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_south(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, SOUTH);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s arrives from the north.\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_east(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, EAST);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s arrives from the west.\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_west(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, WEST);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s arrives from the east.\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_up(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, UP);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s comes from below.\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_down(D_MOBILE *dMob, char *arg)
{
  do_movement(dMob, DOWN);
  D_MOBILE *xMob;
  ITERATOR *pIter;
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
     if (xMob == dMob) continue;
//     if (xMob == NULL) continue;
     if ((dMob->coordx == xMob->coordx) && (dMob->coordz == xMob->coordz) && (dMob->coordy == xMob->coordy))
     stcf(xMob, "%s comes fom above.\n\r",dMob->name);
   }
      FreeIterator(pIter);
  return;
}

void cmd_look(D_MOBILE *dMob, char *arg)
{
  char buf[MAX_BUFFER];
  ITERATOR *pIter;
  D_MOBILE *xMob;
  bool ytrue;
  bool xtrue;
  bool ztrue;
  bool ytrue2;
  bool xtrue2;
  bool ztrue2;

  if (dMob->coordx >= 20)
    xtrue = FALSE;
  else
    xtrue = TRUE;

  if (dMob->coordy >= 20)
    ytrue = FALSE;
  else
    ytrue = TRUE;

  if (dMob->coordx <= -20)
    xtrue2 = FALSE;
  else
    xtrue2 = TRUE;

  if (dMob->coordy <= -20)
    ytrue2 = FALSE;
  else
    ytrue2 = TRUE;

  if (dMob->coordz >= 3)
    ztrue = FALSE;
  else
    ztrue = TRUE;

  if (dMob->coordz <= -3)
    ztrue2 = FALSE;
  else
    ztrue2 = TRUE;

  /* send x/y/z coords */
  stcf(dMob, "Current Location: #Y[#R%dX#Y] [#R%dY#Y] [#R%dZ#Y]#n\n\r\n\r", dMob->coordx, dMob->coordy, dMob->coordz);
  stcf(dMob,"Exits: [ ");
  if (ztrue && ztrue2)
  stcf(dMob,"D U ");
  if (!ztrue && ztrue2)
  stcf(dMob,"D ");
  if (ztrue && !ztrue2)
  stcf(dMob,"U ");
  if (!ztrue && !ztrue2)
  stcf(dMob,"");
  if (ytrue && ytrue2)
  stcf(dMob,"N S ");
  if (!ytrue && ytrue2)
  stcf(dMob,"S ");
  if (ytrue && !ytrue2)
  stcf(dMob,"N ");
  if (!ytrue && !ytrue2)
  stcf(dMob,"");
  if (xtrue && xtrue2)
  stcf(dMob,"W E ");
  if (!xtrue && xtrue2)
  stcf(dMob,"W ");
  if (xtrue && !xtrue2)
  stcf(dMob,"E ");
  if (!xtrue && !xtrue2)
  stcf(dMob,"");
//  if (!ztrue && !ztrue2 && !xtrue && !ytrue && !ytrue2 && !xtrue2)
  stcf(dMob,"]\n\r");

  /* display others in room */
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
//  for (dsock = dsock_list; dsock; dsock = dsock->next)
//  {
//    if (dsock->state != STATE_PLAYING) continue;
//    if ((xMob = dMob) == NULL) continue;
    if ((xMob->name != dMob->name) && (xMob->coordx == dMob->coordx) && (xMob->coordy == dMob->coordy) && (xMob->coordz == dMob->coordz))
    {
      sprintf(buf, "%s #D(#W%s#D)#n is here.\n\r", xMob->name, xMob->title);
      text_to_mobile(dMob, buf);
    }
    else continue;
  }
      FreeIterator(pIter);
  return;
}

void cmd_title(D_MOBILE *dMob, char *arg)
{
  char buf[MAX_BUFFER];
  if (arg[0] == '\0')
  {
    sprintf(buf, "Title reset to default value of %s.\n\r", DEFAULT_TITLE);
    dMob->title = str_dup(DEFAULT_TITLE);
    text_to_mobile(dMob, buf);
    return;
  }
  else
  {
    dMob->title = str_dup(arg);
    sprintf(buf, "Title set to %s.\n\r", arg);
    text_to_mobile(dMob, buf);
  }
}

void append_dfile( D_MOBILE *ch, char *file, char *str )
{
    FILE *fp;
    static char buf [ 128 ];
    static char buf2 [ 128 ];
    static char buf3 [ 128 ];
    struct tm * datetime;

    datetime = localtime( &current_time );
    strftime( buf, sizeof( buf ), "%m/%d/%Y", datetime );
    strftime( buf2, sizeof( buf2 ), "%r", datetime );

    if ( str[0] == '\0' )
        return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
        xprintf(buf3,"Could not write to %s in append_dfile", file);
        bug(buf3, 0 );
    //    stcf(ch "Could not open the file!\n\r");
    }
    else
    {
    if (ch == NULL)
    fprintf(fp, "[%5s][%5s] (Someone): %s\n",buf, buf2, str);
    else
    fprintf(fp, "[%5s][%5s] (%s): %s\n",buf, buf2, ch->name, str);
        fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

