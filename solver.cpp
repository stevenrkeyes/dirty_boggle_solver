#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <locale>
#include <algorithm>
#include <chrono>

#include "trie.h"

#define WIDTH  4
#define HEIGHT 4

#define TIME_DIAGNOSTICS

// Function to check if a word has a Q in it without being QU because these
// words are not present in certain versions of Boggle
bool has_independent_Q(std::string s)
{
    std::string::iterator c_iter;

    for (c_iter = s.begin(); c_iter != s.end(); ++c_iter)
    {
        if (*c_iter == 'Q')
        {
            if (*(c_iter + 1) != 'U')
            {
                return true;
            }
        }
    }

    return false;
}

// Strip all non-letters from the word (spaces, hyphens, punctuation, etc)
std::string strip(const std::string &s) {
    std::string result;

    result.reserve(s.length());
    std::remove_copy_if(s.begin(),
                        s.end(),
                        std::back_inserter(result),
                        std::not1(std::ptr_fun(isalpha)));

    return result;
}

// Convert a word s into an array of capitalized letter strings
// This is necessary because we want to represent QU as one unit and also
// ignore non-letters
std::vector<std::string> vectorize(std::string s)
{
    // "#" indicates a comment
    if (s[0] == '#') {
        return std::vector<std::string>();
    }
    // Make uppercase
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    // Remove non-letters
    std::string clean_string = strip(s);
    // The boggle board doesn't contain independent Q tiles, so words with Q
    // without U can be ignored
    if (has_independent_Q(clean_string))
    {
        return std::vector<std::string>();
    }

    std::vector<std::string> v;
    std::string::iterator c_iter;

    for (c_iter = clean_string.begin(); c_iter != clean_string.end(); ++c_iter)
    {
        if (*c_iter == 'Q')
        {
            v.push_back("QU");
            ++c_iter;
        }
        else
        {
            v.push_back(std::string(1, *c_iter));
        }
    }
    return v;
}

// Load words from a dictionary file (a list of words, one word per line) into
// a prefix tree
trie::Trie<std::string> load_words(const char *filename)
{
    std::ifstream file(filename);
    trie::Trie<std::string> words;
    std::locale loc;
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string> vectorized_word = vectorize(line);
        if (!vectorized_word.empty())
        {
            words.insert(vectorized_word);
        }
    }

    return words;
}

// Finds boggle words starting from a given tile (or string of tiles)
// (via recursion, guided by a prefix tree)
// The string of previous tiles is represented as a grid of visited tiles, the prefix that
// those tiles become, and the coordinates (i, j) of the next to visit tile.
std::vector<std::string> get_boggle_words_with_prefix(std::string boggle_board[WIDTH][HEIGHT],
                                                      bool past_visited_tiles[WIDTH][HEIGHT],
                                                      int i,
                                                      int j,
                                                      std::string prefix,
                                                      trie::Node<std::string> previous_node) {
    bool visited_tiles[WIDTH][HEIGHT];
    std::copy(&past_visited_tiles[0][0],
              &past_visited_tiles[0][0] + WIDTH * HEIGHT,
              &visited_tiles[0][0]);
    visited_tiles[i][j] = true;

    // initialize an array to store any found words
    std::vector<std::string> found_words;

    // check if the new letter is in the children of the previous prefix node
    // if not, stop looking because it's not in the prefix tree of valid words
    trie::Node<std::string> *current_node = previous_node.findChild(boggle_board[i][j]);
    if (!current_node) {
        return found_words;
    }

    // otherwise, append the new letter to the prefix
    prefix.append(boggle_board[i][j]);

    // also, check if that child itself completes a word
    if (current_node->wordMarker()) {
        found_words.push_back(prefix);
    }
    // and continue searching

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
                                                                  prefix,
                                                                  *current_node);
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

    // Use a prefix tree of dictionary words to know when to stop searching down a path
    //trie::Trie<std::string> words = load_words("/usr/share/dict/words");
    trie::Trie<std::string> words = load_words("dirty_words");
    trie::Node<std::string> root_node = *words.getRootNode();

    // find words starting from each tile
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            new_found_words = get_boggle_words_with_prefix(boggle_board,
                                                           visited_tiles,
                                                           i,
                                                           j,
                                                           prefix,
                                                           root_node);
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

struct compare_length_then_alphabet_order {
    bool operator()(const std::string& first, const std::string& second) {
        if (first.size() == second.size()) {
            return first < second;
        } else {
            return first.size() > second.size();
        }
    }
};

int main()
{
    std::cout << "Welcome to the Dirty Boggle Solver\n";

    std::string boggle_board[WIDTH][HEIGHT];
    get_boggle_board(boggle_board);

    std::cout << "Finding dirty words...\n";

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    std::vector<std::string> all_found_words = get_boggle_words(boggle_board);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    int duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    #ifdef TIME_DIAGNOSTICS
    std::cout << "Executed in " << duration << " milliseconds\n";
    #endif //TIME_DIAGNOSTICS

    std::cout << "Words found:\n";

    // all_found_words contains duplicates if the word was was found in different ways
    std::set<std::string, compare_length_then_alphabet_order> found_words(all_found_words.begin(),
                                                                          all_found_words.end());
    //std::sort(all_found_words.begin(), found_words.end(), c);

    for (std::set<std::string>::const_iterator i = found_words.begin();
         i != found_words.end();
         ++i) {
        std::cout << *i << ", ";
    }

    std::cout << "\n";

    return 0;
}
