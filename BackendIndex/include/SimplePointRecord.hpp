#ifndef RASTER_POINT_RECORD
#define RASTER_POINT_RECORD

#include <stdint.h>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>


#include <Record.hpp>


/**
 * The data record (size = 16 bytes).
 */

struct SimplePointEntry {
    uint32_t point_id; // 4 bytes
    uint32_t time; // 4 bytes
    float loc_x; // 4 bytes
    float loc_y; // 4 bytes
};


class SimplePointRecord : public Record {

private:
    SimplePointEntry data;

public:

    size_t getAttributeSize(unsigned int n) {

        switch (n) {
            case 0:
                return sizeof(data.point_id);
            case 1:
                return sizeof(data.time);
            case 2:
                return sizeof(data.loc_x) + sizeof(data.loc_y); //TODO: check, which one is int64_t, lat?

            default:
                std::cerr << "Warning: wrong column called in TwitterRecord.getAttributeSize" << std::endl;
                return 0;
        }
    }

    std::unique_ptr<char[]> getAttributeAsBinary(unsigned int n) {
        using namespace std;

        unique_ptr<char[]> binAttrib(new char[getAttributeSize(n)]);
        char *begin;

        switch (n) {

            case 0: {
                begin = reinterpret_cast<char *>(&data.point_id);
                memcpy(binAttrib.get(), begin, getAttributeSize(n));
                break;
            }
            case 1: {
                uint32_t time_temp = (uint32_t) data.time;
                begin = reinterpret_cast<char *>(&time_temp);
                memcpy(binAttrib.get(), begin, getAttributeSize(n));
                break;
            }
            case 2: {
                begin = reinterpret_cast<char *>(&data.loc_x); //TODO: check, which one is int64_t, lat?
                memcpy(binAttrib.get(), begin, sizeof(data.loc_x));

                begin = reinterpret_cast<char *>(&data.loc_y);
                memcpy(binAttrib.get() + sizeof(data.loc_x), begin, sizeof(data.loc_y));
                break;
            }

            default:
                std::cerr << "Warning: wrong column called in TwitterRecord.getAttributeAsBinary" << std::endl;
                return NULL;
        }
        return binAttrib;
    }


    STdims getIndexDimensions() const {
        STdims indexDims;
        indexDims.x = data.loc_x; //TODO: check which one is int64_t, lat
        indexDims.y = data.loc_y;
        indexDims.t = data.time;

        return indexDims;
    }

    uint64_t getRecordSize() {
        // XXX: should be 32 here, insert assert?
        return sizeof(SimplePointEntry);
    }

    uint64_t getNbAttributes() {
        return 3;
    }

    char *getPointer() {
        return (char *) &data;
    }

    void parseLine(const std::string &line) override {
        std::istringstream iss(line);
        iss >> data.point_id;
        iss >> data.time;
        iss >> data.loc_x;
        iss >> data.loc_y;
        // printf("id %d, time %d lat %.6f lon %.6f\n", data.point_id, data.time, data.loc_x, data.loc_y);
    }

    void updateRecordLocation(float x, float y) {
        data.loc_x = x;
        data.loc_y = y;
    }

};

#endif // RASTER_POINT_RECORD

