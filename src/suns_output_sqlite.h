
int suns_output_sqlite_open(char *path, sqlite3 **db, const char **err);
int suns_output_sqlite_close(sqlite3 *db, const char **err);
int suns_output_sqlite_model_list(sqlite3 *db,
                                  list_t *models,
                                  const char **err);
int suns_output_sqlite_model_did(sqlite3 *db,
                                 suns_model_did_t *did,
                                 const char **err);
int suns_output_sqlite_init_db(sqlite3 *db, const char **err);
int suns_output_sqlite_device(sqlite3 *db,
                              suns_device_t *d,
                              const char **err);
int suns_output_sqlite_device_row(sqlite3 *db,
                                  suns_device_t *d,
                                  sqlite3_int64 *rowid,
                                  const char **err);
int suns_output_sqlite_dataset(sqlite3 *db,
                               suns_dataset_t *ds,
                               sqlite3_int64 device_rowid,
                               const char **err);
int suns_output_sqlite_dataset_row(sqlite3 *db,
                                   suns_dataset_t *ds,
                                   sqlite3_int64 device_rowid,
                                   sqlite3_int64 *rowid,
                                   const char **err);
int suns_output_sqlite_value(sqlite3 *db,
                             suns_value_t *v,
                             int did,
                             sqlite3_int64 dataset_rowid,
                             const char **err);
