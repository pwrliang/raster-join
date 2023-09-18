#include <QCoreApplication>

#include<iostream>
#include <cassert>

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDataStream>

#include <HashGridIndex.hpp>
#include <Utils.h>

#define MAX_ATTR 20 //maximum number of attributes in any dataset

using namespace std;

QString databaseName, inputPointsFileName;
double min_long, max_long, min_lat, max_lat
DatasetType datasetType = SimplePoint;  //or Twitter
bool importLocationOnly = true;


void createBackendIndex() {
    vector <pair<size_t, size_t>> attributes;
    Dataset *ds;
    float partitionSize;
    size_t attrNum;
    quint8 attrType[MAX_ATTR];
    BoundD bound;

    if (datasetType == Twitter) {
        auto init = std::initializer_list<quint8>({1, 1, 0, 2, 2});
        std::copy(init.begin(), init.end(), attrType);
        attrNum = 5;
        ds = new Dataset(inputPointsFileName.toStdString(), Twitter, bound);
        attributes.push_back(make_pair(2, ds->getAttributeSize(2))); //location
        if (!importLocationOnly) { //importing other attributes
            attributes.push_back(make_pair(1, ds->getAttributeSize(1))); //time
        }
        double gres = getGroundResolution(bound);
        partitionSize = 10000 / gres;
    } else if (datasetType == Taxi) {
        bound.min_lat = 24.396308;
        bound.max_lat = 49.384358;
        bound.min_lon = -124.848974;
        bound.max_lon = -66.885444;

        auto init = std::initializer_list<quint8>({1, 0, 0, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
        std::copy(init.begin(), init.end(), attrType);
        attrNum = 20;
        ds = new Dataset(inputPointsFileName.toStdString(), Taxi, bound);
        attributes.push_back(make_pair(1, ds->getAttributeSize(1))); //pickup location
        if (!importLocationOnly) { //importing other attributes
            attributes.push_back(make_pair(3, ds->getAttributeSize(3))); //time
            attributes.push_back(make_pair(6, ds->getAttributeSize(6))); //duration
            attributes.push_back(make_pair(7, ds->getAttributeSize(7))); //miles
            attributes.push_back(make_pair(8, ds->getAttributeSize(8))); //fare
            attributes.push_back(make_pair(18, ds->getAttributeSize(18))); //passengers
        }
        double gres = getGroundResolution(bound);
        partitionSize = 1000 / gres;
    } else if (datasetType == SimplePoint) {
        bound.min_lat = min_lat;
        bound.max_lat = max_lat;
        bound.min_lon = min_long;
        bound.max_lon = max_long;

        bool valid = bound.min_lat >= -90 && bound.max_lat <= 90;
        valid &= bound.min_long >= -180 && bound.max_long <=  180;

        if (!valid) {
            printf("Invalid lat/long bound\n");
            exit(1);
        }

        // int64, int64, loc
        auto init = std::initializer_list<quint8>({Uint, Uint, Location}); // read by binResultMetadata
        std::copy(init.begin(), init.end(), attrType);
        attrNum = 3;
        ds = new Dataset(inputPointsFileName.toStdString(), SimplePoint, bound, 0, false);
        attributes.push_back(make_pair(2, ds->getAttributeSize(2))); //location
        if (!importLocationOnly) { //importing other attributes
            attributes.push_back(make_pair(0, ds->getAttributeSize(0))); //point id
            attributes.push_back(make_pair(1, ds->getAttributeSize(1))); //time
        }
        double gres = getGroundResolution(bound);
        partitionSize = 10000 / gres;
    }


    //qDebug() << "Partition Size" << partitionSize;
    HashGridIndex *index = new HashGridIndex(databaseName, partitionSize, partitionSize, bound, 864000, attrType,
                                             attrNum);
    //Time resolution: 10 days (= 864000 seconds)

    index->buildIndex(ds, attributes);

    delete ds;
    delete index;
}

HashGridIndex *loadBackendIndex(QueryResult &queryResult, size_t locAtt, BoundD &bound) {


    QFile config_file(databaseName + "_config");
    if (!config_file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open Config File";
    }

    QDataStream in(&config_file);

    float lx, ly;
    uint32_t lz;
    int p1, p2, p3;

    in >> bound.min_lat >> bound.max_lat >> bound.min_lon >> bound.max_lon >> lx >> ly >> lz >> p1 >> p2 >> p3;
    quint8 attrNum, attrType;

    printf("lat: %.6f %.6f, lon: %.6f %.6f\n", bound.min_lat, bound.max_lat, bound.min_lon, bound.max_lon);

    in >> attrNum; //number of attributes

    for (size_t i = 0; i < attrNum; i++) {
        in >> attrType;
        queryResult.addAttributeMetadata(i, attrType);
    }

    HashGridIndex *index = new HashGridIndex(databaseName, lx, ly, lz, p1, p2, p3);

    vector <pair<size_t, size_t>> attributes;
    attributes.push_back(make_pair(locAtt, queryResult.getAttributeSize(locAtt)));
    queryResult.setLocationAttribute(locAtt);

    index->loadIndex(attributes);
    return index;
}

uint32_t executeQueryFullScan(Dataset *ds, float region_low[3], float region_high[3]) {

    std::unique_ptr <Record> record = Record::getNewRecord(ds->getDsType());
    int64_t nbRecordsRead = 0;
    uint32_t nbResults = 0;

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    while (ds->getNextRecord(record.get())) {
        STdims index_dims = record->getIndexDimensions();

        min_x = std::min(min_x, index_dims.x);
        max_x = std::max(max_x, index_dims.x);
        min_y = std::min(min_y, index_dims.y);
        max_y = std::max(max_y, index_dims.y);

        if (index_dims.x >= region_low[0] && index_dims.x <= region_high[0] &&
            index_dims.y >= region_low[1] && index_dims.y <= region_high[1] &&
            index_dims.t >= region_low[2] && index_dims.t <= region_high[2]) {
            nbResults++;
        }

        if (++nbRecordsRead % 1000000 == 0) {
            std::cerr << "Read record #" << nbRecordsRead << "/" << ds->getTotalNbRecords() << std::endl;
            std::cerr << "Record: getRecordSize=" << record->getRecordSize() << ", getIndexDim=[" <<
                      index_dims.x << ", " << index_dims.y << ", " << index_dims.t << "]" << std::endl;
        }
    }
    printf("x: %f - %f, y: %f - %f\n", min_x, max_x, min_y, max_y);
    return nbResults;
}

void testBackendIndex() {
    //Executes test queries 1. using the index, 2. performing sequential scan


    QueryResult queryResult;
    BoundD bound;
    size_t locAtt = 2; //location attribute for Taxi Dataset
    HashGridIndex *index = loadBackendIndex(queryResult, locAtt, bound);

    Dataset *ds;
    ds = new Dataset(inputPointsFileName.toStdString(), SimplePoint, bound, 0, false);
    ds->openFile();


    // float region_low[3] = {(float) 65239080, (float) -14790345, (float)  1230768000};
    // float region_high[3] = {(float) 65249080, (float) -14690345, (float) 1316469180};

    // float region_low[3] = {(float) 65239180, (float) -14790345, (float)  1230768000};
    // float region_high[3] = {(float) 65249680, (float) -14690345, (float) 1316469180};

    float region_low[3] = {(float) -142011696.000000, (float) -62280992.000000, (float) 0};
    float region_high[3] = {(float) 141815664.000000, (float) 51307656.000000, (float) 0};


    index->queryIndex(region_low, region_high, queryResult);
    std::vector<char> *binAttribResult = queryResult.getAttribute(locAtt);
    uint32_t result_size = binAttribResult->size() / (2 * sizeof(float));
    uint32_t nbResults = 0;
    char *charArray = binAttribResult->data();
    const float *points = reinterpret_cast<float *>(charArray);
    for (uint64_t i = 0; i < result_size * 2; i += 2) {
        if (points[i] >= region_low[0] && points[i] <= region_high[0] &&
            points[i + 1] >= region_low[1] && points[i + 1] <= region_high[1]) {
            nbResults++;
        }
    }
    ds->closeFile();

    ds->openFile();
    qDebug() << "# Coarse Query Reults (Index)" << result_size << "# Filtered (by Location) Query Results" << nbResults
             << "# Query Results (Full Scan)" << executeQueryFullScan(ds, region_low, region_high);


    ds->closeFile();
    delete ds;
}

bool parseArguments(const QMap <QString, QString> &args) {
    QList <QString> keys = args.keys();
    qDebug() << keys;

    if (keys.contains("--help")) {
        return false;
    }


    if (keys.contains("--backendIndexName")) {
        databaseName = args["--backendIndexName"];
    } else {
        return false;
    }

    if (keys.contains("--inputFilename")) {
        inputPointsFileName = args["--inputFilename"];
    } else {
        return false;
    }

    if (keys.contains("--min-long")) {
        min_long = std::stod(args["--min-long"]);
    } else {
        return false;
    }
    if (keys.contains("--max-long")) {
        max_long = std::stod(args["--max-long"]);
    } else {
        return false;
    }

    if (keys.contains("--min-lat")) {
        min_lat = std::stod(args["--min-lat"]);
    } else {
        return false;
    }
    if (keys.contains("--max-lat")) {
        max_lat = std::stod(args["--max-lat"]);
    } else {
        return false;
    }
    return true;
}


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    assert(sizeof(float) == sizeof(int) && sizeof(int) == sizeof(uint32_t));

    QStringList argString = app.arguments();
    QMap <QString, QString> args;
    for (int i = 1; i < argString.size(); i++) {
        QString arg1 = argString[i];
        if (arg1.startsWith("--")) {
            QString arg2 = " ";
            if ((i + 1) < argString.length()) {
                arg2 = argString[i + 1];
            }
            if (arg2.startsWith("--")) {
                args[arg1] = " ";
            } else {
                args[arg1] = arg2;
            }
        }
    }
    if (!parseArguments(args)) {
        cout << "help message: see code!" << "\n";
        return 1;
    }

    createBackendIndex();

    // testBackendIndex();

    exit(0);
    return app.exec();
}

