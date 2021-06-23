#include "Metadata.h"

Metadata::Metadata(int startBit, int fileLength, const string& filePath) {
    setStartBit(startBit);
    setFileLength(fileLength);
    setFilePath(filePath);
}

int Metadata::getStartBit() const {
    return startBit;
}

void Metadata::setStartBit(int startBit) {
    Metadata::startBit = startBit;
}

int Metadata::getFileLength() const {
    return fileLength;
}

void Metadata::setFileLength(int fileLength) {
    Metadata::fileLength = fileLength;
}

const string &Metadata::getFilePath() const {
    return filePath;
}

void Metadata::setFilePath(const string &filePath) {
    Metadata::filePath = filePath;
}

