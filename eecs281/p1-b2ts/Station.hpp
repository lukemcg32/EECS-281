// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#ifndef STATION_HPP
#define STATION_HPP
#pragma once
#include <string>
#include <deque>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;


struct location { 
    uint32_t lev;
    uint32_t row;
    uint32_t col;

    location(uint32_t l, uint32_t r, uint32_t c) 
            : lev(l), row(r), col(c) {}
};

// got rid of bool discovered - just check if it has a direction
struct place {
    char symbol = '.';
    char direction = '\0';
};


// main class
class SpaceStation {
public:
~SpaceStation() {} // dtor

SpaceStation(uint32_t numLevels, uint32_t size) : start(location(0,0,0)), goal(location(0,0,0)), 
        num_levels(numLevels), level_size(size) {} //SpaceStation

// TODO: make sure this works
void update_station(uint32_t lev, uint32_t size) { 
    num_levels = lev;
    level_size = size;

    levels.resize(lev, vector<vector<place>>(size, vector<place>(size)));
}


void setTile(uint32_t lev, uint32_t row, uint32_t col, char data) {
    if (is_valid(lev, row, col)) {
        if (data == 'S') {
            set_start(lev, row, col);
            setDirection(lev, row, col, 'S');
        } if (data == 'H') {
            set_goal(lev, row, col); 
            setDirection(lev, row, col, 'H');
        }
        levels[lev][row][col].symbol = data;
        // for debugging...
        // cout << "setTile() called with: " << lev << " " << row << " " << col << "\n";
    }
    else {
        // for debugging...
        // cout << "\n" << "Dimensions of current container: " << get_num_levs() << " " << get_size() << " " << get_size() << "\n";
        // cout << "Dimensions of input:             " << lev << " " << row << " " << col << "\n";

        cerr << "Invalid coordinates @ line 6o...setTile()";
        exit(1);
    }
} // setTile


char getTile(uint32_t lev, uint32_t row, uint32_t col) const { // linear time! O(n)
        // cout << "gT asking for: " << lev << " " << row << " " << col << "\n";
        validateCoordinates(lev, row, col);
        return levels[lev][row][col].symbol;
} // getTile

place get_location_tile(uint32_t lev, uint32_t row, uint32_t col) {
    place& temp = levels[lev][row][col];
    return temp;
}

void setDirection(uint32_t lev, uint32_t row, uint32_t col, char dir) {
    // cout << "sD asking for: " << lev << " " << row << " " << col << "\n";
    validateCoordinates(lev, row, col);
    levels[lev][row][col].direction = dir;
} //setLocation

char getDirection(uint32_t lev, uint32_t row, uint32_t col) const {
    // cout << "gD asking for: " << lev << " " << row << " " << col << "\n";
    validateCoordinates(lev, row, col);
    return levels[lev][row][col].direction;
} // getDirection

void set_start (uint32_t lev, uint32_t row, uint32_t col) {
    start.lev = lev;
    start.row = row;
    start.col = col;
} // set_start

void set_goal (uint32_t lev, uint32_t row, uint32_t col) {
    goal.lev = lev;
    goal.row = row;
    goal.col = col;
} // set_goal

uint32_t get_start_lev() {
    return start.lev;
}

uint32_t get_start_row() {
    return start.row;
}

uint32_t get_start_col() {
    return start.col;
}

uint32_t get_goal_lev() {
    return goal.lev;
}

uint32_t get_goal_row() {
    return goal.row;
}

uint32_t get_goal_col() {
    return goal.col;
}

uint32_t get_num_levs() {
    return num_levels;
}

uint32_t get_size() {
    return level_size;
}

bool is_valid(uint32_t lev, uint32_t row, uint32_t col) {
    // less than's because rows/levs initialized to 0 for first loco
    return (lev < num_levels && row < level_size && col < level_size);
}

bool is_validCharacter(char c) {
    return (c == 'S' || c == 'H' || c == 'E' || c == '.' || c == '#');
}


private:
    location start;
    location goal;
    vector<vector<vector<place>>> levels; // 3D vector respresnting each level
    uint32_t num_levels; // Number of levels in the space station
    uint32_t level_size; // Size of each level (assuming square levels)

    // uint32_t will never be less than 0
    void validateCoordinates(uint32_t level, uint32_t row, uint32_t col) const {
        if (level > num_levels || row > level_size || col > level_size) {
            cerr << "Coordinates are out of the space station bounds";
            exit(1);
        }
    }

};

// when you call, only call the SearchStrategy and it will automatically create an instance of SpaceStation
class SearchStrategy : public SpaceStation {

public:
    ~SearchStrategy() {}

