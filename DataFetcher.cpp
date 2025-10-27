#include "DataFetcher.h"

DataFetcher::DataFetcher(QObject *parent)
    : QObject(parent)
{}

bool DataFetcher::loadManifest(QString path){
    bool res = JsonHandler().readJson(path, &this->manifest);
    return res;
}

bool DataFetcher::createTask(QString path, QString modelID, double tolerance, QString taskID)
{
    emit logMessageRequested("校样功能",
                                 QString("任务 %1 开始创建（模型ID=%2，公差=%3）")
                                 .arg(taskID, modelID).arg(tolerance));

    QJsonObject taskJson = QJsonObject();
    QJsonObject isoTaskJson;
    QString taskPath = path + "/" + taskID;
    QDir dir(taskPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    bool res = JsonHandler().readJson(this->TaskAddress, &taskJson);
    QString designPath, measurePath;
    if (this->getDesignPath(modelID, &designPath) && this->getMeasurePath(modelID, &measurePath)) {
        bool designExists  = QFile::exists(designPath);
        bool measureExists = QFile::exists(measurePath);

        if (!designExists || !measureExists) {
            if (!designExists)
                emit logMessageRequested("校样功能",
                                         QString("❌ 理想模型文件不存在：%1").arg(designPath));
            if (!measureExists)
                emit logMessageRequested("校样功能",
                                         QString("❌ 实测模型文件不存在：%1").arg(measurePath));

            emit logMessageRequested("校样功能", "❌ 任务创建终止：缺少模型文件。");
            return false;
        }

        QString measureFileName = QFileInfo(measurePath).fileName();
        QString designFileName = QFileInfo(designPath).fileName();

        isoTaskJson["taskID"] = taskID;
        isoTaskJson["modelID"] = modelID;
        isoTaskJson["path"] = taskPath;
        isoTaskJson["tolerance"] = tolerance;
        isoTaskJson["measureName"] = measureFileName;
        isoTaskJson["designName"] = designFileName;
        taskJson[taskID] = isoTaskJson;
        JsonHandler().writeJson(this->TaskAddress, taskJson);
        JsonHandler().writeJson(taskPath+"/task.json", isoTaskJson);
        emit logMessageRequested("校样功能", "已写入任务元信息文件 task.json");

        QFile::copy(measurePath, taskPath + "/" + measureFileName);
        QFile::copy(designPath, taskPath + "/" + designFileName);
        emit logMessageRequested("校样功能",
                                     QString("模型文件已拷贝到任务目录：%1").arg(taskPath));

        emit logMessageRequested("校样功能",
                                     QString("任务 %1 创建完成 ✅").arg(taskID));
        return true;
    }
    else {
        emit logMessageRequested("校样功能",
                                         QString("❌ manifest 中未找到模型 %1 的路径信息").arg(modelID));
        return false;
    }

}

QStringList DataFetcher::scanTask() {
    QJsonObject taskJson = QJsonObject();
    QStringList keyList = {};
    bool hasChange = false;
    if(JsonHandler().readJson(this->TaskAddress, &taskJson)){
        keyList = taskJson.keys();
        for (int i = 0;i < keyList.length();i++){
            QJsonObject readJson = taskJson[keyList[i]].toObject();
            if(!(QFile::exists(readJson["path"].toString()+"/"+readJson["designName"].toString())&&QFile::exists(readJson["path"].toString()+"/"+readJson["measureName"].toString())&&QFile::exists(readJson["path"].toString()+"/task.json"))){
                taskJson.remove(keyList[i]);
                hasChange = true;
            }
        }
        if (hasChange){
            JsonHandler().writeJson(this->TaskAddress, taskJson);
            keyList = taskJson.keys();
        }

    }
    return keyList;
}

bool DataFetcher::getDesignPath(QString modelID, QString *measurePath) {
    if (!this->manifest.contains(modelID)) {
//        *measurePath = "不存在该模型的信息";
        return false;
    }
    *measurePath = manifest[modelID].toObject()["designPath"].toString();
    return true;
}

bool DataFetcher::getMeasurePath(QString modelID, QString *measurePath) {
    if (!this->manifest.contains(modelID)) {
//        *measurePath = "不存在该模型的信息";
        return false;
    }
    *measurePath = manifest[modelID].toObject()["measurePath"].toString();
    return true;
}

bool DataFetcher::generateNewTaskID(QJsonObject taskJson, QString *taskID) {
    QSet<QString> existingIDs;

    for (auto it = taskJson.constBegin(); it != taskJson.constEnd(); ++it) {
        QJsonObject task = it.value().toObject();
        if (task.contains("taskID") && task["taskID"].isString()) {
            existingIDs.insert(task["taskID"].toString());
        }
    }

    int newNumber = 1;
    QString newID;

    do {
        newID = QString("T%1").arg(newNumber, 3, 10, QChar('0'));
        newNumber++;
        if (newNumber > 999) {
            qWarning() << "TaskID 数量已达到上限";
            return false;
        }
    } while (existingIDs.contains(newID));
    *taskID = newID;
    return true;
}

