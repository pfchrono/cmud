/*
 * This file handles command interpreting
 */
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* unlink() */

/* include main header file */
#include "mud.h"

void handle_cmd_input(D_SOCKET *dsock, char *argument)
{
  D_MOBILE *dMob;
  char command[MAX_BUFFER];
  bool found_cmd = FALSE;
  int i;

  if ((dMob = dsock->player) == NULL)
    return;
    /*
     * Strip leading spaces.
     */
    while (isspace(*argument))
        argument++;
    if (argument[0] == '\0')
        return;
    if (!isalpha(argument[0]) && !isdigit(argument[0])) {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while (isspace(*argument))
            argument++;
    } else {

  argument = one_argument(argument, command);
    }

  for (i = 0; tabCmd[i].cmd_name[0] != '\0' && !found_cmd; i++)
  {
    if (tabCmd[i].level > dMob->level) continue;

    if (is_prefix(command, tabCmd[i].cmd_name))
    {
      found_cmd = TRUE;
      (*tabCmd[i].cmd_funct)(dMob, argument);
    }
  }

  if( !found_cmd
#ifdef IMC
        &&   !imc_command_hook( dMob, command, argument )
#endif
     )
    text_to_mobile(dMob, "No such command.\n\r");
}

/*
 * The command table, very simple, but easy to extend.
 */
const struct typCmd tabCmd [] =
{

 /* command          function        Req. Level   */
 /* --------------------------------------------- */

 /* command          function        Req. Level   */
 /* --------------------------------------------- */
  { "north",         cmd_north,      LEVEL_GUEST  },
  { "south",         cmd_south,      LEVEL_GUEST  },
  { "east",          cmd_east,       LEVEL_GUEST  },
  { "west",          cmd_west,       LEVEL_GUEST  },
  { "up",            cmd_up,         LEVEL_GUEST  },
  { "down",          cmd_down,       LEVEL_GUEST  },
  { "look",          cmd_look,       LEVEL_GUEST  },
  { "'",             cmd_say,        LEVEL_GUEST  },
  { ".",             cmd_chat,	     LEVEL_GUEST  },
  { "chat",          cmd_chat,	     LEVEL_GUEST  },
  { "goto",          cmd_goto,	     LEVEL_GOD    },
  { "commands",      cmd_commands,   LEVEL_GUEST  },
  { "compress",      cmd_compress,   LEVEL_GUEST  },
  { "copyover",      cmd_copyover,   LEVEL_GOD    },
  { "help",          cmd_help,       LEVEL_GUEST  },
  { "linkdead",      cmd_linkdead,   LEVEL_ADMIN  },
  { "debug",	     cmd_debug,	     LEVEL_ADMIN  },
  { "quit",          cmd_quit,       LEVEL_GUEST  },
  { "relevel",       cmd_relevel,    LEVEL_GUEST  },
  { "save",          cmd_save,       LEVEL_GUEST  },
  { "shutdown",      cmd_shutdown,   LEVEL_GOD    },
  { "version",       do_version,     LEVEL_GUEST  },
  { "who",           cmd_who,        LEVEL_GUEST  },
  { "title",         cmd_title,      LEVEL_GUEST  },

  /* end of table */
  { "", 0 }
};
