/*
 * Copyright © 2008-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

#include "meter.h"

void mb_read_meter(struct ele_meter *pmeter)
{
	modbus_t *ctx;
	int rc;
	struct meter_register *preg;

        ctx = modbus_new_rtu("/dev/ttyS0", 9600, 'N', 8, 1);
	modbus_set_slave(ctx, pmeter->addr);

	modbus_set_debug(ctx, TRUE);

	rc = modbus_connect(ctx);
        if (rc == -1) {
		fprintf(stderr, "Unable to connect %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return;
        }

	preg = pmeter->registers;
	
	while(preg) {
		rc = modbus_read_registers(ctx, preg->reg_addr, 2, &preg->reg_val);
		if (rc != 0) {
			preg = preg->next;
			continue;
		}
		rc = modbus_read_registers(ctx, preg->scale_addr, 2, &preg->scale_val);
		preg = preg->next;
	}
	printf("...................rc = %d\n", rc);

	modbus_close(ctx);
	modbus_free(ctx);

	return;
}

int mb_read_all_meters(struct ele_meter *imeter)
{
	struct ele_meter *pmeter;

	pmeter = imeter;

	while (pmeter) {
		mb_read_meter(pmeter);
		pmeter = pmeter->next;
	}

	return 0;
}
