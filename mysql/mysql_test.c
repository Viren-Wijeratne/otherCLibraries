#include <stdio.h>
#include <stdlib.h>
#include <mariadb/mysql.h>

int main(void){

    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;

    conn = mysql_init(NULL);

    if(conn == NULL){
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if(mysql_real_connect(conn, "localhost", "root", "2409", "mydb", 0, NULL, 0) == NULL){
        fprintf(stderr, "Connection failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    printf("Connected to MySQL successfully!\n");

/*
    //Quering for tables within the database
    if(mysql_query(conn, "SHOW TABLES") != 0){
        fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }
*/

/*
    //Add a row to a table; employees
    if(mysql_query(conn, "INSERT INTO employees(first_name, last_name, hourly_pay) VALUES ('Bob', 'Builder', 10.00)")  != 0){
        fprintf(stderr, "Insert failed: %s\n", mysql_error(conn));
    }else{
        printf("Rows inserted: %llu\n", mysql_affected_rows(conn));
    }
*/

/*
    //Delete a row from a table; employees
    if(mysql_query(conn, "DELETE from employees WHERE first_name = 'Bob'")  != 0){
        fprintf(stderr, "Delete failed: %s\n", mysql_error(conn));
    }else{
        printf("Rows deleted: %llu\n", mysql_affected_rows(conn));
    }
*/

/*
    //Update a table with already existing data
    if(mysql_query(conn, "UPDATE employees SET hourly_pay = 12.50 WHERE employee_id = 6")  != 0){
        fprintf(stderr, "Update failed: %s\n", mysql_error(conn));
    }else{
        printf("Rows updated: %llu\n", mysql_affected_rows(conn));
    }
*/

    //Save any output comming from the database into result as an object
    result = mysql_store_result(conn);

    if(result == NULL){
        fprintf(stderr, "Could not retrieve results: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    while((row = mysql_fetch_row(result)) != NULL){
        printf("Table: %s\n", row[0]);
    }

    mysql_free_result(result);
    mysql_close(conn);

    return 0;
}

/* 
Compile using:
gcc -Wall -Wextra -o mysql_test mysql_test.c \
    $(mariadb_config --include) \
    $(mariadb_config --libs)

or

as the proper header #include <mariadb/mysql.h> was added we can use the shorten compilation command as:
gcc -Wall -Wextra -o mysql_test mysql_test.c -lmariadb

Functions and there usage:
    MYSQL *conn             //Holds the connection state.
    mysql_init()            //Allocates and initializes a connection handle.
    mysql_real_connect()    //Connects and authenticates.
    mysql_query()           //Sends the SQL query to the server.
    mysql_store_result()    //Retrieves the result set into client memory.
    mysql_fetch_row()       //Reads the returned rows one at a time.
    mysql_free_result()     //Releases the result set.
    mysql_close()           //Closes the database connection.
*/
