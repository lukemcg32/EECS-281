// Project Identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98
#include <iostream>
#include <getopt.h>
#include "bank.h"

void printHelp() {
    std::cout << "Usage: ./bank [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h           Show help message\n";
    std::cout << "  --file, -f <file>    Specify registration file\n";
    std::cout << "  --verbose, -v        Enable verbose output\n";
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    // Command-line argument parsing
    std::string regFilename;
    bool verboseMode = false;
    bool fileMode = false;
    int choice;
    int option_index = 0;

    struct option long_opts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"verbose", no_argument, nullptr, 'v'},
        {"file", required_argument, nullptr, 'f'},
        {0, 0, 0, 0} // Terminate the array
    };

    while ((choice = getopt_long(argc, argv, "hf:v", long_opts, &option_index)) != -1) {
        switch (choice) {
            case 'h':
                // Help message
                printHelp();
                exit(0);

            case 'f':
                regFilename = optarg;
                fileMode = true;
                break;

            case 'v':
                verboseMode = true;
                break;
        }
    }

    // Instantiate Bank object
    Bank bank(verboseMode);

    if (!fileMode) {
        std::cerr << "Error: Registration filename not specified.\n";
        exit(1);
    }

    // Read registration file
    bank.readRegistrationFile(regFilename);

    bank.processCommands();
    bank.processQueries();

    return 0;
}
