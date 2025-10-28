#ifndef ANALYSIS_ENGINE_H
#define ANALYSIS_ENGINE_H

#include <vector>
#include <string>
#include <QJsonObject>
#include <QObject>

// 3D点结构体
struct Point3D {
    double x, y, z;
    Point3D(double x = 0, double y = 0, double z = 0);
    double distanceTo(const Point3D& other) const;
};

class AnalysisEngine : public QObject {
    Q_OBJECT
private:
    std::string taskID;
    std::vector<Point3D> idealModelPoints;    // 理想设计模型点
    std::vector<Point3D> measuredCloudPoints; // 测量点云数据
    std::vector<double> deviations;           // 计算得到的偏差值

    void loadPcdFile(const std::string& path);
    void loadStlFile(const std::string& path);
    void loadFile(const std::string& path);  // 自动根据文件扩展名选择加载方法
    void alignModelAndCloud();
    void computeDeviation();
    std::string getCurrentTimestamp();       // 获取当前时间戳
    QJsonObject buildStatisticsJson() const;

public:
    explicit AnalysisEngine(QObject *parent = nullptr);
    AnalysisEngine();
    void runAnalysis(const std::string& taskId,
        const std::string& idealModelPath,
        const std::string& measuredCloudPath);

    //结果返回json
    void saveResult(const std::string& path);

    // 点云配准方法
    void performICPRegistration();
signals:
    // 统一的日志信号（与 TaskDockPanel 的命名保持一致更好复用）
    void logMessageRequested(const QString &module, const QString &message);
};

#endif // ANALYSIS_ENGINE_H

//使用示例
//int main(int argc, char* argv[])
//{
//    QApplication app(argc, argv);

//    // 读取JSON配置文件
//    QString jsonPath = "D:/433Qt/433/433tester/task.json"; // JSON文件路径
//    QJsonObject jsonObj;
//    if (!JsonHandler::readJson(jsonPath, &jsonObj)) {
//        qDebug() << "无法读取JSON文件:" << jsonPath;
//        return 1;
//    }

//    // 从JSON中读取数据
//    QString taskID = jsonObj["taskID"].toString();
//    QString taskPath = jsonObj["path"].toString();
//    QString designName = jsonObj["designName"].toString();   // 点云文件名
//    QString measureName = jsonObj["measureName"].toString(); // 模型文件名

//    // 拼接路径
//    QString measuredCloudPath = taskPath + "/" + designName;  // 点云文件路径
//    QString idealModelPath = taskPath + "/" + measureName;    // 模型文件路径

//    qDebug() << "开始分析任务:" << taskID;
//    qDebug() << "理想模型路径:" << idealModelPath;
//    qDebug() << "测量点云路径:" << measuredCloudPath;

//    // 执行分析
//    AnalysisEngine engine;
//    engine.runAnalysis(taskID.toStdString(),
//                       idealModelPath.toStdString(),
//                       measuredCloudPath.toStdString());

//    // 保存结果
//    QString resultPath = taskPath + "/result.json";
//    engine.saveResult(resultPath.toStdString());

//    return 0;
//}
