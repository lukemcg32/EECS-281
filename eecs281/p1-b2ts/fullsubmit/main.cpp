// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <string>
#include <iostream>
#include <getopt.h>
#include "io_handlers.hpp"

// ** test files must not exceed 10 levels or 8x8


// for help selection
void print_usage() { 
    cout << "Usage: ./ship [OPTIONS] < inputfile > outputfile\n"
         << "Options:\n"
         << "  -s, --stack        Use stack (DFS)\n"
         << "  -q, --queue        Use queue (BFS)\n"
         << "  -o, --output M|L   Output format, M (map) or L (list)\n"
         << "  -h, --help         Display this help and exit\n";
} // print_usage()



int main(int argc, char *argv[]) {
    // necessary
    // will cause memory leaks with valgrind
    std::ios_base::sync_with_stdio(false);

    int gotopt;
    int option_index = 0;
    char mode = '\0';
    char output_format = 'M';  // Default output format MAP

    // idk if this should be a struct or not
    struct option longOpts[] = {{ "stack", no_argument, nullptr, 's'},
                                { "queue", no_argument, nullptr, 'q'},
                                { "output", required_argument, nullptr, 'o'},
                                { "help", no_argument, nullptr, 'h'}}; // long_opts[]

    while ((gotopt = getopt_long(argc, argv, "sqo:h", longOpts, &option_index)) != -1) {
        switch (gotopt) {
            case 's':
                if (mode == 'q') {
                    cerr << "Error: Both --stack and --queue specified.\n";
                    exit(1); // idk about exit...
                } // if
                mode = 's';
                break;

            case 'q':
                if (mode == 's') {
                    cerr << "Error: Both --stack and --queue specified.\n";
                    exit(1);
                } // if
                mode = 'q';
                break;

            case 'o':
                if (*optarg != 'M' && *optarg != 'L') {
                    cerr << "Error: Invalid output format specified (use 'M' or 'L').\n";
                    exit(1);
                } // if
                else if(*optarg == 'L') output_format = 'L';
                break;

            case 'h':
                print_usage();
                exit(0);
        } // switch
    } // while

    if (!mode) {
        cerr << "Error: Must specify one of --stack or --queue.\n";
        print_usage();
        exit(1);
    }

   SearchStrategy station(1, 1); 


    // read_in will resize and initialize everything
    read_in(station);

    if (output_format == 'M') {
        station.print_map(mode);
        return 0; 
    }
    else if (output_format == 'L') {
        station.print_list(mode);
        return 0; 
    }
    else {
        cout << "should never see this...\n";
        return EXIT_FAILURE;
    }
}
