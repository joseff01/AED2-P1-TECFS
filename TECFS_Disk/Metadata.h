#ifndef TECFS_DISK_METADATA_H
#define TECFS_DISK_METADATA_H

#include <iostream>

using namespace std;

class Metadata {
private:
    int startBit = 0;
    int fileLength = 0;
    int fileNum = 0;
    string filePath;

public:
    [[nodiscard]] int getStartBit() const;
    void setStartBit(int startBit);
    [[nodiscard]] int getFileLength() const;
    void setFileLength(int fileLength);
    [[nodiscard]] const string &getFilePath() const;
    void setFilePath(const string &filePath);
    [[nodiscard]] int getFileNum() const;
    void setFileNum(int fileNum);
    Metadata(int startBit, int fileLength, const string& filePath);
};


#endif //TECFS_DISK_METADATA_H
