/* file      : signal.c
 * date      : May 9th, 2003
 * version   : 0.2
 *
 * This file contains the signal handling code for
 * SocketMud(tm). It requires the event scheduler patch,
 * but besides that, it should be possible to install
 * this snippet into any SocketMud(tm) based product.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "mud.h"

bool sigGuard = FALSE;

void crashguard(int i)
{
  FILE *fp;
  ITERATOR *pIter;
  D_MOBILE *xMob;
  char buf[100];

  /* if signal handling is not yet enabled, we will
   * set the signal handling for SIGSEGV to the default
   * value, and then send a SIGSEGV to the MUD (again)
   */
  if (!sigGuard)
  {
    signal(SIGSEGV, SIG_DFL);
    raise(SIGSEGV);
    return;
  }

  /* fork the MUD, allowing one process to continue and
   * the other to die, creating a coredump for debugging.
   */
  if (fork() <= 0)
  {
    signal(SIGSEGV, SIG_DFL);
    raise(SIGSEGV);
    return;
  }
  wait(NULL);

  /* log the attempt to do a copyover */
  bug("SocketMud crashed, attempting to do a copyover.");

  /* attempt to open the copyover file to store sockets */
  if ((fp = fopen(COPYOVER_FILE, "w")) == NULL)
  {
    bug("Failed to open %s, aborting.", COPYOVER_FILE);
    exit(1);
  }

  /* write error message */
  sprintf(buf, "\n\r <*>            SocketMud Has Crashed....             <*>\n\r");

  /* inform all players of the crash, and turn off compression */
  pIter = AllocIterator(dmobile_list);
  while ((xMob = (D_MOBILE *) NextInList(pIter)) != NULL)
   {
//  for (dsock = dsock_list; dsock ; dsock = dsock_next)
//  {
//    dsock_next = dsock->next;

    compressEnd(xMob->socket, xMob->socket->compressing, FALSE);

/*    if (xMob->socket->state != STATE_PLAYING)
    {
      text_to_socket(dMob->socket, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r");
      close_socket(dsock, FALSE);
    }*/
      fprintf(fp, "%d %s %s\n",
        xMob->socket->control, xMob->name, xMob->socket->hostname);

      save_player(xMob->socket->player);

      text_to_socket(xMob->socket, buf);
  }

  /* close copyover file */
  fprintf (fp, "-1\n");
  fclose (fp);

  /* recycle sockets */
  recycle_sockets();

  /* attempt to execute SocketMud again */
  sprintf(buf, "%d", control);
  execl(EXE_FILE, "SocketMud", "crashguard", buf, (char *) NULL, (char *) NULL);

  /* log failed attempt */
  bug("Attempt to restore MUD failed.");
  exit(1);
}

void termguard(int i)
{
  ITERATOR *pIter;
//  D_SOCKET *dsock;
  char buf[200];

  bug("The server is shutting down, attempting to close MUD.");

  sprintf(buf, "\n\rServer Reboot - Stand By!!\n\r");

  pIter = AllocIterator(dmobile_list);
  /* inform all players and save them */
/*  for (dsock = dmobile_list; dsock; dsock = dsock->next)
  {
    text_to_socket(dsock, buf);

    if (dsock->state == STATE_PLAYING && dsock->player)
      save_player(dsock->player);
  }*/

  /* close MUD */
  exit(1);
}

bool event_game_sigguard(EVENT_DATA *event)
{
  sigGuard = TRUE;

  return FALSE;
}

void init_sigguard()
{
  EVENT_DATA *event;

  /* set the handlers for SIGSEGV (crash)
   * and SIGTERM (unexpected shutdown)
   */
  signal(SIGSEGV, crashguard);
  signal(SIGTERM, termguard);

  /* enqueue the event that will enable
   * the signal guard. We delay this by
   * 10 seconds to prevent spam-copyover.
   */
  event = alloc_event();
  event->fun = &event_game_sigguard;
  event->type = EVENT_GAME_SIGGUARD;
  add_event_game(event, 10 * PULSES_PER_SECOND);
}

