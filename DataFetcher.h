#pragma once
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
#include <JsonHandler.h>



class DataFetcher {
public://task.Json:"taskID":{"modelID","path"(任务文件夹地址),"tolerance"}
    QJsonObject manifest;//manifest.Json:"modelID":{"modelName,"modelPath","cloudPath"}
    QString TaskAddress = "./taskList.json";
    QString ManifestAddress = ":/json_config/manifest.json";
    bool loadManifest(QString path);
    bool createTask(QString path, QString modelID, double tolerance, QString *taskID);//需要输入目标文件夹地址path
    bool getDesignPath(QString modelID, QString *designPath);//设计数据
    bool getMeasurePath(QString modelID, QString *measurePath);//测量数据
private:
    bool generateNewTaskID(QJsonObject taskJson, QString *taskID);
};

