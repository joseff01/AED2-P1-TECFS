#include "DataBlock.h"

DataBlock::DataBlock(const string& stringData, const string& filePath) {
    setDataString(stringData);
    setFilePath(filePath);
}

const string &DataBlock::getDataString() const {
    return dataString;
}

void DataBlock::setDataString(const string &dataString) {
    DataBlock::dataString = dataString;
}

const string &DataBlock::getFilePath() const {
    return filePath;
}

void DataBlock::setFilePath(const string &filePath) {
    DataBlock::filePath = filePath;
}
