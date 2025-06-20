#include <iostream>
#include <string>
#include "minigit.cpp"

int main() {
    std::string command;

    std::cout << "Welcome to MiniGit!\n";
    std::cout << "Available commands:\n";
    std::cout << "  init               → create refs folder only\n";
    std::cout << "  add <filename>     → stage a file (file must exist)\n";
    std::cout << "  commit -m \"msg\"    → commit changes to current branch\n";
    std::cout << "  log                → show commits for current branch\n";
    std::cout << "  branch <name>      → create a new branch\n";
    std::cout << "  checkout <name>    → switch to a different branch\n";
    std::cout << "  exit               → quit the MiniGit system\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "init") {
            init();  // Now only creates .minigit/refs/
        }
        else if (command.rfind("add ", 0) == 0) {
            std::string filename = command.substr(4);
            addFile(filename);
        }
        else if (command.rfind("commit -m ", 0) == 0) {
            std::string message = command.substr(10);
            if (!message.empty() && message.front() == '"' && message.back() == '"') {
                message = message.substr(1, message.length() - 2);
            }
            commit(message);
        }
        else if (command == "log") {
            log();
        }
        else if (command.rfind("branch ", 0) == 0) {
            std::string name = command.substr(7);
            branch(name);
        }
        else if (command.rfind("checkout ", 0) == 0) {
            std::string name = command.substr(9);
            checkout(name);
        }
        else if (command == "exit") {
            std::cout << "Exiting MiniGit.\n";
            break;
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }

    return 0;
}