    // contructor will initialize the visited vector to the size, but will need to resize after the read
    // resize call comes in the read function
    SearchStrategy(uint32_t numLevels, uint32_t size) : SpaceStation(numLevels, size) {}

    void print_map (char mode) {
	    bool path_found;


         // print this first because we need it regardless
        cout << "Start in level " << get_start_lev() << ", row " << get_start_row() << ", column " 
                                  << get_start_col() << "\n";

        if (mode == 's') {
            path_found = stack_findPath(); // using stack if mode == 's'
        }
        else if (mode == 'q') {
            path_found = queue_findPath(); // otherwise using queue
        }
        else {
            cerr << "should never see this @ 208";
            exit(1);
        }

        // uses same code as later because we haven't looped through and backtraced yet...
        if (!path_found) { // print input file to output if no path found
            for (uint32_t lev_loop = 0; lev_loop < get_num_levs(); lev_loop++) {
                cout << "//level " << lev_loop << "\n";
                for (uint32_t row_loop = 0; row_loop < get_size(); row_loop++) {
                    for (uint32_t col_loop = 0; col_loop < get_size(); col_loop++) {
                        cout << getTile(lev_loop, row_loop, col_loop);
                    }
                    cout << "\n"; // end line whenever rows are done printing??...yessir
                }
            } // for
            return;
        } // if no path
        
	    uint32_t lev = get_goal_lev();
	    uint32_t row = get_goal_row();
	    uint32_t col = get_goal_col();
	    char direction = getDirection(lev, row, col);
        bool bt = true;


	    while(bt) {
		    if (direction == 'n') row += 1;
		    else if (direction == 'e') col -= 1;
		    else if (direction == 's') row -= 1;
		    else if (direction == 'w') col += 1;
		    else { 
		    	char temp = (char) ((uint32_t) '0' + lev);
		    	lev = (uint32_t)(direction - '0');
		    	direction = temp;
            } // else
		    setTile(lev, row, col, direction); // change tile to direction
		    direction = getDirection(lev, row, col); // set direction to new tile's direction

		    if (direction == 'S') bt = false; // found start - end the loop
	    } // while

        // iterate through and print each Tile
        for (uint32_t lev_loop = 0; lev_loop < get_num_levs(); lev_loop++) {
            cout << "//level " << lev_loop << "\n";
            for (uint32_t row_loop = 0; row_loop < get_size(); row_loop++) {
                for (uint32_t col_loop = 0; col_loop < get_size(); col_loop++) {
                    cout << getTile(lev_loop, row_loop, col_loop);
                }
                cout << "\n"; // end line whenever rows are done printing??...yessir
            }
        } // for
        return;
    } //print_map


    void print_list (char mode) {
        bool path_found;
	    deque<location> output;

        cout << "//path taken\n"; // print this first because we need it regardless

        if (mode == 's') {
            path_found = stack_findPath(); // using stack if mode == 's'
        }
        else if (mode == 'q') {
            path_found = queue_findPath(); // otherwise using queue
        }
	    else {
		    cerr << "should never see this @ 262";
		    exit(1);
	    }

        if (!path_found) { 
            // print path taken - nothing after that...
            return;
        }

	    uint32_t lev = get_goal_lev();
	    uint32_t row = get_goal_row();
	    uint32_t col = get_goal_col();
	    char direction = getDirection(lev, row, col);
	    bool bt = true;

	    while(bt) {
	    	if (direction == 'n') row += 1;
	    	else if (direction == 'e') col -= 1;
	    	else if (direction == 's') row -= 1;
	    	else if (direction == 'w') col += 1;
	    	else { 
		    	char temp = (char) ((uint32_t) '0' + lev);
		    	lev = (uint32_t)(direction - '0');
		    	direction = temp;;
	    	} // else
	
		    char temp_direction = direction;
		    direction = getDirection(lev, row, col);
            if (getTile(lev, row, col) == 'S' || direction == 'S' ) {
                bt = false;
            }
		    setDirection(lev, row, col, temp_direction);
		    output.emplace_back(location(lev, row, col));

		    if (direction == 'S') bt = false;

	    } // while

        // now the list output...
        while(!output.empty()) {
            auto current = output.back();
            output.pop_back();
            cout << '(' << current.lev << ',' << current.row << ',' << current.col 
                        << ',' <<  getDirection(current.lev, current.row, current.col) << ')' << "\n";
        }
    } // print_list()
   
private:
    deque<location> fs;
    
