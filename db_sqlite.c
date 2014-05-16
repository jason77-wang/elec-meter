#include <stdlib.h>
#include <stdio.h>

#include "core-util.h"
#include "meter.h"
#include "db_sqlite.h"
#include "xmalloc.h"

#define GLB_METER_TABLE "create table meters_regs\
(ID integer primary key autoincrement,\
 dat date,\
 meter_name nvarchar(128),\
 meter_addr integer,\
 reg_addr integer,\
 reg_val integer,\
 scal_addr integer,\
 scal_val integer,\
 repeat_num integer,\
 unit nvarchar(16),\
 dtype nvarchar(16)\
)"

/* we want to build a table like this:
ID, date, meter_name, meter_addr, reg_addr, reg_val, scal_addr, scal_val, repeat_num, unit, dtype.
 */


static bool db_file_exit(const char* filename)
{
	return is_file_exist(filename);
}

sqlite3 * db_glb_init(const char* dbname)
{
	sqlite3 *db = NULL;
	char *errmsg = NULL;
	int r, exist = 0;

	if (db_file_exit(dbname))
		exist = 1;

	printf("%s\n", GLB_METER_TABLE);

	r = sqlite3_open(dbname, &db);
	if(r != SQLITE_OK)
		return NULL;

	if (exist)
		return db;

	r = sqlite3_exec(db, GLB_METER_TABLE, NULL, NULL, &errmsg);
	if(r != SQLITE_OK ) {
		printf("Create table failed. err_code:%d，err_msg:%s/n", r, errmsg );
		sqlite3_close(db);
		return NULL;
	}

	return db;
}

static void db_insert_single_reg(sqlite3 *db, struct ele_meter *pmeter, struct meter_register *preg)
{
	int r;
	char *cmd, *errmsg, *date_time;

	date_time = get_current_date_time();

	cmd = pa_sprintf_malloc("insert into meters_regs(dat, meter_name, meter_addr, reg_addr, reg_val, scal_addr, scal_val, repeat_num, unit, dtype) \
				values('%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%s', '%s')",
				date_time, pmeter->name, pmeter->addr, preg->reg_addr, preg->reg_val, preg->scale_addr, preg->scale_val, preg->repeat_num, preg->unit, preg->dtype);
	r = sqlite3_exec(db, cmd, 0, 0, &errmsg);
	pa_xfree(cmd);
	pa_xfree(date_time);
}

static void db_insert_meter(sqlite3 *db, struct ele_meter *pmeter)
{
	struct meter_register *preg;

	preg = pmeter->registers;

	while(preg) {
		db_insert_single_reg(db, pmeter, preg);
		preg = preg->next;
	}
}


void db_insert_meters(sqlite3 *db, struct ele_meter *pmeter)
{
	struct ele_meter * t_pm = pmeter;

	while(t_pm) {
		db_insert_meter(db, t_pm);
		t_pm = t_pm->next;
	}
}

void db_glb_close(sqlite3 *db)
{
	sqlite3_close(db);
}
