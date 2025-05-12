// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <string>
#include <vector>
#include <iostream>
#include "Station.hpp"


// Modifies: SpaceStation/levels
// Effects: add the locations of objects/locations into the levels member of SpaceStation Class
// TODO: Implement
void read_in(SearchStrategy &station) { // confirm that this should be pass by ref
    char type = '\0';
    uint32_t num_levs = 0;
    uint32_t num_rows = 0;

    char aux = '\0'; // initialized as empty
    string junk = "";

    // these are for indexing...
    uint32_t lev = 0;
    uint32_t row = 0;
    uint32_t col = 0;
    
    // read in prelim info
    cin >> type;
    cin >> num_levs;
    cin >> num_rows;


    // if (type == 'M') input = "M\n" + to_string(num_levs) + "\n" + to_string(num_rows) + "\n";
    // else input = "L\n" + to_string(num_levs) + "\n" + to_string(num_rows) + "\n";
    

    // resizes my station and search algorithm vectors
    station.update_station(num_levs, num_rows);


    // read in if list data
    if (type == 'L') {
        char c = '\0';

        while (cin >> aux) {
            if (aux == '/') {
                getline(cin, junk);
                continue; // make sure this works
            }
            if (aux == '(' ) {
                // aux for commas and final parenthesis (used as junk)
                cin >> lev >> aux >> row >> aux >> col >> aux >> c;
                if (lev > num_levs || row > num_rows || col > num_rows) {
                    cerr << "bad location\n";
                    exit(1);
                }
                if (!station.is_validCharacter(c)) {
                    cerr << "bad character in input file\n";
                    exit(1);
                }

                getline(cin, junk); // last parenthesis for failsafe

                // set the tile in the station - will also handle start/finish
                station.setTile(lev, row, col, c);
            } 
        }
    } // if 'L'

    // read in if map data
    if (type == 'M') {
        uint32_t i = 0;


        while (cin >> aux) {
            if (aux == '/') {
                getline(cin, junk);

                continue;
            }

            col = i % station.get_size();
            row = i / station.get_size();
            if (i == (station.get_size() * station.get_size())) {
                lev++;
                i = 0;
                row = 0;
            }
            if (lev > station.get_num_levs() || row > station.get_size() 
                                 || col > station.get_size()) {
                cerr << "bad location\n";
                exit(1);
            }
            if (!station.is_validCharacter(aux)) {
                cerr << "bad character in input file\n";
                exit(1);
            }

            station.setTile(lev, row, col, aux);
            if (aux == 'S') station.set_start(lev, row, col);
            if (aux == 'H') station.set_goal(lev, row, col);
            i++; 
        }
    } // if 'M'

} // read()