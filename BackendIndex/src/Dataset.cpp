#include <iostream>
#include <fstream>
#include <stdint.h>

#include <Dataset.hpp>
#include <Utils.h>

using namespace std;

Dataset::Dataset(string binFilePath, DatasetType dsType, const BoundD &bound, int64_t limitNbRecords, bool isBin) :
        binFilePath(binFilePath), dummyRecord(Record::getNewRecord(dsType)),
        bound(bound),
        limitNbRecords(limitNbRecords),
        isBin(isBin), nbRecordsRead(0), dsType(dsType) {}

bool Dataset::getNextRecord(Record *record) {
    if (
            !binFile.is_open() || !binFile.good() ||                    // file not properly open
            (limitNbRecords > 0 && nbRecordsRead >= limitNbRecords) ||    // limit of records to be read passed
            nbRecordsRead >= totalNbRecords                                // no records left to read
            ) {
        return false;
    }
    bool found = false;

    if (isBin) {
        while (nbRecordsRead < totalNbRecords && !found) {
            binFile.read(record->getPointer(), getRecordSize());
            nbRecordsRead++;
            STdims stdims = record->getIndexDimensions();
            PointF transformed;
            if (transformPoint(PointF(stdims.x, stdims.y), transformed, bound)) {
                found = true;
                record->updateRecordLocation(transformed.x(), transformed.y());
            }
        }
    } else {
        std::string line;
        while (nbRecordsRead < totalNbRecords && !found) {
            std::getline(binFile, line);
            if (line.empty()) {
                continue;
            }
            record->parseLine(line);
            nbRecordsRead++;
            STdims stdims = record->getIndexDimensions();
            PointF transformed;
            if (transformPoint(PointF(stdims.x, stdims.y), transformed, bound)) {
                found = true;
                record->updateRecordLocation(transformed.x(), transformed.y());
            }
        }
    }
    //XXX
    //std::cerr << "DEBUG: val= [" << record->getIndexDimensions()[0] << ", " << record->getIndexDimensions()[1] << ", " << record->getIndexDimensions()[2] << "]" << std::endl;

    return found;
}

void Dataset::openFile() {
    if (isBin) {
        // open file
        binFile.open(binFilePath, ios::in | ios::binary);

        // get the file size
        int64_t begin, end;
        begin = binFile.tellg();
        binFile.seekg(0, ios::end);
        end = binFile.tellg();

        int64_t diff = (end - begin);
        totalNbRecords = diff / getRecordSize();

        // go back to beginning
        binFile.seekg(0, ios::beg);

        cout << "Dataset: opened file with " << totalNbRecords << " records (with " << diff << " bytes and record size "
             << getRecordSize() << ")." << endl;
    } else {
        binFile.open(binFilePath, ios::in);
        totalNbRecords = std::count(std::istreambuf_iterator<char>(binFile),
                                    std::istreambuf_iterator<char>(), '\n');
        cout << "Dataset: opened file with " << totalNbRecords << " records" << endl;
        binFile.clear();
        binFile.seekg(0, std::ios::beg);
    }
}

void Dataset::closeFile() {
    binFile.close();
}

uint64_t Dataset::getRecordSize() {
    return dummyRecord->getRecordSize();
}

uint64_t Dataset::getNbAttributes() {
    return dummyRecord->getNbAttributes();
}

size_t Dataset::getAttributeSize(unsigned int n) {
    return dummyRecord->getAttributeSize(n);
}

DatasetType Dataset::getDsType() {
    return this->dsType;
}

