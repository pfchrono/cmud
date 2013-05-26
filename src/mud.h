/*
 * This is the main headerfile
 */

#ifndef MUD_H
#define MUD_H

#include <zlib.h>
#include <pthread.h>
#include <arpa/telnet.h>

#include "list.h"
#include "stack.h"

/************************
 * Standard definitions *
 ************************/

/* define TRUE and FALSE */
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#define eTHIN   0
#define eBOLD   1

#define A                       1
#define B                       2
#define C                       4
#define D                       8
#define E                       16
#define F                       32
#define G                       64
#define H                       128
#define I                       256
#define J                       512
#define K                       1024
#define L                       2048
#define M                       4096
#define N                       8192
#define O                       16384
#define P                       32768
#define Q                       65536
#define R                       131072
#define S                       262144
#define T                       524288
#define U                       1048576
#define V                       2097152
#define W                       4194304
#define X                       8388608
#define Y                       16777216
#define Z                       33554432
#define aa                      67108864        /* doubled due to conflicts */
#define bb                      134217728
#define cc                      268435456
#define ddd                     536870912
#define ee                      1073741824

/* A few globals */
#define PULSES_PER_SECOND     4                   /* must divide 1000 : 4, 5 or 8 works */
#define MAX_BUFFER         1024                   /* seems like a decent amount         */
#define MAX_OUTPUT         2048                   /* well shoot me if it isn't enough   */
#define MAX_HELP_ENTRY     4096                   /* roughly 40 lines of blocktext      */
#define MUDPORT            4050                   /* just set whatever port you want    */
#define FILE_TERMINATOR    "EOF"                  /* end of file marker                 */
#define COPYOVER_FILE      "../txt/copyover.dat"  /* tempfile to store copyover data    */
#define EXE_FILE           "../src/CMud"     /* the name of the mud binary         */
#define NULL_FILE       "/dev/null"             /* To reserve one stream */
#define LOG_DIR        "../log/"
#define CHAT_LOG	"chat.log"

/* Connection states */
#define STATE_NEW_NAME         0
#define STATE_NEW_PASSWORD     1
#define STATE_VERIFY_PASSWORD  2
#define STATE_ASK_PASSWORD     3
#define STATE_PLAYING          4
#define STATE_CLOSED           5

/* Thread states - please do not change the order of these states    */
#define TSTATE_LOOKUP          0  /* Socket is in host_lookup        */
#define TSTATE_DONE            1  /* The lookup is done.             */
#define TSTATE_WAIT            2  /* Closed while in thread.         */
#define TSTATE_CLOSED          3  /* Closed, ready to be recycled.   */

/* player levels */
#define LEVEL_GUEST            1  /* Dead players and actual guests  */
#define LEVEL_PLAYER           2  /* Almost everyone is this level   */
#define LEVEL_ADMIN            3  /* Any admin without shell access  */
#define LEVEL_GOD              4  /* Any admin with shell access     */

/* Communication Ranges */
#define COMM_LOCAL             0  /* same room only                  */
#define COMM_LOG              10  /* admins only                     */

/* few basic defines */
#define DEFAULT_TITLE          "The Player"

/* Directions */
#define NORTH                   0
#define SOUTH                   1
#define EAST                    2
#define WEST                    3
#define UP                      4
#define DOWN                    5
#define NORTHEAST               6
#define SOUTHEAST               7
#define SOUTHWEST               8
#define NORTHWEST               9

/* define simple types */
typedef  unsigned char     bool;
typedef  short int         sh_int;


/******************************
 * End of standard definitons *
 ******************************/

/***********************
 * Defintion of Macros *
 ***********************/

#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define IS_ADMIN(dMob)          ((dMob->level) > LEVEL_PLAYER ? TRUE : FALSE)
#define CREATE(result, type, number)                            \
do                                                              \
{                                                               \
   if (!((result) = (type *) calloc ((number), sizeof(type))))  \
        { perror("malloc failure"); abort(); }                  \
} while(0)

