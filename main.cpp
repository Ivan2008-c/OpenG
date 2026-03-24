#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <iomanip>

namespace fs = std::filesystem;

enum class UserRole { ROOT = 0, USER = 1, HUSER = 2 };
enum class Edition { HOME = 0, PRO = 1, PROGRAM = 2 };

struct User { 
    std::string name; 
    std::string passHash; 
    UserRole role; 
};

class OpenGUnix {
private:
    std::string hostname;
    Edition sysEdition;
    int langID = 1;
    User* currentSession = nullptr;
    std::vector<User> users;

    const std::string rootDir = "openg_root";
    const std::string cfgFile = "openg_root/etc/system.cfg";
    const std::string usrFile = "openg_root/etc/passwd.dat";

    std::string encrypt(std::string p) {
        for(char &c : p) c = (c + 5); 
        return p;
    }

    void saveSystem() {
        fs::create_directories(rootDir + "/etc");
        std::ofstream f(cfgFile);
        f << hostname << "\n" << static_cast<int>(sysEdition) << "\n" << langID << "\n";
        f.close();

        std::ofstream u(usrFile);
        for(const auto& usr : users) {
            u << usr.name << " " << usr.passHash << " " << static_cast<int>(usr.role) << "\n";
        }
        u.close();
    }

    void runInstaller() {
        std::system("cls");
        std::cout << "--- OpenG UNIX INSTALLER ---\n\n";
        std::cout << "Set Hostname: "; std::cin >> hostname;
        std::cout << "Set ROOT Password (for Yara): "; 
        std::string rp; std::cin >> rp;
        std::cout << "Select Edition (0:Home, 1:Pro, 2:Prog): ";
        int ed; std::cin >> ed;
        sysEdition = static_cast<Edition>(ed);

        fs::create_directories(rootDir + "/bin");
        fs::create_directories(rootDir + "/home/user");

        users.clear();
        users.push_back({"root", encrypt(rp), UserRole::ROOT});
        users.push_back({"user", "", UserRole::USER}); // Обычный юзер без пароля
        saveSystem();

        std::cout << "\nSUCCESS. RESTARTING...\n";
        std::system("pause");
        std::exit(0);
    }

    void loadSystem() {
        if (!fs::exists(cfgFile)) runInstaller();
        
        std::ifstream f(cfgFile);
        int ed;
        f >> hostname >> ed >> langID;
        sysEdition = static_cast<Edition>(ed);
        f.close();

        std::ifstream u(usrFile);
        std::string un, ph; int ur;
        users.clear();
        while(u >> un >> ph >> ur) users.push_back({un, ph, static_cast<UserRole>(ur)});
        u.close();

        // По умолчанию заходим как user
        for(auto &u : users) if(u.name == "user") currentSession = &u;
        if(!currentSession) currentSession = &users[0];
    }

    bool askYaraPass() {
        std::string pass;
        std::cout << "[Yara] Password for root: ";
        std::cin >> pass;
        std::string hashed = encrypt(pass);
        
        for(auto &u : users) {
            if(u.name == "root" && u.passHash == hashed) return true;
        }
        return false;
    }

public:
    OpenGUnix() { loadSystem(); }

    void run() {
        std::system("cls");
        std::cout << "OpenG Unix Kernel v5.0\nType 'yara [command]' for admin tasks.\n\n";
        std::string line;

        while (true) {
            std::cout << "[" << currentSession->name << "@" << hostname << "]:/# ";
            if (!std::getline(std::cin, line) || line == "exit") break;
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string cmd; ss >> cmd;

            if (cmd == "yara") {
                if (askYaraPass()) {
                    std::string sub; ss >> sub;
                    std::cout << "[Yara] Executing " << sub << " as ROOT...\n";
                    // Здесь логика команд Yara
                } else {
                    std::cout << "Yara: Access denied (Incorrect password).\n";
                }
                std::cin.ignore(); // Очистка после cin в askYaraPass
            } 
            else if (cmd == "neofetch") {
                std::cout << "OS: OpenG Unix\nUser: " << currentSession->name << "\nHost: " << hostname << "\n";
            }
            else if (cmd == "clear") {
                std::system("cls");
            }
            else {
                std::cout << cmd << ": Command not found\n";
            }
        }
    }
};

int main() {
    OpenGUnix kernel;
    kernel.run();
    return 0;
}