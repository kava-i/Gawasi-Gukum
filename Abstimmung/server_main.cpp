#include <iostream>
#include <signal.h>
#include "../DataExtraction/httplib.h"
#include "../DataExtraction/json.hpp"
#include <string>
#include <fstream>

using namespace httplib;
Server srv;

void sig_handler(int)
{
    //Just stop the server when systemd wants to exit us.
    srv.stop();
}

std::mutex m_lck;
bool isKeyValid(std::string k, std::vector<std::string> &usedKeys)
{
    for(auto &it : usedKeys)
	if(k==it)
	    return false;
    return true;

}

void submitVote(const Request &req, Response &resp,std::vector<std::string> &usedKeys)
{   
    std::lock_guard g(m_lck);
    try
    {
	std::string k = req.get_param_value("key",0);
	if(!isKeyValid(k,usedKeys))
	    throw 0;
	usedKeys.push_back(k);
	resp.status = 200;
    }
    catch(...)
    {
	resp.status = 403;
    }
}

void validKey(const Request &req, Response &resp,const std::vector<std::string> &usedKeys)
{
    std::lock_guard g(m_lck);
    try
    {
	std::string k = req.get_param_value("key",0);
	if(!isKeyValid(k,usedKeys))
	    throw 0;
	resp.status = 200;
    }
    catch(...)
    {
	resp.status = 403;
    }
}


void getPossibleVoters(const Request &req, Response &resp)
{
    std::vector<std::string> people = {"Albertin, Johanna","Calwore, Ray","Ende, Marc","Fuchsstein, Elias Lou","Gardener, Charlie","Gaylay, Erkel","Grimm, Marja","Grimm, Tom","Knaub, Sergej","Mati, Plinio","Mati, Sarah","Reh, Dascha","Rose, Elaine","Saint-Just, Maxine Pierre","Schwarz, Eva","Schwarz, Eva","Albertin, David Maria","Grass, Shari","Stiegler, Eva","Drake"};
    nlohmann::json js_people;
    js_people = people;
    resp.status = 200;
    resp.set_content(js_people.dump(),"application/json");
}

int main()
{
    signal(SIGTERM, sig_handler);
    std::vector<std::string> usedKeys;
	
    srv.Get("/api/possibleVoters",&getPossibleVoters);
    srv.Get("/api/isKeyValid",[&](const Request &req, Response &resp){validKey(req,resp,usedKeys);});
    srv.Get("/api/submitVote",[&](const Request &req, Response &resp){submitVote(req,resp,usedKeys);});
    srv.listen("localhost",9710);
    return 0;
}