#define RECREATE(result,type,number)                            \
do                                                              \
{                                                               \
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
        { perror("realloc failure"); abort(); }                 \
} while(0)
#define DISPOSE(point)                                          \
do                                                              \
{                                                               \
  if (!(point))                                                 \
  {                                                             \
        bug( "Freeing null pointer" ); \
        fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
  }                                                             \
  else free(point);                                             \
  point = NULL;                                                 \
} while(0)

#define IREAD(sKey, sPtr)             \
{                                     \
  if (!strcasecmp(sKey, word))        \
  {                                   \
    int sValue = fread_number(fp);    \
    sPtr = sValue;                    \
    found = TRUE;                     \
    break;                            \
  }                                   \
}
#define SREAD(sKey, sPtr)             \
{                                     \
  if (!strcasecmp(sKey, word))        \
  {                                   \
    sPtr = fread_string(fp);          \
    found = TRUE;                     \
    break;                            \
  }                                   \
}

#define SREAD2(sKey, sPtr)             \
{                                     \
  if (!str_cmp(sKey, word))        \
  {                                   \
    sPtr = fread_string(fp);          \
    found = TRUE;                     \
    break;                            \
  }                                   \
}

/***********************
 * End of Macros       *
 ***********************/


/******************************
 * New structures             *
 ******************************/

/* type defintions */
typedef struct  dSocket       D_SOCKET;
typedef struct  dMobile       D_MOBILE;
typedef struct  help_data     HELP_DATA;
typedef struct  lookup_data   LOOKUP_DATA;
typedef struct  event_data    EVENT_DATA;
typedef struct  world_data    WORLD_DATA;

/* the actual structures */
struct dSocket
{
  D_SOCKET      * next;
  D_MOBILE      * player;
  LIST          * events;
  char          * hostname;
  char            inbuf[MAX_BUFFER];
  char            outbuf[MAX_OUTPUT];
  char            next_command[MAX_BUFFER];
  bool            bust_prompt;
  sh_int          lookup_status;
  sh_int          state;
  sh_int          control;
  sh_int          top_output;
  unsigned char   compressing;                 /* MCCP support */
  z_stream      * out_compress;                /* MCCP support */
  unsigned char * out_compress_buf;            /* MCCP support */
};

#ifdef IMC
   #include "imc.h"
#endif

struct dMobile
{
  D_SOCKET      * socket;
  LIST          * events;
  char          * name;
  char          * password;
  sh_int          level;
  int		  coordx;
  int		  coordy;
  int		  coordz;
  char          * title;
#ifdef IMC
    IMC_CHARDATA *imcchardata;
#endif
};

struct help_data
{
  HELP_DATA     * next;
  time_t          load_time;
  char          * keyword;
  char          * text;
};

struct lookup_data
{
  D_SOCKET       * dsock;   /* the socket we wish to do a hostlookup on */
  char           * buf;     /* the buffer it should be stored in        */
};

struct typCmd
{
  char      * cmd_name;
  void     (* cmd_funct)(D_MOBILE *dMOb, char *arg);
  sh_int      level;
};

typedef struct buffer_type
{
  char   * data;        /* The data                      */
  int      len;         /* The current len of the buffer */
  int      size;        /* The allocated size of data    */
} BUFFER;

struct world_data
{
  WORLD_DATA      * next;
  time_t            load_time;
  char            * location;
  char            * description;
  char            * exits;
};

/* here we include external structure headers */
#include "event.h"

/******************************
 * End of new structures      *
 ******************************/

/***************************
 * Global Variables        *
 ***************************/

