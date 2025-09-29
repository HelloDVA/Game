#include <iostream>
#include <mysql/mysql.h>
#include "database.h"
#include "connectionpool.h"

Database::Result Database::Query(std::string &query){
	Database::Result rows_result;

	if(mysql_query(conn_, query.c_str())){
		std::cout << "query failed\n";
		return rows_result;
	} 

    MYSQL_ROW mysql_row;
	MYSQL_RES *result = mysql_use_result(conn_);

	if(result){
		while((mysql_row = mysql_fetch_row(result)) != NULL){
			Database::Row row = FetchRow(result, mysql_row);
			rows_result.push_back(row);
		}
	}
	mysql_free_result(result);
	return rows_result;
}

Database::Row Database::FetchRow(MYSQL_RES* result, MYSQL_ROW mysql_row) {
    Row rows;
    unsigned long* lengths = mysql_fetch_lengths(result);
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    int num_fields = mysql_num_fields(result);
    
    for (int i = 0; i < num_fields; i++) {
        std::string fieldName(fields[i].name);
        std::string value;
        if (mysql_row[i]) {
            value.assign(mysql_row[i], lengths[i]);
        }
        rows[fieldName] = value;
    }
    return rows;
}

void Database::Close(){
    ConnectionPool::getinstance().ReleaseConnection(conn_);
}
