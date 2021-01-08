#include <iostream>
#include <string>

#include "include/lib/sha256.hpp"
#include "include/lib/md5.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    // TODO: https://ru.wikipedia.org/wiki/Tiger_(%D1%85%D0%B5%D1%88-%D1%84%D1%83%D0%BD%D0%BA%D1%86%D0%B8%D1%8F)
    // SHA-1
    // SHA-2 https://tools.ietf.org/html/rfc4634
    // SHA-3
    // OpenSSL support https://mind-control.fandom.com/wiki/OpenSSL
    if (argc < 3) {
        cout << "Unsupported arguments count: " << argc << endl;
        return 1;
    }

    string algorithm = string(argv[1]);
    string filePath = string(argv[2]);

    if (algorithm == "sha256") {
        string stringHash = sha2::sha256(filePath);
        cout << stringHash << endl;
    } else if (algorithm == "md5") {
        string stringHash = md::md5(filePath);
        cout << stringHash << endl;
    } else {
        cout << "Unkonwn algorithm: " << algorithm << endl;
        return 1;
    }

    return 0;
}
