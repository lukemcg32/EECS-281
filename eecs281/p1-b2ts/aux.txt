// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <iostream>
#include <getopt.h>
#include <cstring>
#include <vector>
#include <deque>
using namespace std;

// ----------------------------------------------------------------------------
//                    MapLibrary Declarations
// ----------------------------------------------------------------------------

class Game {
    
public:

    Game();

    // Read and process command line arguments.
    void get_options(int argc, char** argv);
    // Read the text input file through stdin.
    void read_data();

    // Inplement explore process
    void search();
    // void printInfor(); //debug
    // Backtrack and print output as expected
    void result();
    
private:
    struct Infor{
        char symbol = '.';
        char direction = '\0';
    };

    struct Position {
        uint32_t level;
        uint32_t row;
        uint32_t column;

        Position() { }
        Position(uint32_t level_in, uint32_t row_in, uint32_t column_in):
            level(level_in), row(row_in), column(column_in) { }
    };
     
    // Store map information
    vector<vector<vector<Infor>>> map;
    
    // Store mode
    char output_mode; // set it to map by default.
    char routine_mode  = '\0';
    // the number of levels in the space station
    uint32_t num_level;
    // the size of every level of the space station
    uint32_t size;
    Position start; // start position
    Position hangar; // hangar position
    bool path; // whether find solution

    void mapInput();
    void listInput();
    void enterData(uint32_t level_in, uint32_t row_in, uint32_t col_in,
                    char symbol_in);

    void queueMode();
    void stackMode();
    bool explore(uint32_t level, uint32_t row, uint32_t column,
                    deque<Position> &container);
    bool potential(uint32_t level, uint32_t row_in, uint32_t column_in);
    bool recordDirect(uint32_t level_in, uint32_t row_in, uint32_t column_in,
                    char direct, deque<Position> &container);
    bool elevator(uint32_t level, uint32_t row, uint32_t column,
                    deque<Position> &container);
    void mapOutput();
    void listOutput();
    void listOutputHelper(uint32_t level, uint32_t row, uint32_t column,
                          char direct_in);
};




// ----------------------------------------------------------------------------
//                               Driver
// ----------------------------------------------------------------------------

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    // Instantiate a music library.
    Game game;
    // Read and process the command line options.
    game.get_options(argc, argv);

    // Read in the provided file through stdin.
    game.read_data();
    
    game.search();
    // game.printInfor(); // debug
    game.result();   
    return (0); 
}




// ----------------------------------------------------------------------------
//                             Function
// ----------------------------------------------------------------------------

Game::Game(): output_mode('m'), num_level(0), size(0), path(false){}

// Read and process command line options.
void Game::get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options
    opterr = false;

    // use getopt to find command line options
    struct option longOpts[] = {{ "stack", no_argument, nullptr, 's'},
                                { "queue", no_argument, nullptr, 'q'},
                                { "output", required_argument, nullptr, 'o'},
                                { "help", no_argument, nullptr, 'h'}};
    
    while ((option=getopt_long(argc,argv,"sqo:h",longOpts,&option_index))!=-1){
        switch (option) {
            case 's':
                if (routine_mode == 'q'){
                    cerr << "Multiple routing modes specified" << endl;
                    exit(1);
                } // if
                else {routine_mode = 's';}
                
                break;
            
            case 'q':
                if (routine_mode == 'q'){
                    cerr << "Multiple routing modes specified" << endl;
                    exit(1);
                } // if
                else {routine_mode = 'q';}
                break;
            
            case 'o':
                if ((*optarg != 'M') && (*optarg != 'L')) {
                    cerr << "Invalid output mode specified" << endl;
                    cerr << "Invalid character is: " << optarg << endl;
                    exit(1);
                }
                else if (*optarg == 'L') {output_mode = 'l';}
                break;

            case 'h':
                cout << "This program reads a text file with the input mode,\n"
                     << " levels, size, and the layout of the space station.\n"
                     <<  "Comments may also be included after first 3 line.\n"
                     <<  "It outputs a path to hangar with specified mode,\n"
                     <<  "which assigns by command line and defaults to Map.\n"
                     <<  "Usage: \'./project1\n\t[--stack | -s]\n"
                     <<                      "\t[--queue | -q]\n"
                     <<                      "\t[--output | -o] <M|L>\n"
                     <<                      "\t[--help | -h]\n"
                     <<                      "\t< infile > outfile\'" << endl;
                exit(0);
        }
    }
    
    // make sure routine mode has been selected since it has no default
    if (!routine_mode) {
        cerr << "No routing mode specified" << endl;
        exit(1);
    }
}

