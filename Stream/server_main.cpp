#include <iostream>
#include <signal.h>
#include "../DataExtraction/httplib.h"
#include "../DataExtraction/json.hpp"
#include <string>
#include <fstream>

using namespace httplib;
Server srv;
std::map<std::string,int> mapVote;

void sig_handler(int)
{
    //Just stop the server when systemd wants to exit us.
    srv.stop();
}

int main()
{
    signal(SIGTERM, sig_handler);
    std::vector<std::string> usedKeys;
	
    srv.Get("/api/audio/sync_server",[&](const Request &req, Response &resp){resp.status=200;});
    srv.listen("localhost",9711);
    return 0;
}
