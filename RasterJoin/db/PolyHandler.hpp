#ifndef POLYHANDLER_H
#define POLYHANDLER_H

#include <QString>
#include <QPointF>
#include <QVector>
#include <QMap>

#include <vector>

#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include "clip2tri/clip2tri.h"

#include "Utils.h"
#include "Common.h"

using namespace c2t;

typedef vector <Point> TPolygon;
typedef QVector <TPolygon> PolygonCollection;

struct Bound {
    QPointF leftBottom, rightTop;
};

/**
 * Assumes polygons can fit in main memory
 */
class PolyHandler {
public:
    PolyHandler();

    void initFromFile(QString polyIndex, const BoundD &origin_bound);

    void getTriangulation(std::vector<float> &verts, std::vector<float> &ids);

    QVector<float> getPolyOutline();

    QVector<float> getOutlineIds();

    Bound getBounds();

    int getNoPolys();

protected:
    void addPolygonCollection(QString collectionName, QString polyFile);

    void addPolygonCollection(QString collectionName, const PolygonCollection &polys, const Bound &collectionBound);

public:
    QMap <QString, PolygonCollection> polyDb;
    QMap <QString, Bound> dbBounds;
    QMap <QString, QVector<float>> polys;
    QMap <QString, QVector<int>> pindexes;
    QMap <QString, QVector<float>> outlines;
    QMap <QString, QVector<float>> oids;
    QMap <QString, QVector<float>> eids;
    QString currentCollection;
    BoundD origin_bound;

public:
    static void readPolygons(QString polyFile, PolygonCollection &polys, QPointF &leftBottom, QPointF &rightTop,
                             const BoundD &origin_bound);
};

#endif // POLYHANDLER_H
