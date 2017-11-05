#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <locale>
#include <algorithm>
#include <chrono>

#define WIDTH  4
#define HEIGHT 4

#define TIME_DIAGNOSTICS

// Load words from a dictionary file (a list of words, one word per line) into a hash table
std::unordered_set<std::string> load_words(const char *filename)
{
    std::ifstream file(filename);

    std::unordered_set<std::string> words;

    std::locale loc;

    for (std::istream_iterator<std::string> i = std::istream_iterator<std::string>(file); 
        i != std::istream_iterator<std::string>();
        ++i) {
        std::string word = *i;
        // "#" indicates a comment
        if (word[0] != '#') {
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
            words.insert(word);
        }
    }

    return words;
}

// Determine whether a string is a word in the supplied dictionary
// Did some tests: doing a linear list search to check if the word exists takes OTO 3 ms,
// but doing a hash table lookup takes OTO 200 ns, which is enough to search every 
// possible boggle string.
bool is_word(std::string str)
{
    // static so that we only load the words into the hash table once
    static std::unordered_set<std::string> words = load_words("dirty_words");
    return words.end() != words.find(str);
}

// Finds boggle words starting from a given tile (or string of tiles) (via recursion)
// The string of previous tiles is represented as a grid of visited tiles, the prefix that
// those tiles become, and the coordinates (i, j) of the next to visit tile.
// Note: this checks about 12 million possible strings.
std::vector<std::string> get_boggle_words_with_prefix(std::string boggle_board[WIDTH][HEIGHT],
                                                      bool past_visited_tiles[WIDTH][HEIGHT],
                                                      int i,
                                                      int j,
                                                      std::string prefix) {
    bool visited_tiles[WIDTH][HEIGHT];
    std::copy(&past_visited_tiles[0][0],
              &past_visited_tiles[0][0] + WIDTH * HEIGHT,
              &visited_tiles[0][0]);
    visited_tiles[i][j] = true;
    prefix.append(boggle_board[i][j]);

    // initialize an array to store any found words
    std::vector<std::string> found_words;

    if (is_word(prefix)) {
        found_words.push_back(prefix);
    }

    // Check each adjacent tile to the most recently visited tile
    for (int row = i - 1; row <= i + 1 && row < HEIGHT; ++row) {
        for (int col = j - 1; col <= j + 1 && col < WIDTH; ++col) {
            if (row >= 0 && col >= 0 && !visited_tiles[row][col]) {
                std::vector<std::string> longer_found_words;
                // find any words that start with the current prefix
                longer_found_words = get_boggle_words_with_prefix(boggle_board,
                                                                  visited_tiles,
                                                                  row,
                                                                  col,
                                                                  prefix);
                // concatenate them to the current list of found words
                found_words.insert(found_words.end(),
                                   longer_found_words.begin(),
                                   longer_found_words.end());
            }
        }
    }

    return found_words;
}

// Finds all words on a boggle board by calling the recursive function
// get_boggle_words_with_prefix
std::vector<std::string> get_boggle_words(std::string boggle_board[WIDTH][HEIGHT]) {
    bool visited_tiles[WIDTH][HEIGHT] = {{false}};
    std::string prefix = "";

    std::vector<std::string> found_words;
    std::vector<std::string> new_found_words;

    // find words starting from each tile
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            new_found_words = get_boggle_words_with_prefix(boggle_board,
                                                           visited_tiles,
                                                           i,
                                                           j,
                                                           prefix);
            found_words.insert(found_words.end(),
                               new_found_words.begin(),
                               new_found_words.end());
        }
    }

    return found_words;
}

void print_boggle_board(std::string boggle_board[WIDTH][HEIGHT]) {
    for (int j = 0; j < HEIGHT; ++j) {
        for (int i = 0; i < WIDTH; ++i) {
            std::string letter = boggle_board[i][j];
            if (letter != "QU") {
                std::cout << letter << "  ";
            } else {
                std::cout << letter << " ";
            }
        }
        std::cout << "\r\n";
    }
}

// Get the boggle board from the user and return it via boggle_board as an output parameter
void get_boggle_board(std::string boggle_board[WIDTH][HEIGHT]) {
    std::cout << "Please enter the boggle tiles in reading order.\r\n";
    
    std::string board[WIDTH][HEIGHT] = {{" "," "," "," "},
                                        {" "," "," "," "},
                                        {" "," "," "," "},
                                        {" "," "," "," "}};
    std::string input_letter;

    for (int j = 0; j < HEIGHT; ++j) {
        for (int i = 0; i < WIDTH; ++i) {
            std::cout << "\nNext letter: ";
            std::cin >> input_letter;
            std::cout << "\n";
            // ok this seems a little silly, there's gotta be a better way to uppercase a
            // letter in c++
            std::transform(input_letter.begin(),
                           input_letter.end(),
                           input_letter.begin(),
                           ::toupper);
            if (input_letter == "Q") {
                input_letter = "QU";
            }
            board[i][j] = input_letter;
            print_boggle_board(board);
        }
    }

    
    std::copy(&board[0][0], &board[0][0] + WIDTH * HEIGHT, &boggle_board[0][0]);
}

// Oh my
struct compare_length {
    bool operator()(const std::string& first, const std::string& second) {
        return first.size() > second.size();
    }
};

int main()
{
    std::cout << "Welcome to the Dirty Boggle Solver\n";

    std::string boggle_board[WIDTH][HEIGHT];
    get_boggle_board(boggle_board);

    std::cout << "Finding dirty words...\n";

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    std::vector<std::string> found_words = get_boggle_words(boggle_board);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    int duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    #ifdef TIME_DIAGNOSTICS
    std::cout << "Executed in " << duration << " milliseconds\n";
    #endif //TIME_DIAGNOSTICS

    std::cout << "Words found:\n";

    compare_length c;
    std::sort(found_words.begin(), found_words.end(), c);

    for (std::vector<std::string>::const_iterator i = found_words.begin();
         i != found_words.end();
         ++i) {
        std::cout << *i << ", ";
    }

    std::cout << "\n";

    return 0;
}
