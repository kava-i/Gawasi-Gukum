#include <iostream>
#include <fstream>
#include <list>
#include <filesystem>
#include "func.hpp"
#include "json.hpp"
#include "easy-encryption/encrypt.h"
namespace fs=std::filesystem;

int main()
{
    std::string key = "GAWASI";
    fs::path p = "clear";
    for(auto&p : fs::directory_iterator("clear")) {
        nlohmann::json j_data;
        std::string sPath = p.path();
        std::cout << sPath << "\n";
        std::ifstream read(sPath);
        read >> j_data;
        read.close();

        std::vector<std::string>listWords = j_data["words"];
        std::vector<std::string>listInfos = j_data["infos"];

        for(auto& it : listWords) {
            it = func::convertStr(it); 
            it = encrypt(it, key); 
        }
        for(auto& it : listInfos) {
            it = func::convertStr(it);
            it = encrypt(it, key); 
        }

        nlohmann::json j_encryptedData;
        j_encryptedData["words"] = listWords;
        j_encryptedData["infos"] = listInfos;
        std::string filename = p.path().filename();
        std::ofstream write("data/"+filename);
        write << j_encryptedData;
        write.close();
    }

        
}
        
