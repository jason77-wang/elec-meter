#ifndef foometerhfoo
#define foometerhfoo

struct meter_register {
	uint reg_addr;
	unsigned short reg_val;
	uint repeat_num;
	uint scale_addr;
	unsigned short scale_val;
	char *unit;
	char *dtype;
	struct meter_register *next;
};

struct ele_meter {
	char *name;
	unsigned int addr;

	struct meter_register *registers;

	struct ele_meter *next;
};

int mb_read_all_meters(struct ele_meter *imeter);

#endif