// REQUIRE: The first three line is input mode (M|L), 
//          number of level, station size, and no comments.
// MODIFY: input_mode, num_level, size, map
// EFFECT: Read data into the program through stdin.
void Game::read_data() {

    char input_mode = '\0'; // M or L

    cin >> input_mode;
    cin >> num_level;
    cin >> size;
    // initialize map with the exact size
    map.resize(num_level, vector<vector<Infor>>(size, vector<Infor>(size)));

    if (input_mode == 'M'){mapInput();;}
    else {listInput();}
}

// REQUIRE: Input text is in the list mode
// EFFECT: Recognize data we want, skip comment and other symbols.
void Game::listInput(){
    char first = '\0';
    char waste = '\0';
    char symbol = '\0';
    string comment;

    uint32_t level;
    uint32_t row;
    uint32_t col;

    // Read to the end of the text file.
    while (cin >> first) {
        // Skip comments.
        if (first == '/')
            getline(cin, comment);

        // Otherwise just grab the rest of the line.
        else {
            cin >> level >> waste >> row >> waste >> col
                         >> waste >> symbol >> waste;
            // error check
            if (level >= num_level) {
                cerr << "Invalid map level" << endl;
                exit(1);
            }

            if (row >= size) {
                cerr << "Invalid map row" << endl;
                exit(1);
            }

            if (col >= size) {
                cerr << "Invalid map column" << endl;
                exit(1);
            }
            enterData(level, row, col, symbol);
        } // else
    } // while
} 

// REQUIRE: Input text is in the map mode
// EFFECT: Recognize data we want, skip comment and other symbols.
void Game::mapInput(){
    char first = '\0';
    char element = '\0';
    string comment;
    uint32_t curr_line = 0;
        
    // Read to the end of the text file.
    while (cin >> first) {
        if (curr_line / size == num_level){
            cerr << "Level exceed input number" << endl;
            exit(1);
        }
        // Skip comments.
        if (first == '/')
            getline(cin, comment);

        // Otherwise just grab the rest of the line.
        else{
            // locate where we are now and assign first element
            uint32_t curr_level = curr_line / size;
            uint32_t curr_row = curr_line % size;
            enterData(curr_level, curr_row, 0, first);
            // traverse rest of line and assign each element
            for (uint32_t curr_col = 1; curr_col < size; ++curr_col){
                cin >> element;
                enterData(curr_level, curr_row, curr_col, element);
            }
            curr_line += 1;
        } // else if
    } // while
}

// REQUIRE: The first three line is input mode (M|L)
// MODIFY: map
// EFFECT: store map information with necessary remark
void Game::enterData(uint32_t level_in, uint32_t row_in,
                     uint32_t col_in, char symbol_in){
    if (symbol_in == 'S') {
        start = {level_in, row_in, col_in};
        map[level_in][row_in][col_in].direction = 'S'; // indicates discovered
    } else if (symbol_in == 'H')
        hangar = {level_in, row_in, col_in};
    else if (symbol_in != '#' && symbol_in != 'E' && symbol_in != '.') {
        cerr << "Invalid map character" << endl;
        exit(1);
    }
    map[level_in][row_in][col_in].symbol = symbol_in;
}