     // first in (pushes back), last out (pops/peeks back)
bool stack_findPath() {

    fs.emplace_back(get_start_lev(), get_start_row(), get_start_col());
    setDirection(get_start_lev(), get_start_row(), get_start_col(), 'S');
    while (!fs.empty()) {
        auto current = fs.back();
        fs.pop_back();
        uint32_t l = current.lev;
        uint32_t r = current.row;
        uint32_t c = current.col;

        auto tile = get_location_tile(l, r, c);

        if (tile.symbol == 'E') {
            for (uint32_t i = 0; i < get_num_levs(); i++) {
                auto temp = get_location_tile(i, r, c);
                if (i != l && temp.symbol == 'E' && temp.direction == '\0') { // not discovered
                    fs.emplace_back(i, r, c);
                    setDirection(i, r, c, static_cast<char>(48 + l));
                } // if 
            } // for
        } // if(E)

        char directions[4] = {'n', 'e', 's', 'w'};
        for (const auto& dir : directions) {
            if (dir == 's' && r != get_size() - 1) {
                tile = get_location_tile(l, r + 1, c);
                if (tile.symbol == 'H') {
                    setDirection(l, r + 1, c, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) { 
                    setDirection(l, r + 1, c, dir);
                    fs.emplace_back(l, r + 1, c);
                }
            }
            if (dir == 'e' && c != get_size() - 1) {
                tile = get_location_tile(l, r, c + 1);
                if (tile.symbol == 'H') {
                    setDirection(l, r, c + 1, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r, c + 1, dir);
                    fs.emplace_back(l, r, c + 1);
                }
               
            }
            if (dir == 'n' && r != 0) {
                tile = get_location_tile(l, r - 1, c);
                if (tile.symbol == 'H') {
                    setDirection(l, r - 1, c, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r - 1, c, dir);
                    fs.emplace_back(l, r - 1, c);
                } 
                
            }
            if (dir == 'w' && c != 0) {
                tile = get_location_tile(l, r, c - 1);
                if (tile.symbol == 'H') {
                    setDirection(l, r, c - 1, dir);
                    return true; 
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r, c - 1, dir);
                    fs.emplace_back(l, r, c - 1);
                } 
            }
        } // for(dir)
    } // while
    return false;
} // stack_findpath


     // first in (pushes back), first out (pops/peeks front)
    // removed comments for size reasons, but same code as dfs 
bool queue_findPath() { // queue
    fs.emplace_back(get_start_lev(), get_start_row(), get_start_col());
    setDirection(get_start_lev(), get_start_row(), get_start_col(), 'S');
    
    while (!fs.empty()) {
        auto current = fs.front();
        fs.pop_front();
        uint32_t l = current.lev;
        uint32_t r = current.row;
        uint32_t c = current.col;

        auto tile = get_location_tile(l, r, c);

        if (tile.symbol == 'E') {
            for (uint32_t i = 0; i < get_num_levs(); i++) {
                auto temp = get_location_tile(i, r, c);
                if (i != l && temp.symbol == 'E' && temp.direction == '\0') { // not discovered
                    fs.emplace_back(i, r, c);
                    setDirection(i, r, c, static_cast<char>(48 + l));
                } // if 
            } // for
        } // if(E)

        char directions[4] = {'n', 'e', 's', 'w'};
        for (const auto& dir : directions) {
            if (dir == 's' && r != get_size() - 1) {
                tile = get_location_tile(l, r + 1, c);
                if (tile.symbol == 'H') {
                    setDirection(l, r + 1, c, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) { 
                    setDirection(l, r + 1, c, dir);
                    fs.emplace_back(l, r + 1, c);
                }
            }
            if (dir == 'e' && c != get_size() - 1) {
                tile = get_location_tile(l, r, c + 1);
                if (tile.symbol == 'H') {
                    setDirection(l, r, c + 1, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r, c + 1, dir);
                    fs.emplace_back(l, r, c + 1);
                }
               
            }
            if (dir == 'n' && r != 0) {
                tile = get_location_tile(l, r - 1, c);
                if (tile.symbol == 'H') {
                    setDirection(l, r - 1, c, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r - 1, c, dir);
                    fs.emplace_back(l, r - 1, c);
                } 
                
            }
            if (dir == 'w' && c != 0) {
                tile = get_location_tile(l, r, c - 1);
                if (tile.symbol == 'H') {
                    setDirection(l, r, c - 1, dir);
                    return true;
                }
                if (tile.direction == '\0' && (tile.symbol != '#')) {
                    setDirection(l, r, c - 1, dir);
                    fs.emplace_back(l, r, c - 1);
                } 
            }
        } // for(dir)
    } // while
    return false;
} // queue_findpath


}; // SearchStrategy : public SpaceStation

#endif // STATION_HPP