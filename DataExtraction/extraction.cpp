#include <iostream>
#include <fstream>
#include <list>
#include <filesystem>
#include "fuzzy.hpp"
#include "func.hpp"
#include "json.hpp"
#include "easy-encryption/encrypt.h"
namespace fs=std::filesystem;

void extractData()
{
    // ***** LOADING FILE ***** //
    std::cout << "Verdächtige Person (Name der verschlüsselten Datei): ";
    std::string sInput;
    getline(std::cin, sInput);

    std::ifstream read("data/"+sInput);
    if(!read)
    {
        std::cout << "Ausgewählte Person in unserem Datensatz nicht vorhanden.\n";
        return;
    }

    nlohmann::json j_encryptedData;
    read >> j_encryptedData;

    std::vector<std::string> listWords = j_encryptedData["words"];
    std::vector<std::string> listInfos = j_encryptedData["infos"];


    // ***** ENTER INFORMATION ***** //
    std::cout << "Informationen zu der verdächtigten person eingeben \n(Dies können einzelne Wörter oder kurze Phrasen sein. \n10 Eingaben möglich. Bestätigen mit \"k\".): \n";

    std::list<std::string> listInputs;
    for(int i=0; i<10; i++) {
        std::cout << i << ": ";
        getline(std::cin, sInput);
        if(sInput == "k") 
            break;
        sInput = func::convertStr(sInput);
        listInputs.push_back(func::returnToLower(sInput));
    }

    // ***** EXTRACT DATA ***** //
    std::string key = "GAWASI";
    unsigned int counter=0;
    for(auto it : listWords) {
        it = func::convertStr(it);
        it = decrypt(it, key);
        for(auto yt : listInputs)
        {
            if(yt.length() <= 1) continue;
            if(fuzzy::fuzzy_cmp(it, yt) <= 0.2 || fuzzy::fuzzy_cmp(yt, it) <= 0.2) {
                counter++; 
                break;
            }
        }
    }

    // ***** OUTPUT FOUND DATA ***** //
    std::cout << counter << " Übereinstimmungen.\n\n\n";
    if(counter >= 2) std::cout << decrypt(listInfos[0], key) << "\n\n";
    if(counter >= 4) std::cout << decrypt(listInfos[1], key) << "\n\n";
    if(counter >= 6) std::cout << decrypt(listInfos[2], key) << "\n\n";
    
}

int main()
{
    for(;;)
    {
        std::cout << "**** Ministerium für Justiz ****\n";
        std::cout << "[D]aten einer verdächtigen Person extrahieren.\n";
        std::cout << "[H]ilfe und Informationen.\n";
        std::cout << "[P]rogramm beenden.\n";
        std::cout << ">";
        std::string sInput;
        getline(std::cin, sInput);

        if(sInput== "D")
            extractData();            

        else if(sInput == "H")
            std::cout << "HELP!!!\n"; 

        else if(sInput == "P")
            break; 

        else
            std::cout << "Falsche Eingabe!\n";
    }

    //Leave programm
    std::cout << "\n\nProgramm wird beendet...\n";
}

