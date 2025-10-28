#pragma once
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonValue>
#include<QJsonParseError>
#include<QString>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <JsonHandler.h>
#include <QObject>


class DataFetcher : public QObject
{
    Q_OBJECT
public:
    explicit DataFetcher(QObject *parent = nullptr);
    //task.Json:"taskID":{"modelID","path"(任务文件夹地址),"tolerance"}
    QJsonObject manifest;//manifest.Json:"modelID":{"modelName,"modelPath","cloudPath"}
    QString TaskAddress = "./taskList.json";
    QString ManifestAddress = ":/json_config/manifest.json";
    bool loadManifest(QString path);
    bool createTask(QString path, QString modelID, double tolerance, QString taskID, QString taskName);//需要输入目标文件夹地址path
    bool getDesignPath(QString modelID, QString *designPath);//设计数据
    bool getMeasurePath(QString modelID, QString *measurePath);//测量数据
    QStringList scanTask();//去除无效任务，返回任务列表

private:
    bool generateNewTaskID(QJsonObject taskJson, QString *taskID);

signals:
    // 统一的日志信号（与 TaskDockPanel 的命名保持一致更好复用）
    void logMessageRequested(const QString &module, const QString &message);
};

/*将resources/json.qrc/json_config/manifest.json中的地址改为自己电脑上文件的绝对地址
 * 使用示例
int main(int argc, char* argv[])
{
    DataFetcher tester =  DataFetcher();
    QStringList keyList = tester.scanTask();
    tester.loadManifest(tester.ManifestAddress);
    QString TaskID = "T1202";
    QString TaskName = "aaa";
    tester.createTask("./test","1",10,TaskID,TaskName);
    return 0;
}
*/
