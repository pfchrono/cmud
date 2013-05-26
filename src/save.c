#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* main header file */
#include "mud.h"

#define                    MAX_STRING      5413120
char *string_space;
char *top_string;
char *string_hash[MAX_KEY_HASH];
char str_empty[1];
int nAllocString;
int sAllocString;
int nAllocPerm;
int sAllocPerm;
void save_pfile           ( D_MOBILE *dMob );
void save_profile         ( D_MOBILE *dMob );

void save_player(D_MOBILE *dMob)
{
  if (!dMob) return;

  save_pfile(dMob);      /* saves the actual player data */
  save_profile(dMob);    /* saves the players profile    */
}

char *fread_string2(FILE * fp)
{
    char *plast;
    char c;
    plast = top_string + sizeof(char *);
    if (plast > &string_space[MAX_STRING - MAX_STRING_LENGTH]) {
        bug("Fread_string: MAX_STRING %d exceeded.", MAX_STRING);
        exit(1);
    }
    /*
     * Skip blanks.
     * Read first char.
     */
    do {
        c = getc(fp);
    }
    while (isspace(c));
    if ((*plast++ = c) == '~')
        return &str_empty[0];
    for (;;) {
        /*
         * Back off the char type lookup,
         *   it was too dirty for portability.
         *   -- Furey
         */
        switch (*plast = getc(fp)) {
        default:
            plast++;
            break;
        case EOF:
            /* temp fix */
            bug("Fread_string: EOF", 0);
            return NULL;
            /* exit( 1 ); */
            break;
        case '\n':
            plast++;
            *plast++ = '\r';
            break;
        case '\r':
            break;
        case '~':
            plast++;
            {
                union {
                    char *pc;
                    char rgc[sizeof(char *)];
                } u1;
                int ic;
                int iHash;
                char *pHash;
                char *pHashPrev;
                plast[-1] = '\0';
                iHash = UMIN(MAX_KEY_HASH - 1, plast - 1 - top_string);
                for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev) {
                    for (ic = 0; ic < sizeof(char *); ic++)
                        u1.rgc[ic] = pHash[ic];
                    pHashPrev = u1.pc;
                    pHash += sizeof(char *);
                    if (top_string[sizeof(char *)] == pHash[0]
                        && !strcmp(top_string + sizeof(char *) + 1, pHash + 1))
                         return pHash;
                }
                    return str_dup(top_string + sizeof(char *));
            }
        }
    }
}

void save_pfile(D_MOBILE *dMob)
{
  char pName[20];
  char pfile[256];
  FILE *fp;
  int size, i;

  pName[0] = toupper(dMob->name[0]);
  size = strlen(dMob->name);
  for (i = 1; i < size; i++)
    pName[i] = tolower(dMob->name[i]);
  pName[i] = '\0';

  /* open the pfile so we can write to it */
  sprintf(pfile, "../players/%s.pfile", pName);
  if ((fp = fopen(pfile, "w")) == NULL)
  {
    bug("Unable to write to %s's pfile", dMob->name);
    return;
  }

  /* dump the players data into the file */
  fprintf(fp, "Name            %s~\n", dMob->name);
  fprintf(fp, "Level           %d\n",  dMob->level);
  fprintf(fp, "Password        %s~\n", dMob->password);
  fprintf(fp, "Coordx          %d\n",  dMob->coordx);
  fprintf(fp, "Coordy          %d\n",  dMob->coordy);
  fprintf(fp, "Coordz          %d\n",  dMob->coordz);
  fprintf(fp, "Title           %s~\n", dMob->title);
#ifdef IMC
    imc_savechar( dMob, fp );
#endif

  /* terminate the file */
  fprintf(fp, "%s\n", FILE_TERMINATOR);
  fclose(fp);
}

