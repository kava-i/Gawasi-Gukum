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

std::mutex m_lck;
bool isKeyValid(std::string k, const std::vector<std::string> &usedKeys)
{
    for(auto &it : usedKeys)
	if(k==it)
	    return false;
    std::vector<std::string> validKeys = {"TpCwWQWFbAouoGTTF29z","hi6eDnZ3utLSrDY9iBBr","qkm6jRyEF5rtRRY96tcd","ubd8pj5KuUv2veUd6x3k","MgcMohfNGLgYuMPgbUU3","iv9ZtiaHaEvMmxCoiEAo","f53cXruQF6JdzsofXnJu","tHDnd7BAe75JZZxWcVHA","z7w2Lmkzv77SjRRyautV","ryfcfV2Ta2KBPbVc4jSn","ctJdeNZwJwWxUBWeGDMR","ewomxBe6isgTEE9NwtKK","HBpfsokEZMcjhhhEYPTd","v84weChZJdDe6FEWJS87","gmfyq6f5qWcvpJDKefAr","PK3pK72keovpYErt6X7A","VswHLGUE7RfueMThmtpz","D2redXVfgd2gbvmZN7n5","HSNvQJFfc5QXpkZaUnLV","MkJHeovePPuTYPEVdoWe","FCVBwnhzxVhC7eaFH9VR","hMPN5vcyLPXomjb8ok7x","swxwiMSUoYF7QTbasC6f","8LF5JVhKg4qSAMN9Shdi"};
    for(auto &it : validKeys)
	if(k==it)
	    return true;
    return false;
}

void submitVote(const Request &req, Response &resp,std::vector<std::string> &usedKeys)
{   
    std::lock_guard g(m_lck);
    try
    {
	std::cout<<"SUBMIT VOTE CALLED!"<<std::endl;
	std::string k = req.get_param_value("key",0);
	std::string name = req.get_param_value("name",0);
	if(!isKeyValid(k,usedKeys))
	    throw 0;
	usedKeys.push_back(k);
	if(mapVote.find(name)!=mapVote.end())
		mapVote[name]++;
	else
		mapVote[name] = 1;
	
	std::cout<<"\n\n\n~~~~~~ VOTING RESULTS ~~~~~~"<<std::endl;
	for(auto &it : mapVote)
	{
		std::cout<<it.first<<" has "<<it.second<<" votes!"<<std::endl;
	}
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
    srv.Post("/api/submitVote",[&](const Request &req, Response &resp){submitVote(req,resp,usedKeys);});
    srv.listen("localhost",9710);
    return 0;
}
