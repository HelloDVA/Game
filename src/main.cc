#include"server/server.h"

int main()
{
	Server *server = new Server();
	server->Start();
	return 0;
}
