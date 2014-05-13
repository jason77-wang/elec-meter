#include <stdlib.h>
#include <stdio.h>

#include <sqlite3.h>

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
