#include <stdlib.h>
#include <stdio.h>

#include <sqlite3.h>

#include "core-util.h"
#include "meter.h"

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
 dtype nvarchar(16),\
)"

/* we want to build a table like this:
ID, date, meter_name, meter_addr, reg_addr, reg_val, scal_addr, scal_val, repeat_num, unit, dtype.
 */

static bool db_file_exit(const char* filename)
{
	return is_file_exist(filename);
}

int db_glb_init(const char* dbname)
{
	sqlite3* db = NULL;
	char *errmsg = NULL;
	int r;

	if (db_file_exit(dbname))
		return 0;

	printf("%s\n", GLB_METER_TABLE);

	r = sqlite3_open(dbname, &db);
	if(r != SQLITE_OK)
		return r;

	r = sqlite3_exec(db, GLB_METER_TABLE, NULL, NULL, &errmsg);
	if(r != SQLITE_OK ) {
		printf("Create table failed. err_code:%d，err_msg:%s/n", r, errmsg );
		sqlite3_close(db);
		return r;
	}

	return 0;
}

static void db_insert_single_reg(struct ele_meter *pmeter, struct meter_register *preg)
{
	int r;

	r = sqlite3_exec(db, "insert into MyTable_1(name) values ('走路' )", 0, 0, errmsg);
}

static void db_insert_meter(struct ele_meter *pmeter)
{
	struct meter_register *preg;

	preg = pmeter->registers;

	while(preg) {
		db_insert_single_reg(pmeter, preg);
		preg = preg->next;
	}
}


void db_insert_meters(struct ele_meter *pmeter)
{
	struct ele_meter * t_pm = pmeter;

	while(t_pm) {
		db_insert_meter(t_pm);
		t_pm = t_pm->next;
	}
}



#if 0
int sqlite_test()
{
	sqlite3* db = NULL;
	int result;
	char *errmsg = 0;

	result = sqlite3_open("./test_database.db", &db);
	if(result != SQLITE_OK)
		return -1;

	result = sqlite3_exec(db, "create table MyTable_1(ID integer primary key autoincrement, name nvarchar(32))", NULL, NULL, errmsg);
	if(result != SQLITE_OK )
		printf("创建表失败，错误码:%d，错误原因:%s/n", result, errmsg );

	//插入一些记录

	result = sqlite3_exec(db, "insert into MyTable_1(name) values ('走路' )", 0, 0, errmsg);
	if(result != SQLITE_OK )
		printf("插入记录失败，错误码:%d，错误原因:%s/n", result, errmsg);

	sqlite3_close(db);

	return 0;

}
#endif
