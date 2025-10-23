#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#endif // JSONHANDLER_H
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonValue>
#include<QJsonParseError>
#include<QString>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

class JsonHandler {
public:
    static bool readJson(QString Path, QJsonObject *JsonData);
    static bool writeJson(QString Path, QJsonObject JsonData);

};
