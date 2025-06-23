
#include<mysql/field_types.h>
#include<string>
#include<iostream>

#include <mysql/mysql.h>

#include"connectionpool.h"
#include "database.h"

int main()
{
	std::string host = "localhost";
	std::string user = "root";
	std::string password = "mysql123456";
	std::string dbname = "game";
	int poolsize = 10;

	int port = 3306;

	ConnectionPool::getinstance().init(host, user, password, dbname, poolsize);
	MYSQL* conn = ConnectionPool::getinstance().GetConnection();

	Database database(conn);

	std::string username = "user1";
	std::string userpwd;
	std::string query = "SELECT * FROM user WHERE username = '" + username + "'";
	
	Database::Result rows = database.Query(query);
	
	if(!rows.empty()){
		for(int i = 0; i < rows.size(); i ++){
			Database::Row row = rows[i];	
			std::cout << row["username"] << row["password"] << std::endl;
		}
	} else {
		ConnectionPool::getinstance().ReleaseConnection(conn);
	}

	/* std::string password = NULL; */
	/* std::string dbname = "user"; */
	/* int poolsize = 10; */
	/* ConnectionPool::getinstance().init(host, user, password, dbname, poolsize); */
	/* MYSQL* conn = ConnectionPool::getinstance().GetConnection(); */
	/* if(!conn){ */
	/* 	std::cerr << "failed to get the connection" << std::endl; */
	/* 	return 1; */
	/* } */
	
	/* MYSQL_STMT *stmt = mysql_stmt_init(conn); */
	/* if(!stmt){ */
	/* 	std::cerr << "failed to get the stmt" << std::endl; */
	/* 	return 1; */
	/* } */
	/* const char *query = "SELECT password FROM user WHERE username = ?"; */
	/* if(mysql_stmt_prepare(stmt, query, strlen(query))){ */
	/* 	mysql_stmt_close(stmt); */
	/* 	return 1; */
	/* } */

	/* std::string username = "user1"; */
	/* MYSQL_BIND bind; */
	/* memset(&bind, 0, sizeof(bind)); */
	/* bind.buffer_type = MYSQL_TYPE_VARCHAR; */
	/* bind.buffer = const_cast<char*>(username.c_str()); */
	/* bind.buffer_length = username.length(); */
 	/* if (mysql_stmt_bind_param(stmt, &bind)) { */
        /* mysql_stmt_close(stmt); */
        /* return 1; */
    /* } */
    
    /* if (mysql_stmt_execute(stmt)) { */
        /* mysql_stmt_close(stmt); */
        /* return 1; */
    /* } */	

	/* char user_pwd[10]; */
	/* MYSQL_BIND result[1]; */	
	/* memset(result, 0, sizeof(result)); */
	/* result[0].buffer_type = MYSQL_TYPE_VARCHAR; */
	/* result[0].buffer = (void*)&user_pwd; */
	/* result[0].buffer_length = sizeof(user_pwd); */

	/* if(mysql_stmt_bind_result(stmt, result)){ */
	/* 	mysql_stmt_close(stmt); */
	/* 	return 1; */
	/* } */

	/* while(!mysql_stmt_fetch(stmt)){ */
		
	/* } */

}