// EFFECT: store map information with necessary remark
void Game::search(){
    // Prepare search container and store start position
    deque<Position> container;
    container.push_back(start);
    Position curr_loca;

    while (!container.empty()) {
        if (routine_mode == 'q') { // queue mode
            curr_loca = container.front();
            container.pop_front();
        } else {                   // stack mode
            curr_loca = container.back();
            container.pop_back();
        }
        if (explore(curr_loca.level, curr_loca.row, curr_loca.column,
                                                    container)) break;
    }
}

// REQUIRE: A valid location in map
// MODIFY: container, map
// EFFECT: explore possible next step for current location
bool Game::explore(uint32_t level, uint32_t row, uint32_t column,
                                   deque<Position> &container){
    return ((row != 0 && recordDirect(level, row-1, column, 'n', container))
    ||(column !=  size-1 && recordDirect(level, row, column+1, 'e', container))
    ||(row !=  size-1 && recordDirect(level, row+1, column, 's', container))
    ||(column != 0 && recordDirect(level, row, column-1, 'w', container))
    ||(map[level][row][column].symbol == 'E'
        && elevator(level, row, column, container)));
}

// REQUIRE: A valid location and direction in map
// MODIFY: container, map
// EFFECT: Store direction according to where it comes from
bool Game::recordDirect(uint32_t level_in, uint32_t row_in, uint32_t column_in,
                        char direct, deque<Position> &container){
    Infor &square = map[level_in][row_in][column_in];
    if (square.direction == '\0' && square.symbol != '#') {
        square.direction = direct;
        // immediately stop after finding hangar and change path
        if (square.symbol == 'H') {
            return path = true;
        }

        container.emplace_back(level_in, row_in, column_in);
    } // if
    return false;
}

// REQUIRE: A valid location
// MODIFY: container, map
// EFFECT: detect whether the location is undiscovered elevator
bool Game::elevator(uint32_t level, uint32_t row, uint32_t column,
                                    deque<Position> &container){
    // Check same position in all other levels
    for (uint32_t p_lev = 0; p_lev < num_level; ++p_lev){
        if (p_lev != level && map[p_lev][row][column].symbol == 'E'
                           && map[p_lev][row][column].direction == '\0') {
            container.emplace_back(p_lev, row, column);
            // Store which level it comes from
            map[p_lev][row][column].direction = (char) ((uint32_t)'0' + level);
        }
    }
    return false;
}

void Game::result(){
    if (output_mode == 'l') listOutput();
    else mapOutput();
}

// MODIFY: map
void Game::mapOutput(){
    uint32_t bt_level;
    uint32_t bt_row;
    uint32_t bt_column;
    char bt_direct;
    bool backtrack = true; // A trigger, false (stop) if back to start

    // If we found a solution, then do backtrack.
    if (path){
        // temporary information of backtracking
        bt_level = hangar.level;
        bt_row = hangar.row;
        bt_column = hangar.column;
        bt_direct = map[bt_level][bt_row][bt_column].direction;
    } else {
        backtrack = false;
    }

    while (backtrack) {

        // restore direction information to previous step
        if (bt_direct == 'n') {
            bt_row += 1;
        } else if (bt_direct == 'e') {
            bt_column -= 1;
        } else if (bt_direct == 's') {
            bt_row -= 1;
        } else if (bt_direct == 'w') {
            bt_column += 1;
        } else { // elevator
            char temp_level = (char) ((uint32_t) '0' + bt_level);
            bt_level = (uint32_t)(bt_direct - '0');
            bt_direct = temp_level;
        }
        map[bt_level][bt_row][bt_column].symbol = bt_direct;
        bt_direct = map[bt_level][bt_row][bt_column].direction;

        // detect whether back to start
        if (bt_direct == 'S') backtrack = false;
    }

    // print map with or without solution
    cout << "Start in level " << start.level << ", row " << start.row
                              << ", column " << start.column << '\n';
    for (uint32_t lev = 0; lev < num_level; ++lev) {
        cout << "//level " << lev << '\n';
        for (uint32_t row = 0; row < size; ++row) {
            for (uint32_t col = 0; col < size; ++col) {
                cout << map[lev][row][col].symbol;
            }
            cout << '\n';
        } // for
    } // for
}

