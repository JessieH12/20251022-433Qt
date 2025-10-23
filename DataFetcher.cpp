#include<DataFetcher.h>

bool DataFetcher::loadManifest(QString path){
    bool res = JsonHandler().readJson(path, &this->manifest);
    return res;
}

bool DataFetcher::createTask(QString path, QString modelID, double tolerance, QString *taskID) {
    QJsonObject taskJson = QJsonObject();
    QJsonObject isoTaskJson;
    bool res = JsonHandler().readJson(this->TaskAddress, &taskJson);
    QString designPath, measurePath;
    if (this->getDesignPath(modelID, &designPath) && this->getMeasurePath(modelID, &measurePath)) {
        if (this->generateNewTaskID(taskJson, taskID)) {
            isoTaskJson["taskID"] = *taskID;
            isoTaskJson["modelID"] = modelID;
            isoTaskJson["path"] = path;
            isoTaskJson["tolerance"] = tolerance;
            taskJson[*taskID] = isoTaskJson;
            JsonHandler().writeJson(this->TaskAddress, taskJson);
            JsonHandler().writeJson(path+"/task.json", isoTaskJson);
            QString measureFileName = QFileInfo(measurePath).fileName();
            QString designFileName = QFileInfo(designPath).fileName();
            QFile::copy(measurePath, path + "/" + measureFileName);
            QFile::copy(designPath, path + "/" + designFileName);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        qWarning() << "存在错误的文件地址";
        return false;
    }

}

bool DataFetcher::getDesignPath(QString modelID, QString *measurePath) {
    if (!this->manifest.contains(modelID)) {
        *measurePath = "不存在该模型的信息";
        return false;
    }
    *measurePath = manifest[modelID].toObject()["designPath"].toString();
    return true;
}

bool DataFetcher::getMeasurePath(QString modelID, QString *measurePath) {
    if (!this->manifest.contains(modelID)) {
        *measurePath = "不存在该模型的信息";
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

