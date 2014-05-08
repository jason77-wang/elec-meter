#include <stdlib.h>
#include <stdio.h>

#include "meter.h"
#include "conf-parser.h"

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

int main ()
{
	int r;
	FILE *f;
	const char *filename = "./elec_meters.conf";
	void *userdata;
	struct meter_register **next_reg;

	if (!(f = pa_fopen_cloexec(filename, "r"))) {
		printf("open meter config failed.\n");
		goto error_exit;
	}

	state.filename = filename;
	state.item_table = items;
	state.userdata = userdata;

	glb_meter = malloc(sizeof(struct ele_meter));
	
	next_reg = &glb_meter->registers;
	
	fill_parser_state_meter(items, glb_meter);

parse_again:
	r = pa_config_parse(f, &state);

	if (r == 1) {
		struct meter_register *reg;
		reg = malloc(sizeof(struct meter_register));
		fill_parser_state_register(items, reg);		
		*next_reg = reg;
		next_reg = &reg->next;
		goto parse_again;
	}





	printf("r = %d\n", r);
	printf("name = %s\n", glb_meter->name);
	printf("addr = %d\n", glb_meter->addr);
	printf("reg_addr = %d, repeat_num = %d, unit = %s\n", glb_meter->registers->reg_addr,
	       glb_meter->registers->repeat_num, glb_meter->registers->unit);

	printf("reg_addr = %d, repeat_num = %d, unit = %s\n", glb_meter->registers->next->reg_addr,
	       glb_meter->registers->next->repeat_num, glb_meter->registers->next->unit);

	return 0;

 error_exit:
	return -1;
}
