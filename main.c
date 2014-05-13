#include <stdlib.h>
#include <stdio.h>

#include "meter.h"
#include "conf-parser.h"
#include "db_sqlite.h"

static pa_config_item items[] = {
	/* [meter] */
	{"slave-addr",		pa_config_parse_unsigned, NULL, NULL},
	{"name",		pa_config_parse_string, NULL, NULL},
	/* [meter registers] */
	{"register-addr",	pa_config_parse_unsigned, NULL, NULL},
	{"repeat-num",		pa_config_parse_unsigned, NULL, NULL},
	{"scale-addr",		pa_config_parse_unsigned, NULL, NULL},
	{"unit",		pa_config_parse_string, NULL, NULL},
	{"data_type",		pa_config_parse_string, NULL, NULL},
        {NULL, NULL, NULL, NULL}
};

struct ele_meter *glb_meter;
static pa_config_parser_state state;

static void fill_parser_state_meter(struct pa_config_item *items,
				    struct ele_meter *pmeter)
{
	items[0].data = &pmeter->addr;
	items[1].data = &pmeter->name;
}

static void fill_parser_state_register(struct pa_config_item *items,
				       struct meter_register *reg)
{
	items[2].data = &reg->reg_addr;
	items[3].data = &reg->repeat_num;
	items[4].data = &reg->scale_addr;
	items[5].data = &reg->unit;
	items[6].data = &reg->dtype;
}

void print_register_info(struct meter_register *preg)
{
	printf("reg_addr = %d, repeat_num = %d, unit = %s, scale_addr = %d\n", preg->reg_addr,
	       preg->repeat_num, preg->unit, preg->scale_addr);
	printf("\n");
}

void print_meter_registers(struct ele_meter *pmeter)
{
	struct meter_register *preg;

	preg = pmeter->registers;
	while (preg) {
		print_register_info(preg);
		preg = preg->next;
	}
}

void print_meter_info(struct ele_meter *pmeter)
{
	printf("\n\n\n");
	printf("name = %s\n", pmeter->name);
	printf("addr = %d\n", pmeter->addr);
}

void print_all_meters()
{
	struct ele_meter *pmeter;

	pmeter = glb_meter;

	while (pmeter) {
		print_meter_info(pmeter);
		print_meter_registers(pmeter);
		pmeter = pmeter->next;
	}
}

int build_glb_meter_list(FILE *f, pa_config_parser_state *pstate)
{
	int r;
	struct ele_meter **next_meter;
	struct meter_register **next_reg;


	next_meter = &glb_meter;

parse_again:
	r = pa_config_parse(f, pstate);

	/* r = 1 means a new register is found */
	if (r == 1) {
		struct meter_register *reg;
		reg = malloc(sizeof(struct meter_register));
		fill_parser_state_register(items, reg);		
		*next_reg = reg;
		next_reg = &reg->next;
		goto parse_again;
	}

	/* r = 2 means a new meter is found */
	if (r == 2) {
		struct ele_meter *pmeter;
		
		pmeter = malloc(sizeof(struct ele_meter));
		fill_parser_state_meter(items, pmeter);
		*next_meter = pmeter;
		next_meter = &pmeter->next;
		next_reg = &pmeter->registers;
		goto parse_again;
	}

	printf("r = %d\n", r);

	return r;
}

int main ()
{
	FILE *f;
	int r;

	const char *filename = "./elec_meters.conf";
	if (!(f = pa_fopen_cloexec(filename, "r"))) {
		printf("open meter config failed.\n");
		goto error_exit;
	}

	state.filename = filename;
	state.item_table = items;

	r = build_glb_meter_list(f, &state);
	if (r != 0)
		goto error_exit;

	mb_read_all_meters(glb_meter);

	print_all_meters();

	sqlite_test();
error_exit:
	return -1;
}
