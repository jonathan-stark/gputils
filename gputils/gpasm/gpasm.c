/* top level functions for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003
   James Bowman, Craig Franklin

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"

#include "libgputils.h"
#include "gpasm.h"
#include "gperror.h"
#include "scan.h"
#include "directive.h"
#include "lst.h"
#include "cod.h"
#include "processor.h"
#include "coff.h"

struct gpasm_state state;

static int cmd_processor = 0;
static char *processor_name = NULL;

int yyparse(void);

#define GET_OPTIONS "?D:I:La:cd:e:ghilmno:p:qr:vw:"

/* Used: acdDehiIlmopqrwv */
static struct option longopts[] =
{
  { "define",      1, 0, 'D' },
  { "include",     1, 0, 'I' },
  { "hex-format",  1, 0, 'a' },
  { "object",      0, 0, 'c' },
  { "define",      1, 0, 'd' },
  { "expand",      1, 0, 'e' },
  { "debug-info",  0, 0, 'g' },
  { "help",        0, 0, 'h' },
  { "ignore-case", 0, 0, 'i' },
  { "force-list",  0, 0, 'L' },
  { "list-chips",  0, 0, 'l' },
  { "dump",        0, 0, 'm' },
  { "dos",         0, 0, 'n' },
  { "output",      1, 0, 'o' },
  { "processor",   1, 0, 'p' },
  { "quiet",       0, 0, 'q' },
  { "radix",       1, 0, 'r' },
  { "version",     0, 0, 'v' },
  { "warning",     1, 0, 'w' },
  { 0, 0, 0, 0 }
};

void
init(void)
{
  /* restore gpasm to its initialized state */
  state.mode = absolute;

  state.radix = 16;
  state.hex_format = inhx32;
  state.case_insensitive = 0;
  state.quiet = 0;
  state.error_level = 0;
  state.debug_info = 0;
  state.path_num = 0;

  state.cmd_line.radix = 0;
  state.cmd_line.hex_format = 0;
  state.cmd_line.error_level = 0;
  state.cmd_line.macro_expand = 0;
  state.cmd_line.processor = 0;
  state.cmd_line.lst_force = 0;

  state.pass = 0;
  state.org = 0;
  state.dos_newlines = 0;
  state.memory_dump = 0;
  state.found_config = 0;
  state.found_devid = 0;
  state.found_idlocs = 0;
  state.maxram = (MAX_RAM - 1);

  state.codfile = normal;
  state.hexfile = normal;
  state.lstfile = normal;
  state.objfile = suppress;

  state.num.errors    = 0;
  state.num.warnings  = 0;
  state.num.messages  = 0;
  state.num.warnings_suppressed = 0;
  state.num.messages_suppressed = 0;
  
  state.processor = no_processor;
  state.processor_chosen = 0;

  state.obj.object = NULL;
  state.obj.section = NULL;
  state.obj.symbol_num = 0;
  state.obj.section_num = 0;
  state.obj.org_num = 0;
  
  state.next_state = _nochange;
  
  return;
}

static void 
add_path(char *path)
{
  if(state.path_num < MAX_PATHS) {
    state.paths[state.path_num++] = strdup(path);
  } else {
    fprintf(stderr, "too many -I paths\n");
    exit(1);
  }
}

