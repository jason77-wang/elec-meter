#ifndef foodbsqlitehfoo
#define foodbsqlitehfoo

int db_glb_init(const char* dbname);
void db_insert_meters(struct ele_meter *pmeter);

#endif
