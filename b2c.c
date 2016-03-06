/*
 *  b2c(1) -- Copyright (c) 1991-2016 Joerg Heitkoetter. All rights reserved.
 *
 *  Send bugs, comments, and all the fish to joke@verizon.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, 59 Temple Place--Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef lint
static char *rcsid = "$Id: b2c.c,v 2.1 1997/11/21 14:51:34 joke Rel $";
#endif


#include <stdio.h>
#include <getopt.h>

#define ROWSIZ	8

char *program_name;
char *program_version = "2.3";
char *program_uri = "https://github.com/jheitkoetter/gnuland/b2c/";

int Hflag = 1;			/* hex encoding: default */
int Cflag = 0;			/* decimal encoding */
int Oflag = 0;			/* octal encoding */

main (argc, argv)
     int argc;
     char **argv;
{
  FILE *fp;
  char *macro = "FOOSIZ";
  char *id = "foo";
  char *ct = "unsigned char";
  int c, cols = ROWSIZ;
  int guarded = 0;
  char *guard = "_foo_h_";

  program_name = *argv;
  while ((c = getopt (argc, argv, "CHOc:D:G:hi:o:t:v")) != EOF)
    switch (c)
      {

      case 'C':
	Cflag = 1;
	Hflag = Oflag = 0;	/* turn others off */
	break;

      case 'H':
	Hflag = 1;
	Cflag = Oflag = 0;	/* turn others off */
	break;

      case 'O':
	Oflag = 1;
	Hflag = Cflag = 0;	/* turn others off */
	break;

      case 'c':
	cols = atoi (optarg);
	if (cols <= 0)
	  cols = ROWSIZ;
	break;

      case 'D':
	macro = optarg;
	break;

      case 'G':
	guarded++;
	guard = optarg;
	break;

      case 'h':
	guarded++;
	break;

      case 'i':
	id = optarg;
	break;

      case 'o':
	if (freopen (optarg, "w", stdout) == NULL)
	  {
	    fprintf (stderr, "%s: cannot redirect: %s\n", program_name, optarg);
	    return (-1);
	  }
	break;

      case 't':
	ct = optarg;
	break;

      case 'v':
	note ();
	break;

      default:
	usage (1);
	break;
      }

  if (!(argc - optind))		/* nothing left */
    b2c (stdin, "<stdin>", cols, macro, id, ct, guarded, guard);
  else
    for (; optind < argc; optind++)
      {
	if ((fp = fopen (argv[optind], "r")) == NULL)
	  {
	    fprintf (stderr, "%s: No such file: %s\n", program_name, argv[optind]);
	    continue;
	  }
	b2c (fp, argv[optind], cols, macro, id, ct, guarded, guard);
	fclose (fp);
      }
  return (0);
}

b2c (fp, file, cols, macro, id, ct, guarded, guard)
     FILE *fp;
     char *file;
     int cols;
     char *macro;
     char *id;
     char *ct;
     int guarded;
     char *guard;
{
  int i = 0, c, peek;

  /* prelude */
  printf ("/* PLEASE DO NOT EDIT THIS FILE. IT HAS BEEN CREATED AUTOMATICALLY BY THE B2C CONVERTER */\n");
  printf ("/* b2c version %s is available at %s */\n\n", program_version, program_uri);
  printf ("/* %s */\n", file);
  if (guarded)
    printf ("#ifndef %s\n#define %s\n\n", guard, guard);
  printf ("%s %s [] = {\n\t", ct, id);

  /* interlude */
  while ((c = getc (fp)) != EOF)
    {
      if ((peek = getc (fp)) == EOF)
	break;
      ungetc (peek, fp);

      if (Hflag)
	printf ("0x%02x,", c & 0xff);
      else if (Cflag)
	printf ("%3d,", c & 0xff);
      else if (Oflag)
	printf ("0%003o,", c & 0xff);

      if (!(++i % cols))
	fputs ("\n\t", stdout);
    }

  /* postlude */
  if (Hflag)
    printf ("0x%02x", c & 0xff);
  else if (Cflag)
    printf ("%3d", c & 0xff);
  else if (Oflag)
    printf ("0%003o", c & 0xff);

  printf ("\n};\n\n#define %s\t(sizeof(%s)/sizeof(%s))\n", macro, id, ct);
  if (guarded)
    printf ("\n#endif\t/* %s */\n", guard);
}

usage (code)
     int code;
{
  fprintf (stderr, "usage: %s [-hvCHO] [-c <#columns>] [-i <identifier>] [-o output] [-t type] [-D size-macro-name] [-G guard-macro-name]\n", program_name);
  exit (code);
}

note ()
{
  fprintf (stderr, "This is %s, version %s\n", program_name, program_version);
  fprintf (stderr, "Copyright (c) 1991-2016 Joerg Heitkoetter. All rights reserved.\n");
  fprintf (stderr, "Send comments, bugs, and all the fish to joke@verizon.com\n");
}
