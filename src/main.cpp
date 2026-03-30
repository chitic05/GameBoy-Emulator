#include "Emulator.h"
#include <iostream>
#include <cstring>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if(!fs::is_directory("roms")){
        fs::create_directory("roms");
        std::cout << "Roms directory created. Please copy your roms there!\n";
        return 0;
    }

    std::vector<std::string> romList;

    for (const auto& entry : fs::directory_iterator("roms")) {
        if (entry.is_regular_file() && entry.path().extension() == ".ch8") {
            
            romList.push_back(entry.path().string());
            
            
        }
    }

    if(romList.empty()){
        std::cout << "Roms folder is empty!\n";
        return 0;
    }

    for(int i = 0; i<romList.size(); i++){
        std::cout << i+1 << '.' << romList[i].substr(5,romList[i].length()) << '\n' << std::flush;
    }
    int option;
    std::cout << "Enter the index of the rom you want to use!\n";
    std::cin >> option;

    if(option > romList.size() || option<1){
        std::cout << "This rom does not exist!\n ";
        return 0;
    }
    
    Emulator emulator;
    emulator.loadRom(romList[option-1]);
    
    try{
        emulator.run();
    }catch(std::exception& e){
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    
    std::cout << "\n=== Emulation completed: "<< std::endl;
    
    return 0;
}