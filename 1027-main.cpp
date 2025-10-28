#include "pclvisualizer.h"
#include "AnalysisEngine.h"
#include "DataFetcher.h"
#include "reportgenerator.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDir>
#include <QDebug>
#include <windows.h>
#include <iostream>
#include <QTextCodec>
int main(int argc, char* argv[])
{

    QApplication app(argc, argv);

    try {
        // 初始化 DataFetcher
        DataFetcher dataFetcher;

        // 加载 manifest 文件
        if (!dataFetcher.loadManifest(dataFetcher.ManifestAddress)) {
            qCritical() << "无法加载 manifest 文件";
            return 1;
        }

        // 扫描并清理无效任务
        QStringList taskList = dataFetcher.scanTask();
        qInfo().noquote() << "当前有效任务数量:" << taskList.size();

        // 创建新任务
        QString taskPath = "D:\\433Qt\\433\\433tester";
        QString modelID = "1";          // 模型ID
        double tolerance = 0.5;         // 默认公差
        QString taskID;

        if (!dataFetcher.createTask(taskPath, modelID, tolerance, taskID)) {
            qCritical() << "无法创建任务";
            return 1;
        }

        qInfo().noquote() << "成功创建任务，任务ID:" << taskID;

        // 读取 task.json
        QString taskJsonPath = taskPath + "/task.json";
        QFile taskFile(taskJsonPath);
        if (!taskFile.open(QIODevice::ReadOnly)) {
            qCritical().noquote() << "无法打开 task.json:" << taskJsonPath;
            return 1;
        }

        QByteArray taskData = taskFile.readAll();
        taskFile.close();

        QJsonParseError parseError;
        QJsonDocument taskDoc = QJsonDocument::fromJson(taskData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qCritical().noquote() << "解析 task.json 失败:" << parseError.errorString();
            return 1;
        }

        QJsonObject taskJson = taskDoc.object();

        // 任务参数
        QString modelIDFromJson = taskJson["modelID"].toString();
        QString designName      = taskJson["designName"].toString();
        QString measureName     = taskJson["measureName"].toString();
        QString taskPathFromJson = taskJson["path"].toString();

        // 拼接路径
        QString idealModelPath   = taskPathFromJson + "/" + designName;
        QString measuredCloudPath = taskPathFromJson + "/" + measureName;

        qInfo().noquote() << "理想模型路径:" << idealModelPath;
        qInfo().noquote() << "测量点云路径:" << measuredCloudPath;

        // 执行分析
        AnalysisEngine engine;
        engine.runAnalysis(taskID.toStdString(),
                           idealModelPath.toStdString(),
                           measuredCloudPath.toStdString());

        // 保存结果
        QString resultPath = taskPathFromJson + "/result.json";
        engine.saveResult(resultPath.toStdString());

        qInfo().noquote() << "分析任务完成，结果已保存到:" << resultPath;

        // 生成报告
        ReportGenerator rg(QStringLiteral("D:/433Qt/433/433tester/output.pdf"));
        rg.generatePDF(QStringLiteral("D:/433Qt/433/433tester/result.json"));
        qInfo() << "📄 报告已生成: D:/433Qt/433/433tester/output.pdf";
    }
    catch (const std::exception &e) {
        qCritical().noquote() << "程序运行出错:" << e.what();
        return 1;
    }

    return 0;
}
