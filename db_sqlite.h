#ifndef foodbsqlitehfoo
#define foodbsqlitehfoo
#include <sqlite3.h>

sqlite3 * db_glb_init(const char* dbname);
void db_insert_meters(sqlite3 *db, struct ele_meter *pmeter);
void db_glb_close(sqlite3 *db);

#endif
