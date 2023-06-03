#ifndef LAB03_SQLITE_H
#define LAB03_SQLITE_H

#define DB_FILE "mydb.db"
void sqlite_get_data();
void sqlite_get_kathedra_data();
void sqlite_get_kontr_data();
void sqlite_get_kathedra_combo();
void sqlite_get_kontr_combo();
void sqlite_kath_update(int kathid, char *kathname);
void sqlite_disc_update(int discid, const char *discname, const char *lect, const char *pract, const char *lab);
void sqlite_kontr_update(int kontrid, char *kontrname);
void sqlite_kath_insert(const char *kathname);
void sqlite_disc_insert(const char *discname, const char *lect, const char *pract, const char *lab, const char *kathid, const char *kontrid);
void sqlite_kontr_insert(const char *kontrname);
void sqlite_kath_delete(int kathid);
void sqlite_disc_delete(int discid);
void sqlite_kontr_delete(int kontrid);
#endif
