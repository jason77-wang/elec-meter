#ifndef foometerhfoo
#define foometerhfoo

struct meter_register {
  uint reg_addr;
  uint repeat_num;
  uint scale_addr;
  char *unit;
  char *dtype;
  struct meter_register *next;
};

struct ele_meter {
  char *name;
  unsigned int addr;

  struct meter_register *registers;
};

#endif
