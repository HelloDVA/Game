#pragma once

#include <mysql/mysql.h>

#include <unordered_map>
#include <vector>
#include <string>

class Database {
	
	public:
		/* Database(std::string &host, std::string &user, std::string &password, std::string &dbname, int poolsize); */
		Database(MYSQL *conn) : conn_(conn){}
		~Database() {}
		using Row = std::unordered_map<std::string, std::string>;
		using Result = std::vector<Row>;
		Result Query(std::string &query);
		Row FetchRow(MYSQL_RES *result, MYSQL_ROW row);
		void Close();
	
	private:
		MYSQL *conn_ = nullptr;
};
