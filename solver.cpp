#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

std::vector<std::string> load_words(const char *filename)
{
    std::ifstream file(filename);
    std::vector<std::string> words;
    std::copy(std::istream_iterator<std::string>(file),
              std::istream_iterator<std::string>(),
              std::back_inserter(words));
    return words;
}

int main()
{
    std::vector<std::string> words = load_words("/usr/share/dict/words");

    std::cout << "Welcome to the Dirty Boggle Solver\n";
    return 0;
}
