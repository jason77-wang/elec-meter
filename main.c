#include <stdlib.h>
#include <stdio.h>

#include "conf-parser.h"

    pa_config_item items[] = {
        /* [General] */
        { "slave-addr",            pa_config_parse_unsigned,          NULL, NULL },
        { "name",     pa_config_parse_string,            NULL, NULL },
#if 0
        { "description",         pa_config_parse_string,            NULL, "General" },
        { "mute-during-activation", pa_config_parse_bool,           NULL, "General" },
        { "eld-device",          pa_config_parse_int,               NULL, "General" },

        /* [Option ...] */
        { "priority",            option_parse_priority,             NULL, NULL },
        { "name",                option_parse_name,                 NULL, NULL },

        /* [Jack ...] */
        { "state.plugged",       jack_parse_state,                  NULL, NULL },
        { "state.unplugged",     jack_parse_state,                  NULL, NULL },

        /* [Element ...] */
        { "switch",              element_parse_switch,              NULL, NULL },
        { "volume",              element_parse_volume,              NULL, NULL },
        { "enumeration",         element_parse_enumeration,         NULL, NULL },
        { "override-map.1",      element_parse_override_map,        NULL, NULL },
        { "override-map.2",      element_parse_override_map,        NULL, NULL },
        /* ... later on we might add override-map.3 and so on here ... */
        { "required",            element_parse_required,            NULL, NULL },
        { "required-any",        element_parse_required,            NULL, NULL },
        { "required-absent",     element_parse_required,            NULL, NULL },
        { "direction",           element_parse_direction,           NULL, NULL },
        { "direction-try-other", element_parse_direction_try_other, NULL, NULL },
        { "volume-limit",        element_parse_volume_limit,        NULL, NULL },
#endif
        { NULL, NULL, NULL, NULL }
    };

struct ele_meter {
  char *name;
  unsigned int addr;
};

struct ele_meter my_meter;
int main ()
{
	int r;
	
	items[0].data = &my_meter.addr;
	items[1].data = &my_meter.name;
	r = pa_config_parse("./elec_meters.conf", NULL, items, NULL);

	printf("name = %s\n", my_meter.name);
	printf("addr = %d\n", my_meter.addr);
}
