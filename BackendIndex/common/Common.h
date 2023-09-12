#ifndef COMMON
#define COMMON

#include <stdint.h>

#include <QVector>
#include <QtAlgorithms>

#define ONE_GB (1024LL*1024LL*1024LL)

#define DISK_PAGE_SIZE 4096
#define FILE_BUFFER_SIZE 4*DISK_PAGE_SIZE
#define BUFFER_SIZE (ONE_GB*4UL)


//FOR TESTING
// 222 objects of 16 bytes (attributes pick_y pick_x pickup_time) = 3552 bytes
//#define BUFFER_SIZE (3552UL) 
// 3 objects of 16 bytes (attributes pick_y pick_x pickup_time) = 160 bytes


enum AttributeType {
    Location = 0, Uint, Int, Float
};

enum ConstraintType {
    EQ = 0, LT, LTE, GT, GTE
};

enum DatasetType {
    Taxi = 0, Twitter, SimplePoint
};

union Value {
    int ival;
    float fval;
    uint32_t uval;
};

struct QueryConstraint {
    int attribId;
    ConstraintType type;
    Value val;
    int dummy;
};


struct BoundD {
    double min_lat, max_lat;
    double min_lon, max_lon;

    BoundD() {
        min_lat = std::numeric_limits<double>::max();
        max_lat = -std::numeric_limits<double>::max();
        min_lon = std::numeric_limits<double>::max();
        max_lon = -std::numeric_limits<double>::max();
    }
};

typedef QVector<int> Node;
typedef QVector<bool> NodeFlag;


#endif // COMMON

