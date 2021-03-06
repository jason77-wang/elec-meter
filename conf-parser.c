/***
  This file is part of PulseAudio.

  Copyright 2004-2006 Lennart Poettering

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "conf-parser.h"
#include "xmalloc.h"
#include "core-util.h"

#define WHITESPACE " \t\n"
#define COMMENTS "#;\n"

#define false 0
#define true 1

/* Run the user supplied parser for an assignment */
static int normal_assignment(pa_config_parser_state *state) {
    const pa_config_item *item;

    pa_assert(state);

    for (item = state->item_table; item->parse; item++) {

        if (item->lvalue && !pa_streq(state->lvalue, item->lvalue))
            continue;

        if (item->section && !state->section)
            continue;

        if (item->section && !pa_streq(state->section, item->section))
            continue;

        state->data = item->data;

        return item->parse(state);
    }

    pa_log("[%s:%u] Unknown lvalue '%s' in section '%s'.", state->filename, state->lineno, state->lvalue, pa_strna(state->section));

    return -1;
}
#if 0
/* Parse a proplist entry. */
static int proplist_assignment(pa_config_parser_state *state) {
    pa_assert(state);
    pa_assert(state->proplist);

    if (pa_proplist_sets(state->proplist, state->lvalue, state->rvalue) < 0) {
        pa_log("[%s:%u] Failed to parse a proplist entry: %s = %s", state->filename, state->lineno, state->lvalue, state->rvalue);
        return -1;
    }

    return 0;
}
#endif

/* Parse a variable assignment line */
static int parse_line(pa_config_parser_state *state) {
    char *c;

    state->lvalue = state->buf + strspn(state->buf, WHITESPACE);

    if ((c = strpbrk(state->lvalue, COMMENTS)))
        *c = 0;

    if (!*state->lvalue)
        return 0;
#if 0
    if (pa_startswith(state->lvalue, ".include ")) {
        char *path = NULL, *fn;
        int r;

        fn = pa_strip(state->lvalue + 9);
        if (!pa_is_path_absolute(fn)) {
            const char *k;
            if ((k = strrchr(state->filename, '/'))) {
                char *dir = pa_xstrndup(state->filename, k - state->filename);
                fn = path = pa_sprintf_malloc("%s" PA_PATH_SEP "%s", dir, fn);
                pa_xfree(dir);
            }
        }

        r = pa_config_parse(fn, NULL, state->item_table, state->userdata);
        pa_xfree(path);
        return r;
    }
#endif

	if (*state->lvalue == ']' || *state->lvalue == '}')
		return 0; /* this is the beginning of a new elec meter */

	if (*state->lvalue == '{')
		return 1; /* this is the beginning of a new register */

	if (*state->lvalue == '[')
		return 2; /* this is the beginning of a new meter */

#if 0
    if (*state->lvalue == '[') {
        size_t k;

        k = strlen(state->lvalue);
        pa_assert(k > 0);

        if (state->lvalue[k-1] != ']') {
            pa_log("[%s:%u] Invalid section header.", state->filename, state->lineno);
            return -1;
        }

        pa_xfree(state->section);
        state->section = pa_xstrndup(state->lvalue + 1, k-2);
        state->in_proplist = false;

        return 0;
    }
#endif
    if (!(state->rvalue = strchr(state->lvalue, '='))) {
        pa_log("[%s:%u] Missing '='.", state->filename, state->lineno);
        return -1;
    }

    *state->rvalue = 0;
    state->rvalue++;

    state->lvalue = pa_strip(state->lvalue);
    state->rvalue = pa_strip(state->rvalue);
#if 0
    if (state->in_proplist)
        return proplist_assignment(state);
    else
#endif
	    return normal_assignment(state);
}


/* r=-1 error, r=0 eof, r=1 new register, */
/* Go through the file and parse each line */
int pa_config_parse(FILE *f, pa_config_parser_state *pstate) {
    int r = -1;

    pa_assert(pstate);

    if (!f)
        goto finish;

    while (!feof(f)) {
      int ret; 
        if (!fgets(pstate->buf, sizeof(pstate->buf), f)) {
            if (feof(f))
                break;

            goto finish;
        }

        pstate->lineno++;
	
	ret = parse_line(pstate);
        if (ret != 0) {
	  r = ret;
            goto finish;
	}
    }

    r = 0;

finish:
    return r;
}

int pa_config_parse_int(pa_config_parser_state *state) {
    int *i;
    int32_t k;

    pa_assert(state);

    i = state->data;

    if (pa_atoi(state->rvalue, &k) < 0) {
        pa_log("[%s:%u] Failed to parse numeric value: %s", state->filename, state->lineno, state->rvalue);
        return -1;
    }

    *i = (int) k;
    return 0;
}

int pa_config_parse_unsigned(pa_config_parser_state *state) {
    unsigned *u;
    uint32_t k;

    pa_assert(state);

    u = state->data;

    if (pa_atou(state->rvalue, &k) < 0) {
        pa_log("[%s:%u] Failed to parse numeric value: %s", state->filename, state->lineno, state->rvalue);
        return -1;
    }

    *u = (unsigned) k;
    return 0;
}

int pa_config_parse_size(pa_config_parser_state *state) {
    size_t *i;
    uint32_t k;

    pa_assert(state);

    i = state->data;

    if (pa_atou(state->rvalue, &k) < 0) {
        pa_log("[%s:%u] Failed to parse numeric value: %s", state->filename, state->lineno, state->rvalue);
        return -1;
    }

    *i = (size_t) k;
    return 0;
}

int pa_config_parse_bool(pa_config_parser_state *state) {
    int k;
    bool *b;

    pa_assert(state);

    b = state->data;

    if ((k = pa_parse_boolean(state->rvalue)) < 0) {
        pa_log("[%s:%u] Failed to parse boolean value: %s", state->filename, state->lineno, state->rvalue);
        return -1;
    }

    *b = !!k;

    return 0;
}

int pa_config_parse_not_bool(pa_config_parser_state *state) {
    int k;
    bool *b;

    pa_assert(state);

    b = state->data;

    if ((k = pa_parse_boolean(state->rvalue)) < 0) {
        pa_log("[%s:%u] Failed to parse boolean value: %s", state->filename, state->lineno, state->rvalue);
        return -1;
    }

    *b = !k;

    return 0;
}

int pa_config_parse_string(pa_config_parser_state *state) {
    char **s;

    pa_assert(state);

    s = state->data;

    pa_xfree(*s);
    *s = *state->rvalue ? pa_xstrdup(state->rvalue) : NULL;
    return 0;
}