D_MOBILE *load_player(char *player)
{
  FILE *fp;
  D_MOBILE *dMob = NULL;
  char pfile[256];
  char pName[20];
  char *word;
  bool done = FALSE, found;
  int i, size;

  pName[0] = toupper(player[0]);
  size = strlen(player);
  for (i = 1; i < size; i++)
    pName[i] = tolower(player[i]);
  pName[i] = '\0';

  /* open the pfile so we can write to it */
  sprintf(pfile, "../players/%s.pfile", pName);     
  if ((fp = fopen(pfile, "r")) == NULL)
    return NULL;

  /* create new mobile data */
  if (StackSize(dmobile_free) <= 0)
  {
    if ((dMob = malloc(sizeof(*dMob))) == NULL)
    {
      bug("Load_player: Cannot allocate memory.");
      abort();
    }
  }
  else
  {
    dMob = (D_MOBILE *) PopStack(dmobile_free);
  }
  clear_mobile(dMob);

#ifdef IMC
    imc_initchar( dMob );
#endif

  /* load data */
  word = fread_word(fp);
  while (!done)
  {
    found = FALSE;
    switch (word[0])
    {
      case 'C':
        if (compares(word, "Coordx")) { IREAD( "Coordx", dMob->coordx ); }
	if (compares(word, "Coordy")) { IREAD( "Coordy", dMob->coordy ); }
	if (compares(word, "Coordz")) { IREAD( "Coordz", dMob->coordz ); }
	break;
      case 'E':
        if (!strcasecmp(word, "EOF")) {done = TRUE; found = TRUE; break;}
        break;
      case 'I':
#ifdef IMC
           if( ( found = imc_loadchar( dMob, fp, word ) ) )
                break;
#endif
        break;
      case 'L':
        IREAD( "Level",     dMob->level     );
        break;
      case 'N':
        SREAD( "Name",      dMob->name      );
        break;
      case 'P':
        SREAD( "Password",  dMob->password  );
        break;
      case 'T':
      SREAD( "Title",     dMob->title     );
        break;
    }
    if (!found)
    {
      bug("Load_player: unexpected '%s' in %s's pfile.", word, player);
      free_mobile(dMob);
      return NULL;
    }

    /* read one more */
    if (!done) word = fread_word(fp);
  }

  fclose(fp);
  return dMob;
}

/*
 * This function loads a players profile, and stores
 * it in a mobile_data... DO NOT USE THIS DATA FOR
 * ANYTHING BUT CHECKING PASSWORDS OR SIMILAR.
 */
D_MOBILE *load_profile(char *player)
{
  FILE *fp;
  D_MOBILE *dMob = NULL;
  char pfile[256];
  char pName[20];
  char *word;
  bool done = FALSE, found;
  int i, size;

  pName[0] = toupper(player[0]);
  size = strlen(player);
  for (i = 1; i < size; i++)
    pName[i] = tolower(player[i]);
  pName[i] = '\0';

  /* open the pfile so we can write to it */
  sprintf(pfile, "../players/%s.profile", pName);
  if ((fp = fopen(pfile, "r")) == NULL)
    return NULL;

  /* create new mobile data */
  if (StackSize(dmobile_free) <= 0)
  {
    if ((dMob = malloc(sizeof(*dMob))) == NULL)
    {
      bug("Load_profile: Cannot allocate memory.");
      abort();
    }
  }
  else
  {
    dMob = (D_MOBILE *) PopStack(dmobile_free);
  }
  clear_mobile(dMob);

  /* load data */
  word = fread_word(fp);
  while (!done)
  {
    found = FALSE;
    switch (word[0])
    {
      case 'E':
        if (!strcasecmp(word, "EOF")) {done = TRUE; found = TRUE; break;}
        break;
      case 'N':
        SREAD( "Name",      dMob->name      );
        break;
      case 'P':
        SREAD( "Password",  dMob->password  );
        break;
    }
    if (!found)
    {
      bug("Load_player: unexpected '%s' in %s's pfile.", word, player);
      free_mobile(dMob);
      return NULL;
    }

    /* read one more */
    if (!done) word = fread_word(fp);
  }

  fclose(fp);
  return dMob;
}


/*
 * This file stores only data vital to load
 * the character, and check for things like
 * password and other such data.
 */
void save_profile(D_MOBILE *dMob)
{
  char pfile[256];
  char pName[20];
  FILE *fp;
  int size, i;
  
  pName[0] = toupper(dMob->name[0]);
  size = strlen(dMob->name);
  for (i = 1; i < size; i++)
    pName[i] = tolower(dMob->name[i]);
  pName[i] = '\0';
  
  /* open the pfile so we can write to it */
  sprintf(pfile, "../players/%s.profile", pName);
  if ((fp = fopen(pfile, "w")) == NULL)
  {
    bug("Unable to write to %s's pfile", dMob->name);
    return;
  }

  /* dump the players data into the file */
  fprintf(fp, "Name           %s~\n", dMob->name);
  fprintf(fp, "Password       %s~\n", dMob->password);

  /* terminate the file */
  fprintf(fp, "%s\n", FILE_TERMINATOR);
  fclose(fp);
}
