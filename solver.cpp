#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>

#define WIDTH  4
#define HEIGHT 4

std::unordered_set<std::string> load_words(const char *filename)
{
    std::ifstream file(filename);

    std::unordered_set<std::string> words;

    std::copy(std::istream_iterator<std::string>(file),
              std::istream_iterator<std::string>(),
              std::inserter(words, words.end()));
    return words;
}

bool is_word(std::string str)
{
    static std::unordered_set<std::string> words = load_words("/usr/share/dict/words");
    return words.end() != words.find(str);
}

int main()
{
    std::string foo = "blooooop";

    // Did some tests: doing a linear list search to check if the word exists takes OTO 3 ms,
    // but doing a hash table lookup takes OTO 200 ns, which is enough to search every 
    // possible boggle string.

    is_word(foo);

    std::cout << "Welcome to the Dirty Boggle Solver\n";
    return 0;
}
