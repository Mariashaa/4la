#include <sqlite3.h>
#include <stdio.h>
#include "lab03_sqlite.h"
int kath_kontr[2];
int callback(void *, int, char **, char **);
int callback2(void *, int, char **, char **);
int callback4(void *, int, char **, char **);
int get_kontrol_callback(void *, int , char **, char **);
int get_kathedra_callback(void *, int , char **, char **);
int i=0;
int get_id_callback(void *data, int argc, char **argv, char **col_names)
{
    // GtkComboBox *lecturer_box = (GtkComboBox *)data;

    if (argc > 0)
    {
        // for (int i = 0; i < 1; i++)
        // {
            // if (i % 2 == 0)
            // {
                //sscanf(argv[i], "%d", &kath_kontr[i]);
                kath_kontr[i] = atoi(argv[0]);
                i++;
                //kath_kontr[1] = atoi(argv[1]);
                fprintf(stderr, "%d - %s\n", 0, argv[0]);
                //fprintf(stderr, "%d - %s\n", 1, argv[3]);
            // }
        //}
        return 0;
    }

    return 1;
}



int get_kathedra_and_kontroll(const char *kathid, const char *kontrid)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql_comp_temp = "SELECT kathedra.id FROM kathedra WHERE kathedra.name = '%s';\
                SELECT kontroltype.id FROM kontroltype WHERE kontroltype.name = '%s';";
    char sql_comp[512];
    sprintf(sql_comp, sql_comp_temp, kathid, kontrid);
    rc = sqlite3_exec(db, sql_comp, get_id_callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to get indexes\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    return 0;
}

void sqlite_get_data()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT kathedra.id, kathedra.name, disciple.id, disciple.name, \
                    disciple.lections, disciple.practics, disciple.labs, kontroltype.id, \
                    kontroltype.name FROM disciple \
                    INNER JOIN kathedra ON disciple.kath_id = kathedra.id \
                    INNER JOIN kontroltype ON disciple.kontr_id = kontroltype.id;";
    rc = sqlite3_exec(db, sql, callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}


void sqlite_get_kathedra_data()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT id, name FROM kathedra;";
    rc = sqlite3_exec(db, sql, callback2, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_get_kathedra_combo()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT id, name FROM kathedra;";
    rc = sqlite3_exec(db, sql, get_kathedra_callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_get_kontr_data()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT id, name FROM kontroltype;";
    rc = sqlite3_exec(db, sql, callback4, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_get_kontr_combo()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = "SELECT id, name FROM kontroltype;";
    rc = sqlite3_exec(db, sql, get_kontrol_callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_kath_update(int kathid, char *kathname)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "UPDATE kathedra\
                              SET \
                              name = '%s' \
                          WHERE \
                              id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kathname, kathid);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to update data in kathedra\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_disc_update(int discid, const char *discname, const char *lect, const char *pract, const char *lab)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "UPDATE disciple SET name = '%s', lections = '%s', practics = '%s', labs = '%s' WHERE id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, discname, lect, pract, lab, discid);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to update data in disciple\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_kontr_update(int kontrid, char *kontrname)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "UPDATE kontroltype\
                              SET \
                              name = '%s' \
                          WHERE \
                              id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kontrname, kontrid);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to update data in kontroltype\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_kath_insert(const char *kathname)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "INSERT INTO kathedra(name) VALUES('%s');";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kathname);
    printf("%s\n", sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to insert data in kathedra\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);

}

void sqlite_disc_insert(const char *discname, const char *lect, const char *pract, const char *lab, const char *kathid, const char *kontrid)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    if (get_kathedra_and_kontroll(kathid, kontrid) == 1)
    {
        return;
    }
    i = 0;
    fprintf(stderr, "kath_id = %d\nkontr_id = %d\n", kath_kontr[0], kath_kontr[1]);
    char *sql_comp_temp = "INSERT INTO disciple(name, lections, practics, labs, kath_id, kontr_id) VALUES('%s', '%s', '%s', '%s', %d, %d);";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, discname, lect, pract, lab, kath_kontr[0], kath_kontr[1]);
    fprintf(stderr, sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to update data in disciple\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_kontr_insert(const char *kontrname)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "INSERT INTO kontroltype(name) VALUES('%s');";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kontrname);
    printf("%s\n", sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to insert data in kontroltype\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}


void sqlite_kath_delete(int kathid)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "DELETE FROM kathedra WHERE id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kathid);
    printf("%s\n", sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to delete data from kathedra\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_disc_delete(int discid)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "DELETE FROM disciple WHERE id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, discid);
    printf("%s\n", sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to delete data from disciple\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_kontr_delete(int kontrid)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql_comp_temp = "DELETE FROM kontroltype WHERE id = %d;";
    char sql_comp[255];
    sprintf(sql_comp, sql_comp_temp, kontrid);
    printf("%s\n", sql_comp);
    rc = sqlite3_exec(db, sql_comp, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to delete data from kontroltype\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

