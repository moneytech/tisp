/* See LICENSE file for copyright and license details. */
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tisp.h"
#ifndef TIB_DYNAMIC
	tsp_include_tib(math);
	tsp_include_tib(io);
	tsp_include_tib(time);
	tsp_include_tib(string);
#endif

int
main(int argc, char *argv[])
{
	int i = 1;
	Val v = NULL;

	Tsp st = tisp_env_init(64);
#ifndef TIB_DYNAMIC
	tib_env_math(st);
	tib_env_io(st);
	tib_env_time(st);
	tib_env_string(st);
#  include "libs.tsp.h"
	tisp_env_lib(st, libs_tsp);
#endif

	if (argc == 1) {
		st->file = "(repl)";
		goto readstr;
	}

	for (; i < argc; i++, v = NULL) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'c') { /* run next argument as tisp command */
				if (!(st->file = argv[++i])) {
					fputs("tisp: expected command after -c\n", stderr);
					exit(2);
				}
readstr:
				if ((v = tisp_read(st)))
					v = tisp_eval(st, st->global, v);
			} else if (argv[i][1] == 'v') { /* version and copyright info */
				fprintf(stderr, "tisp v%s (c) 2017-2020 Ed van Bruggen\n", VERSION);
				exit(0);
			} else if (argv[i][1]) { /* unsupported argument or help */
				fputs("usage: tisp [-hv] [-c COMMAND] [-] [FILE ...]\n", stderr);
				exit(argv[i][1] == 'h' ? 0 : 1);
			} else { /* single hypen read from stdin */
				v = tisp_eval_seq(st, st->global, tisp_parse_file(st, NULL));
			}
		} else { /* otherwise read as file */
			v = tisp_eval_seq(st, st->global, tisp_parse_file(st, argv[i]));
		}
		if (v) tisp_print(stdout, v);
	}

	puts("");


	return 0;
}
