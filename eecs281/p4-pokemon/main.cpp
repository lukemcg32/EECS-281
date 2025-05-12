// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <iomanip>
#include <iostream>
#include <string>
#include <getopt.h>
#include "PokemonTSP.h"
#include "PokemonMST.h"



void print_help() {
    std::cout << "Usage: ./poke [options] < inputFile.txt\n"
              << "Options:\n"
              << "  -m, --mode {MST|FASTTSP|OPTTSP}  Set the program mode\n"
              << "  -h, --help                       Print this help message\n";
}




int main (int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false); // speed
    cout << std::setprecision(2); 
    cout << std::fixed; // for 2 decimal places

    const char* const short_opts = "m:h";
    const option long_opts[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    std::string mode;
    int opt;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'm':
                mode = optarg;
                if (mode != "MST" && mode != "FASTTSP" && mode != "OPTTSP") {
                    std::cerr << "Invalid mode. Use MST, FASTTSP, or OPTTSP.\n";
                    return 1;
                }
                break;
            case 'h':
                print_help();
                return 0;
            case '?':
            default:
                std::cerr << "Unknown option. Use -h for help.\n";
                return 1;
        }
    }

    if (mode.empty()) {
        std::cerr << "Mode is required. Use -h for help.\n";
        return 1;
    }

    uint32_t nV;
    cin >> nV;

    if (mode == "MST") {
        PokemonMST mst(nV);
        mst.readInput();
    } else {
        PokemonTSP tsp(mode, nV);
        tsp.readInput();
    }

    return 0;
}