#ifndef ANALYSIS_ENGINE_H
#define ANALYSIS_ENGINE_H

#include <vector>
#include <string>
#include <QJsonObject>

// 3D点结构体
struct Point3D {
    double x, y, z;
    Point3D(double x = 0, double y = 0, double z = 0);
    double distanceTo(const Point3D& other) const;
};

class AnalysisEngine {
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
    AnalysisEngine();
    void runAnalysis(const std::string& taskId,
        const std::string& idealModelPath,
        const std::string& measuredCloudPath);

    //结果返回json
    void saveResult(const std::string& path);

    // 点云配准方法
    void performICPRegistration();
};

#endif // ANALYSIS_ENGINE_H
