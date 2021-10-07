/*
    sha1.hpp - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#ifndef SHA1_HPP
#define SHA1_HPP


#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
	int  update_hashpiece(std::ifstream &is, int piecelength, int &fileindex, vector<string> &filelist);
//    int hashpiece(std::ifstream &is, int hashindex, int piecelength, int &fileindex, vector<string> &filelist, btitem &d);
//    int  verify_torrent(string torrent_filename, string &contents_directory);
    std::string final();
    int finalraw();
    static std::string from_file(const std::string &filename);

    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};


#endif /* SHA1_HPP */
