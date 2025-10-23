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
public://task.Json:"taskID":{"modelID","path"(�����ļ��е�ַ),"tolerance"}
    QJsonObject manifest;//manifest.Json:"modelID":{"modelName,"modelPath","cloudPath"}
    QString TaskAddress = "./taskList.json";
    QString ManifestAddress = ":/json_config/manifest.json";
    bool loadManifest(QString path);
    bool createTask(QString path, QString modelID, double tolerance, QString *taskID);//��Ҫ����Ŀ���ļ��е�ַpath
    bool getDesignPath(QString modelID, QString *designPath);//�������
    bool getMeasurePath(QString modelID, QString *measurePath);//��������
private:
    bool generateNewTaskID(QJsonObject taskJson, QString *taskID);
};