// By Stack
void Game::listOutput() {
    cout << "//path taken\n";

    if (!path) return;
    bool backtrack = true;

    // store temporary information during backtracking
    deque<Position> outputContainer;
    uint32_t bt_level = hangar.level;
    uint32_t bt_row = hangar.row;
    uint32_t bt_column = hangar.column;
    char bt_direct = map[bt_level][bt_row][bt_column].direction;

    while (backtrack) {
        // restore direction infor to previous step
        if (bt_direct == 'n') {
            bt_row += 1;    
        } else if (bt_direct == 'e') {
            bt_column -= 1;
        } else if (bt_direct == 's') {
            bt_row -= 1;
        } else if (bt_direct == 'w') {
            bt_column += 1;
        } else { // elevator
            char temp_level = (char) ((uint32_t) '0' + bt_level);
            bt_level = (uint32_t) bt_direct - '0';
            bt_direct = temp_level;
        }
        
        char temp_direct = bt_direct;
        bt_direct = map[bt_level][bt_row][bt_column].direction;
        map[bt_level][bt_row][bt_column].direction = temp_direct;
        outputContainer.emplace_back(Position(bt_level, bt_row, bt_column));

        // detect whether back to start
        if (bt_direct == 'S') backtrack = false;
    }

    // Print solution
    while (!outputContainer.empty()) {
        Position print = outputContainer.back();
        cout << "(" << print.level << "," << print.row << "," << print.column
          <<","<< map[print.level][print.row][print.column].direction << ")\n";
        outputContainer.pop_back();
    }
}

// Recursion
// void Game::listOutput() {
//     cout << "//path taken\n";
//     if (path) {
//     listOutputHelper(hangar.level, hangar.row, hangar.column,
//                     '\0');
//     }
// }

// void Game::listOutputHelper(uint32_t level_in, uint32_t row_in,
//                             uint32_t column_in, char direct_in) {
    
//     // Base case: found start
//     if (level_in == start.level && row_in == start.row
//                                 && column_in == start.column){
//         cout << "(" << level_in << "," << row_in << "," << column_in
//           <<","<< direct_in << ")\n";
//         return;
//     }

//     char next_direct = map[level_in][row_in][column_in].direction;
//     if (next_direct == 'n') {
//         listOutputHelper(level_in, row_in+1, column_in, next_direct);

//     } else if (next_direct == 'e') {
//         listOutputHelper(level_in, row_in, column_in-1, next_direct);

//     } else if (next_direct == 's') {
//         listOutputHelper(level_in, row_in-1, column_in, next_direct);

//     } else if (next_direct == 'w') {
//         listOutputHelper(level_in, row_in, column_in+1, next_direct);

//     } else { // elevator
//         uint32_t next_level = (uint32_t) next_direct - '0';
//         listOutputHelper(next_level, row_in, column_in, 
//                          (char) ((uint32_t) '0' + level_in));
//     }

//     if (direct_in != '\0') {
//         cout << "(" << level_in << "," << row_in << "," << column_in
//           <<","<< direct_in << ")\n";
//     }
//     return;
// }

// // For debug
// void Game::printInfor(){
//     cout << "Orginal Map" <<endl;
//     for (uint32_t lev = 0; lev < num_level; ++lev) {
//         cout << "//level " << lev << '\n';
//         for (uint32_t row = 0; row < size; ++row) {
//             for (uint32_t col = 0; col < size; ++col) {
//                 cout << map[lev][row][col].symbol;
//             }
//             cout << '\n';
//         }
//     }
//     cout <<"Backtrack Information" <<endl;
//     for (uint32_t lev = 0; lev < num_level; ++lev) {
//         cout << "//level " << lev << '\n';
//         for (uint32_t row = 0; row < size; ++row) {
//             for (uint32_t col = 0; col < size; ++col) {
//                 if (map[lev][row][col].direction == '\0'){
//                     cout << '.';
//                 }else cout << map[lev][row][col].direction;
//             }
//             cout << '\n';
//         }
//     }
// }