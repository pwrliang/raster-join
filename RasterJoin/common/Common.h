#ifndef COMMON
#define COMMON

#include <stdint.h>
#include <QVector>
#include <QtAlgorithms>
#include <QPointF>

#define DISK_PAGE_SIZE 4096//16384 //4096
#define FILE_BUFFER_SIZE 4*DISK_PAGE_SIZE

#define ONE_GB (1024*1024*1024)

//#define BUFFER_SIZE (ONE_GB*4UL)
// 222 objects of 16 bytes (attributes pick_y pick_x pickup_time) = 3552 bytes
//#define BUFFER_SIZE (3552UL) 
// 3 objects of 16 bytes (attributes pick_y pick_x pickup_time) = 160 bytes
#define BUFFER_SIZE (ONE_GB * 100ULL)

enum AttributeType {
    Location = 0, Uint, Int, Float
};

enum ConstraintType {
    EQ = 0, LT, LTE, GT, GTE
};

enum DatasetType {
    Taxi = 0, Twitter, SimplePoint
};

enum Aggregation {
    Count = 0, Avg, Sum
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

typedef QVector<int> Node;
typedef QVector<bool> NodeFlag;

struct PointF {
    float xv, yv;

    PointF() : xv(0), yv(0) {}

    PointF(float x, float y) : xv(x), yv(y) {}

    inline float x() const {
        return xv;
    }

    inline float y() const {
        return yv;
    }

    inline PointF operator+(const PointF &p) const {
        return PointF(xv + p.xv, yv + p.yv);
    }

    inline PointF operator-(const PointF &p) const {
        return PointF(xv - p.xv, yv - p.yv);
    }

    inline PointF operator*(float a) const {
        return PointF(a * xv, a * yv);
    }

    inline bool operator==(const PointF &p) {
        return (xv == p.xv && yv == p.yv);
    }

    inline bool operator!=(const PointF &p) {
        return (xv != p.xv || yv != p.yv);
    }

    inline QPointF toQPointF() {
        return QPointF(xv, yv);
    }

    inline float *data() {
        float val[2] = {xv, yv};
        return val;
    }
};

struct BoundF {
    float minx, miny;
    float maxx, maxy;

    BoundF() {
        maxx = -std::numeric_limits<float>::max();
        maxy = -std::numeric_limits<float>::max();
        minx = std::numeric_limits<float>::max();
        miny = std::numeric_limits<float>::max();
    }

    inline void updateBound(float x, float y) {
        minx = std::min(minx, x);
        maxx = std::max(maxx, x);
        miny = std::min(miny, y);
        maxy = std::max(maxy, y);
    }

    inline QPointF qcenter() {
        return QPointF((minx + maxx) / 2, (miny + maxy) / 2);
    }
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

typedef QVector <PointF> PolygonF;

#endif // COMMON