extern  STACK       *   dsock_free;       /* the socket free list               */
extern  LIST        *   dsock_list;       /* the linked list of active sockets  */
extern  STACK       *   dmobile_free;     /* the mobile free list               */
extern  LIST        *   dmobile_list;     /* the mobile list of active mobiles  */
extern  LIST        *   help_list;        /* the linked list of help files      */
extern  const struct    typCmd tabCmd[];  /* the command table                  */
extern  bool            shut_down;        /* used for shutdown                  */
extern  char        *   greeting;         /* the welcome greeting               */
extern  char        *   motd;             /* the MOTD help file                 */
extern  int             control;          /* boot control socket thingy         */
extern  time_t          current_time;     /* let's cut down on calls to time()  */
extern  WORLD_DATA  *   world_list;       /* the linked list of world files     */
extern          FILE *                  fpReserve;

/*************************** 
 * End of Global Variables *
 ***************************/

/***********************
 *    MCCP support     *
 ***********************/

extern const unsigned char compress_will[];
extern const unsigned char compress_will2[];

#define TELOPT_COMPRESS       85
#define TELOPT_COMPRESS2      86
#define COMPRESS_BUF_SIZE   8192

/***********************
 * End of MCCP support *
 ***********************/

/***********************************
 * Prototype function declerations *
 ***********************************/

/* more compact */
#define  D_S         D_SOCKET
#define  D_M         D_MOBILE

#define  buffer_new(size)             __buffer_new     ( size)
#define  buffer_strcat(buffer,text)   __buffer_strcat  ( buffer, text )

char  *crypt                  ( const char *key, const char *salt );

/*
 * socket.c
 */
int   init_socket             ( void );
bool  new_socket              ( int sock );
void  close_socket            ( D_S *dsock, bool reconnect );
bool  read_from_socket        ( D_S *dsock );
bool  text_to_socket          ( D_S *dsock, const char *txt );  /* sends the output directly */
void  text_to_buffer          ( D_S *dsock, const char *txt );  /* buffers the output        */
void  text_to_mobile          ( D_M *dMob, const char *txt );   /* buffers the output        */
void  next_cmd_from_buffer    ( D_S *dsock );
bool  flush_output            ( D_S *dsock );
void  handle_new_connections  ( D_S *dsock, char *arg );
void  clear_socket            ( D_S *sock_new, int sock );
void  recycle_sockets         ( void );
void *lookup_address          ( void *arg );

/*
 * interpret.c
 */
void  handle_cmd_input        ( D_S *dsock, char *arg );

/*
 * io.c
 */
void    log_string            ( const char *txt, ... );
void    bug                   ( const char *txt, ... );
time_t  last_modified         ( char *helpfile );
char   *read_help_entry       ( const char *helpfile );     /* pointer         */
char   *fread_line            ( FILE *fp );                 /* pointer         */
char   *fread_string          ( FILE *fp );                 /* allocated data  */
char   *fread_word            ( FILE *fp );                 /* pointer         */
int     fread_number          ( FILE *fp );                 /* just an integer */

/* 
 * strings.c
 */
char   *one_arg               ( char *fStr, char *bStr );
char   *strdup                ( const char *s );
int     strcasecmp            ( const char *s1, const char *s2 );
bool    is_prefix             ( const char *aStr, const char *bStr );
char   *capitalize            ( char *txt );
BUFFER *__buffer_new          ( int size );
void    __buffer_strcat       ( BUFFER *buffer, const char *text );
void    buffer_free           ( BUFFER *buffer );
void    buffer_clear          ( BUFFER *buffer );
int     bprintf               ( BUFFER *buffer, char *fmt, ... );
bool    compares              ( const char *aStr, const char *bStr );

/*
 * help.c
 */
bool  check_help              ( D_M *dMob, char *helpfile );
void  load_helps              ( void );
void  add_help                ( HELP_DATA *help );

/*
 * utils.c
 */
bool  check_name              ( const char *name );
void  clear_mobile            ( D_M *dMob );
void  ex_free_mob             ( D_MOBILE * dMob );
void  free_mobile             ( D_M *dMob );
void  communicate             ( D_M *dMob, char *txt, int range );
void  load_muddata            ( bool fCopyOver );
char *get_time                ( void );
void  copyover_recover        ( void );
D_M  *check_reconnect         ( char *player );

