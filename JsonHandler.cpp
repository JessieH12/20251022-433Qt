#include<JsonHandler.h>


bool JsonHandler::readJson(QString Path, QJsonObject *JsonData) {
    QFile file(Path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON 解析错误:" << parseError.errorString();
        return false;
    }
    *JsonData = doc.object();
    return true;
}

bool JsonHandler::writeJson(QString Path, QJsonObject JsonData) {
    QFile file(Path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QJsonDocument doc(JsonData);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