static void
show_usage(void)
{
  printf("Usage: gpasm [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a FMT, --hex-format FMT       Select hex file format. [inhx32]\n");
  printf("  -c, --object                   Output relocatable object.\n");
  printf("  -D SYM=VAL, --define SYM=VAL   Define SYM with value VAL.\n");
  printf("  -e [ON|OFF], --expand [ON|OFF] Macro expansion.\n");
  printf("  -g, --debug-info               Use debug directives for COFF.\n");
  printf("  -h, --help                     Show this usage message.\n");
  printf("  -i, --ignore-case              Case insensitive.\n");
  printf("  -I DIR, --include DIR          Specify include directory.\n");
  printf("  -L, --force-list               Ignore nolist directives.\n");
  printf("  -l, --list-chips               List supported processors.\n");
  printf("  -m, --dump                     Memory dump.\n");
  #ifndef HAVE_DOS_BASED_FILE_SYSTEM
  printf("  -n, --dos                      Use DOS newlines in hex file.\n");
  #endif
  printf("  -o FILE, --output FILE         Alternate name of output file.\n");
  printf("  -p PROC, --processor PROC      Select processor.\n");
  printf("  -q, --quiet                    Quiet.\n");
  printf("  -r RADIX, --radix RADIX        Select radix. [hex]\n");
  printf("  -w [0|1|2], --warning [0|1|2]  Set message level. [0]\n");
  printf("  -v, --version                  Show version.\n");
  printf("\n");
  #ifdef USE_DEFAULT_PATHS
    #ifdef HAVE_DOS_BASED_FILE_SYSTEM
      printf("Reading header files from %s\n", DOS_HEADER_PATH);    
    #else
      printf("Reading header files from %s\n", GPASM_HEADER_PATH);
    #endif
    printf("\n");    
  #endif
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

void
process_args( int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int usage = 0;
  char *pc;

  /* Scan through the options for the -c flag.  It must be set before the 
     defines are read */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case 'i':
      state.case_insensitive = 1;
      break;
    }
  }

  /* reset the getopt_long index for the next call */
  optind = 1;

  /* initalize the defines table for command line arguments */
  state.stDefines = push_symbol_table(NULL, state.case_insensitive);

  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, 0)) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'a':
      select_hexformat(optarg);
      state.cmd_line.hex_format = 1;
      break;
    case 'c':
      state.mode    = relocatable;
      state.codfile = suppress;
      state.hexfile = suppress;
      state.lstfile = normal;
      state.objfile = normal;
      break;
    case 'D':
    case 'd':
      if ((optarg != NULL) && (strlen(optarg) > 0)) {
	struct symbol *sym;
	char *lhs, *rhs;

	lhs = strdup(optarg);
	rhs = strchr(lhs, '=');
	if (rhs != NULL) {
	  *rhs = '\0';	/* Terminate the left-hand side */
	  rhs++;	/* right-hand side begins after the '=' */
	}

	sym = get_symbol(state.stDefines, lhs);
	if (sym == NULL)
	  sym = add_symbol(state.stDefines, lhs);
	if (rhs)
	  annotate_symbol(sym, rhs);
      }
      break;
    case 'e':
      select_expand(optarg);
      state.cmd_line.macro_expand = 1;
      break;
    case 'g':
      state.debug_info = 1;
      break;
    case 'I':
      add_path(optarg);
      break;    
    case 'i':
      state.case_insensitive = 1;
      break;
    case 'L':
      state.cmd_line.lst_force = 1;
      break;  
    case 'l':
      gp_dump_processor_list();
      exit(0);
      break;
    case 'm':
      state.memory_dump = 1;
      break;
    case 'n':
      #ifndef HAVE_DOS_BASED_FILE_SYSTEM
        state.dos_newlines = 1;
      #endif
      break;
    case 'o':
      strcpy(state.hexfilename, optarg);
      strcpy(state.basefilename, optarg);
      pc = strrchr(state.basefilename, '.');
      if (pc)
        *pc = 0;
      break;
    case 'p':
      cmd_processor = 1;
      processor_name = optarg;
      break;
    case 'q':
      state.quiet = 1;
      break;
    case 'r':
      select_radix(optarg);
      state.cmd_line.radix = 1;
      break;
    case 'w':
      select_errorlevel(atoi(optarg));
      state.cmd_line.error_level = 1;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPASM_VERSION_STRING);
      exit(0);
      
    }
    if (usage)
      break;
  }
  
  if ((optind + 1) == argc)
    state.srcfilename = argv[optind];
  else
    usage = 1;

  if (usage) {
    show_usage();
  }

  /* Add the header path to the include paths list last, so that the user
     specified directories are searched first */
  #ifdef USE_DEFAULT_PATHS
    #ifdef HAVE_DOS_BASED_FILE_SYSTEM
      add_path(DOS_HEADER_PATH);
    #else
      add_path(GPASM_HEADER_PATH);
    #endif
  #endif

}

int
assemble(void)
{
  char *pc; 
  struct symbol_table *cmd_defines;

  /* store the command line defines to restore on second pass */
  cmd_defines = state.stDefines;
  state.c_memory = state.i_memory = i_memory_create();

  if(state.basefilename[0] == '\0') {
    strcpy(state.basefilename, state.srcfilename);
    pc = strrchr(state.basefilename, '.');
    if (pc)
      *pc = 0;
  }

  /* Builtins are always case insensitive */
  state.stBuiltin = push_symbol_table(NULL, 1);
  state.stDirective = state.stBuiltin;
  state.stMacros = push_symbol_table(NULL, state.case_insensitive);
  state.stTop = 
    state.stGlobal = push_symbol_table(NULL, state.case_insensitive);
  state.stTopDefines = 
    state.stDefines = push_symbol_table(cmd_defines, state.case_insensitive);

  opcode_init(0);

  /* the tables are built, select the processor if -p was used */
  if (cmd_processor) {
    select_processor(processor_name);
    state.cmd_line.processor = 1;
  }

  state.pass = 1;
  open_src(state.srcfilename, 0);
  yyparse();
 
  state.pass++;
  state.org = 0;
  state.cblock = 0;
  /* clean out defines for second pass */
  state.stTopDefines = 
    state.stDefines = push_symbol_table(cmd_defines, state.case_insensitive);
  if (state.cmd_line.radix != 1)
    state.radix = 16;
  state.obj.symbol_num = 0;
  state.obj.section_num = 0;
  state.obj.org_num = 0;
  state.found_config = 0;
  state.found_devid = 0;
  state.found_idlocs = 0;
  coff_init();
  cod_init();
  lst_init();
  open_src(state.srcfilename, 0);
  yyparse();

  assert(state.pass == 2);
  
  pop_symbol_table(state.stBuiltin);
  
  hex_init();

  if(state.memory_dump)
    print_i_memory(state.i_memory, 
                   state.device.class == PROC_CLASS_PIC16E ? 1 : 0);

  /* Maybe produce a symbol table */
  if (state.lst.symboltable) {
    lst_throw(); /* Start symbol table on a fresh page */
    lst_symbol_table(state.stGlobal);
    lst_defines_table(state.stDefines);
  }

  /* Maybe produce a memory map */
  if ((state.mode == absolute) && (state.lst.memorymap)) {
    lst_memory_map(state.i_memory);
  }
  
  /* Finish off the object, listing, and symbol files*/
  coff_close_file();
  lst_close();
  if (state.processor_info)
    cod_close_file();
  free_files();

  if ((state.num.errors > 0) ||
      (gp_num_errors > 0))
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}