/*
 * wiz.c
 */
void cmd_debug(D_MOBILE * ch, char * argument);
D_MOBILE *get_char(D_MOBILE * ch, char *argument);
void cmd_goto(D_MOBILE * ch, char *argument);

/*
 * action_safe.c
 */
char *  str_dup               ( const char *str );
bool  str_cmp                 ( const char *astr, const char *bstr );
void  cmd_say                 ( D_M *dMob, char *arg );
void  cmd_quit                ( D_M *dMob, char *arg );
void  cmd_shutdown            ( D_M *dMob, char *arg );
void  cmd_commands            ( D_M *dMob, char *arg );
void  cmd_who                 ( D_M *dMob, char *arg );
void  cmd_help                ( D_M *dMob, char *arg );
void  cmd_compress            ( D_M *dMob, char *arg );
void  cmd_save                ( D_M *dMob, char *arg );
void  cmd_copyover            ( D_M *dMob, char *arg );
void  cmd_linkdead            ( D_M *dMob, char *arg );
void  cmd_relevel             ( D_M *dMob, char *arg );
void  cmd_north               ( D_M *dMob, char *arg );
void  cmd_south               ( D_M *dMob, char *arg );
void  cmd_east                ( D_M *dMob, char *arg );
void  cmd_west                ( D_M *dMob, char *arg );
void  cmd_up                  ( D_M *dMob, char *arg );
void  cmd_down                ( D_M *dMob, char *arg );
//void  cmd_northeast           ( D_M *dMob, char *arg );
//void  cmd_southeast           ( D_M *dMob, char *arg );
//void  cmd_southwest           ( D_M *dMob, char *arg );
//void  cmd_northwest           ( D_M *dMob, char *arg );
void  cmd_look                ( D_M *dMob, char *arg );
void  cmd_title               ( D_M *dMob, char *arg );
void  safe_printf( const char *file, const char *function, int line, int size, char *str, char *fmt, ... );
void  append_dfile    ( D_MOBILE *ch, char *file, char *str );

#define xprintf(var,args...)   safe_printf (__FILE__, __FUNCTION__, __LINE__, sizeof(var), (var), ##args )

/*
 * mccp.c
 */
bool  compressStart           ( D_S *dsock, unsigned char teleopt );
bool  compressEnd             ( D_S *dsock, unsigned char teleopt, bool forced );

/*
 * save.c
 */
void  save_player             ( D_M *dMob );
D_M  *load_player             ( char *player );
D_M  *load_profile            ( char *player );

/*
 * version.c
 *
 */
void do_version ( D_MOBILE * dMob, char *argument );

/*
 * signal.c
 */
void init_sigguard            ( void );

/*
 * random.c
 */
int  random_range             ( int a, int b );
int  random_percent           ( void );
int  roll_dice                ( int amount, int size );
void init_dice                ( void );

/*
 * comm.c
 */
void cmd_chat(D_MOBILE * dMob, char *argument);
void stcf(D_MOBILE *dMob,const char *fmt,...);
char *one_argument( char *argument, char *arg_first );

/*
 * world.c
 */
void do_movement(D_MOBILE *dMob, int dir);

#define LOWER(c)                ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)                ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define MAX_KEY_HASH             1024
#define MAX_STRING_LENGTH        8192
#define MAX_INPUT_LENGTH          400
#define MAS                                262144

// Log types
#define LOG_CRIT       (A)
#define LOG_ERR        (B)
#define LOG_BUG        (C)
#define LOG_SECURITY   (D)
#define LOG_CONNECT    (E)
#define LOG_GAME       (F)
#define LOG_COMMAND    (G)
#define LOG_ALL        127   // All the others added up

#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH
#define COMPILE_TIME    __DATE__ " "__TIME__

/*******************************
 * End of prototype declartion *
 *******************************/

#endif  /* MUD_H */
