#include <iostream>
#include <signal.h>
#include "httplib.h"
#include "func.hpp"
#include "easy-encryption/encrypt.h"
#include "fuzzy.hpp"
#include "json.hpp"
#include <string>
#include <fstream>
#include <filesystem>

using namespace httplib;
Server srv;

void sig_handler(int)
{
    //Just stop the server when systemd wants to exit us.
    srv.stop();
}

void own_split(const std::string &pill, char c, std::vector<std::string> &vec)
{	    auto start = 0;
    std::string tmp;
    
    while(true)
    {
	auto pos = pill.find(c,start);
	if(pos==std::string::npos)
	{
	    tmp = pill.substr(start);
	}
	else
	{
	    tmp = pill.substr(start,pos);
	}
	if(tmp!="")
	    vec.push_back(std::move(tmp));
	if(pos==std::string::npos)
	    break;
	start = pos+1;
    }
}

void HasSuspect(const Request &req, Response &resp)
{
    try
    {
	std::string name = req.get_param_value("name");
	name+=".json";

	std::ifstream read("data/"+name);
	if(!read)
	    throw 0;

	resp.status = 200;
    }
    catch(...)
    {
	resp.status = 404;
    }
}
void QueryInformation(const Request &req, Response &resp)
{
    try
    {
	std::string name = req.get_param_value("name");
	name+=".json";

	std::vector<std::string> keywords;
	std::string debugpill = req.get_param_value("keywords",0);
	own_split(debugpill,';',keywords);
	for(auto &it : keywords)
	    it = func::convertStr(it);
	std::ifstream read("data/"+name);
	if(!read)
	    throw 0;
	nlohmann::json encdata;
	read>>encdata;
	read.close();
	std::vector<std::string> listWords = encdata["words"];
	std::vector<std::string> listInfos = encdata["infos"];

	std::string key = "GAWASI";
	unsigned int counter=0;
	for(auto it : listWords) {
	    it = func::convertStr(it);
	    it = decrypt(it, key);
	    for(auto yt : keywords)
	    {
		if(yt.length() <= 1) continue;
		if(fuzzy::fuzzy_cmp(it, yt) <= 0.2 || fuzzy::fuzzy_cmp(yt, it) <= 0.2) {
		    counter++; 
		    break;
		}
	    }
	}
	std::cout << counter << " Übereinstimmungen.\n\n\n";
	nlohmann::json information;
	information["right_infos"] = counter;
	if(counter >= 2) information["info"].push_back(decrypt(listInfos[0], key));
	if(counter >= 4) information["info"].push_back(decrypt(listInfos[1], key));
	if(counter >= 6) information["info"].push_back(decrypt(listInfos[2], key));
	resp.status=200;
	resp.set_content(information.dump(),"application/json");
    }
    catch(...)
    {
	resp.status = 404;
	resp.set_content("Corrupted request received","text/plain");
    }
}

void getsugs(const Request &req, Response &resp, const std::vector<std::string> &filenames)
{
	try
	{
		nlohmann::json sugs;
		for(auto &it : filenames)
		{
			sugs.push_back(it);
		}
		resp.status = 200;
		resp.set_content(sugs.dump(),"application/json");
	}
	catch(...)
	{
		resp.status = 500;
		resp.set_content("{}","application/json");
	}
}

int main()
{
    signal(SIGTERM, sig_handler);
    std::vector<std::string> NameVector;

    for(auto &it : std::filesystem::directory_iterator("data"))
    {
	std::string filename = it.path().filename();
	std::string name = filename.substr(0,filename.find_last_of('.'));
	std::cout<<"Character name extracted: "<<name<<std::endl;
	NameVector.push_back(std::move(name));
    }

    srv.Get("/api/queryInformation", &QueryInformation);
    srv.Get("/api/hasSuspect",&HasSuspect);
    srv.Get("/api/suggestions",[&](const Request &req, Response &resp){getsugs(req,resp,NameVector);});
    srv.listen("localhost",9709);
    return 0;
}
