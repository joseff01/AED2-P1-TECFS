#ifndef TECFS_DISK_DATABLOCK_H
#define TECFS_DISK_DATABLOCK_H

#include <iostream>

using namespace std;

class DataBlock {
private:
    string dataString;
    string filePath;

public:
    [[nodiscard]] const string &getDataString() const;
    void setDataString(const string &dataString);
    [[nodiscard]] const string &getFilePath() const;
    void setFilePath(const string &filePath);
    explicit DataBlock(const string& stringData, const string& filePath);
};


#endif //TECFS_DISK_DATABLOCK_H
