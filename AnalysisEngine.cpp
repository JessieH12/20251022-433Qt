// AnalysisEngine.cpp  —— 使用qDebug模式输出
#include "AnalysisEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <string>
#include <regex>
#include <chrono>
#include <ctime>
#include "JsonHandler.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QByteArray>
#include <limits>
#include <cstdio>
#include <stdexcept>
#include <QDebug>
#include <QString>

// Point3D 构造函数实现
Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

// 计算与另一点的距离
double Point3D::distanceTo(const Point3D& other) const {
    return std::sqrt((x - other.x) * (x - other.x) +
                     (y - other.y) * (y - other.y) +
                     (z - other.z) * (z - other.z));
}

// AnalysisEngine 构造函数实现
AnalysisEngine::AnalysisEngine() = default;

// 读取STL文件（二进制格式）
void AnalysisEngine::loadStlFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        qDebug() << "无法打开STL文件:" << QString::fromStdString(path);
        throw std::runtime_error(std::string("无法打开STL文件: ") + path);
    }

    // 跳过STL头部（80字节）
    file.seekg(80);

    // 读取三角形数量
    uint32_t numTriangles;
    file.read(reinterpret_cast<char*>(&numTriangles), sizeof(uint32_t));

    std::vector<Point3D> stlPoints;
    stlPoints.reserve(numTriangles * 3); // 每个三角形有3个顶点

    for (uint32_t i = 0; i < numTriangles; ++i) {
        // 读取法向量（跳过）
        float normal[3];
        file.read(reinterpret_cast<char*>(normal), sizeof(float) * 3);

        // 读取三个顶点
        for (int j = 0; j < 3; ++j) {
            float vertex[3];
            file.read(reinterpret_cast<char*>(vertex), sizeof(float) * 3);
            stlPoints.emplace_back(static_cast<double>(vertex[0]),
                                   static_cast<double>(vertex[1]),
                                   static_cast<double>(vertex[2]));
        }

        // 跳过属性字节（2字节）
        file.seekg(2, std::ios::cur);
    }

    qDebug() << "加载STL文件数据:" << stlPoints.size() << "个顶点";

    // 根据当前加载的是理想模型还是测量数据来决定存储到哪个向量
    if (idealModelPoints.empty() && measuredCloudPoints.empty()) {
        idealModelPoints = stlPoints;          // 第一次：理想模型
    } else if (!idealModelPoints.empty() && measuredCloudPoints.empty()) {
        measuredCloudPoints = stlPoints;       // 第二次：测量点云
    } else {
        qDebug() << "点云数据已全部加载，无法再次加载";
        throw std::runtime_error("点云数据已全部加载，无法再次加载");
    }
}

// 读取PCD文件
void AnalysisEngine::loadPcdFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        qDebug() << "无法打开PCD文件:" << QString::fromStdString(path);
        throw std::runtime_error(std::string("无法打开PCD文件: ") + path);
    }

    std::string line;
    int pointCount = 0;
    std::string headerLine;

    // 解析PCD文件头
    while (std::getline(file, headerLine)) {
        if (headerLine.substr(0, 10) == "DATA ascii") {
            break;
        } else if (headerLine.substr(0, 6) == "POINTS") {
            std::istringstream iss(headerLine);
            std::string field;
            iss >> field >> pointCount;
        }
    }

    // 临时存储点云数据
    std::vector<Point3D> tempPoints;
    tempPoints.reserve(pointCount);

    double x, y, z;
    while (file >> x >> y >> z) {
        tempPoints.emplace_back(x, y, z);
        if (pointCount > 0 && tempPoints.size() >= static_cast<size_t>(pointCount)) break;
    }

    qDebug() << "加载PCD点云数据:" << tempPoints.size() << "个点";

    if (idealModelPoints.empty() && measuredCloudPoints.empty()) {
        idealModelPoints = tempPoints;
    } else if (!idealModelPoints.empty() && measuredCloudPoints.empty()) {
        measuredCloudPoints = tempPoints;
    } else {
        qDebug() << "点云数据已全部加载，无法再次加载";
        throw std::runtime_error("点云数据已全部加载，无法再次加载");
    }
}

// 根据文件扩展名自动选择加载方法
void AnalysisEngine::loadFile(const std::string& path) {
    std::string lowerPath = path;
    std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

    if (lowerPath.find(".stl") != std::string::npos) {
        loadStlFile(path);
    } else if (lowerPath.find(".pcd") != std::string::npos) {
        loadPcdFile(path);
    } else {
        qDebug() << "不支持的文件格式:" << QString::fromStdString(path);
        throw std::runtime_error(std::string("不支持的文件格式: ") + path);
    }
}

// 点云配准算法实现 - ICP迭代最近点算法
void AnalysisEngine::performICPRegistration() {
    qDebug() << "执行ICP点云配准...";

    if (idealModelPoints.empty() || measuredCloudPoints.empty()) {
        qDebug() << "错误: 点云数据为空，无法进行配准!";
        return;
    }

    // 将std::vector<Point3D>转换为Eigen矩阵
    Eigen::Matrix3Xd idealMatrix(3, idealModelPoints.size());
    Eigen::Matrix3Xd measuredMatrix(3, measuredCloudPoints.size());

    for (size_t i = 0; i < idealModelPoints.size(); ++i) {
        idealMatrix(0, i) = idealModelPoints[i].x;
        idealMatrix(1, i) = idealModelPoints[i].y;
        idealMatrix(2, i) = idealModelPoints[i].z;
    }
    for (size_t i = 0; i < measuredCloudPoints.size(); ++i) {
        measuredMatrix(0, i) = measuredCloudPoints[i].x;
        measuredMatrix(1, i) = measuredCloudPoints[i].y;
        measuredMatrix(2, i) = measuredCloudPoints[i].z;
    }

    // ICP参数
    const int maxIterations = 2;
    const double tolerance = 1e-6;
    Eigen::Matrix4d transformation = Eigen::Matrix4d::Identity();

    // ICP迭代
    for (int iter = 0; iter < maxIterations; ++iter) {
        // 找到最近点对应关系
        std::vector<std::pair<int, int>> correspondences;
        for (int i = 0; i < measuredMatrix.cols(); ++i) {
            double minDist = std::numeric_limits<double>::max();
            int closestIdx = -1;
            for (int j = 0; j < idealMatrix.cols(); ++j) {
                Eigen::Vector3d diff = measuredMatrix.col(i) - idealMatrix.col(j);
                double dist = diff.norm();
                if (dist < minDist) { minDist = dist; closestIdx = j; }
            }
            if (closestIdx != -1) correspondences.push_back({ i, closestIdx });
        }

        if (correspondences.size() < 3) {
            qDebug() << "错误: 对应点数量不足，无法计算变换!";
            break;
        }

        // 计算质心
        Eigen::Vector3d centroid_measured = Eigen::Vector3d::Zero();
        Eigen::Vector3d centroid_ideal    = Eigen::Vector3d::Zero();
        for (const auto& pair : correspondences) {
            centroid_measured += measuredMatrix.col(pair.first);
            centroid_ideal    += idealMatrix.col(pair.second);
        }
        centroid_measured /= correspondences.size();
        centroid_ideal    /= correspondences.size();

        // 去中心化
        Eigen::Matrix3Xd centered_measured(3, correspondences.size());
        Eigen::Matrix3Xd centered_ideal(3, correspondences.size());
        for (size_t i = 0; i < correspondences.size(); ++i) {
            centered_measured.col(i) = measuredMatrix.col(correspondences[i].first) - centroid_measured;
            centered_ideal.col(i)    = idealMatrix.col(correspondences[i].second)   - centroid_ideal;
        }

        // 计算协方差矩阵
        Eigen::Matrix3d H = centered_measured * centered_ideal.transpose();

        // SVD分解
        Eigen::JacobiSVD<Eigen::Matrix3d> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::Matrix3d U = svd.matrixU();
        Eigen::Matrix3d V = svd.matrixV();

        // 旋转矩阵（处理反射）
        Eigen::Matrix3d R = V * U.transpose();
        if (R.determinant() < 0) {
            Eigen::Matrix3d Vp = V; Vp.col(2) *= -1; R = Vp * U.transpose();
        }

        // 平移向量
        Eigen::Vector3d t = centroid_ideal - R * centroid_measured;

        // 组合、累计变换
        Eigen::Matrix4d currentTransform = Eigen::Matrix4d::Identity();
        currentTransform.block<3,3>(0,0) = R;
        currentTransform.block<3,1>(0,3) = t;
        transformation = currentTransform * transformation;

        // 应用变换
        Eigen::Matrix3Xd transformed_measured(3, measuredMatrix.cols());
        for (int i = 0; i < measuredMatrix.cols(); ++i) {
            transformed_measured.col(i) = R * measuredMatrix.col(i) + t;
        }

        // 误差
        double error = 0.0;
        for (size_t i = 0; i < correspondences.size(); ++i) {
            Eigen::Vector3d diff =
                transformed_measured.col(correspondences[i].first) -
                idealMatrix.col(correspondences[i].second);
            error += diff.squaredNorm();
        }
        error /= correspondences.size();

        qDebug() << "ICP迭代" << (iter + 1) << "，均方误差:" << error;

        if (error < tolerance) {
            qDebug() << "ICP收敛于第" << (iter + 1) << "次迭代";
            break;
        }

        measuredMatrix = transformed_measured;
    }

    // 将变换后的点云数据写回 measuredCloudPoints
    for (size_t i = 0; i < measuredCloudPoints.size(); ++i) {
        Eigen::Vector3d point(measuredCloudPoints[i].x, measuredCloudPoints[i].y, measuredCloudPoints[i].z);
        Eigen::Vector3d tp = transformation.block<3,3>(0,0) * point + transformation.block<3,1>(0,3);
        measuredCloudPoints[i].x = tp.x();
        measuredCloudPoints[i].y = tp.y();
        measuredCloudPoints[i].z = tp.z();
    }

    qDebug() << "ICP配准完成!";
}

// 模拟对齐算法（保留原始方法作为备选）
void AnalysisEngine::alignModelAndCloud() {
    qDebug() << "执行理想模型与测量点云对齐...";

    if (idealModelPoints.empty() || measuredCloudPoints.empty()) return;

    // 计算理想模型中心
    Point3D idealCenter(0, 0, 0);
    for (const auto& v : idealModelPoints) {
        idealCenter.x += v.x; idealCenter.y += v.y; idealCenter.z += v.z;
    }
    idealCenter.x /= idealModelPoints.size();
    idealCenter.y /= idealModelPoints.size();
    idealCenter.z /= idealModelPoints.size();

    // 计算测量点云中心
    Point3D measuredCenter(0, 0, 0);
    for (const auto& p : measuredCloudPoints) {
        measuredCenter.x += p.x; measuredCenter.y += p.y; measuredCenter.z += p.z;
    }
    measuredCenter.x /= measuredCloudPoints.size();
    measuredCenter.y /= measuredCloudPoints.size();
    measuredCenter.z /= measuredCloudPoints.size();

    // 平移
    double tx = idealCenter.x - measuredCenter.x;
    double ty = idealCenter.y - measuredCenter.y;
    double tz = idealCenter.z - measuredCenter.z;
    for (auto& p : measuredCloudPoints) { p.x += tx; p.y += ty; p.z += tz; }

    qDebug() << "几何中心对齐完成，平移向量 (" << tx << "," << ty << "," << tz << ")";
}

// 计算偏差
void AnalysisEngine::computeDeviation() {
    qDebug() << "计算偏差...";

    deviations.clear();
    deviations.reserve(measuredCloudPoints.size());

    for (const auto& measuredPoint : measuredCloudPoints) {
        double minDist = std::numeric_limits<double>::max();
        for (const auto& idealPoint : idealModelPoints) {
            double dist = measuredPoint.distanceTo(idealPoint);
            if (dist < minDist) minDist = dist;
        }
        deviations.push_back(minDist);
    }

    qDebug() << "偏差计算完成，共计算" << deviations.size() << "个偏差值";
}

// 计算统计信息并返回JSON格式字符串
QJsonObject AnalysisEngine::buildStatisticsJson() const {
    QJsonObject obj;
    if (deviations.empty()) return obj;

    double sum = 0.0;
    double maxDev = 0.0;
    int outOfToleranceCount = 0;
    const double tolerance = 0.5; // 偏差阈值

    for (double dev : deviations) {
        sum += dev;
        if (dev > maxDev) maxDev = dev;
        if (dev > tolerance) outOfToleranceCount++;
    }

    double meanDev = sum / deviations.size();
    double outOfTolRatio = static_cast<double>(outOfToleranceCount) / deviations.size();

    obj.insert(QStringLiteral("平均偏差"), meanDev);
    obj.insert(QStringLiteral("最大偏差"), maxDev);
    obj.insert(QStringLiteral("超差比例"), outOfTolRatio);
    obj.insert(QStringLiteral("超差数量"), outOfToleranceCount);
    obj.insert(QStringLiteral("总检测点数量"), static_cast<int>(deviations.size()));
    obj.insert(QStringLiteral("公差阈值"), tolerance);
    obj.insert(QStringLiteral("单位"), QStringLiteral("mm"));
    obj.insert(QStringLiteral("理想模型点数量"), static_cast<int>(idealModelPoints.size()));
    obj.insert(QStringLiteral("实测点云数量"), static_cast<int>(measuredCloudPoints.size()));
    return obj;
}

// 执行分析
void AnalysisEngine::runAnalysis(const std::string& taskId,
                                 const std::string& idealModelPath,
                                 const std::string& measuredCloudPath) {
    this->taskID = taskID;

    //qDebug() << "开始分析任务:" << QString::fromStdString(this->taskID);

    idealModelPoints.clear();
    measuredCloudPoints.clear();

    loadFile(idealModelPath);   // 理想模型
    loadFile(measuredCloudPath); // 测量点云

    performICPRegistration();
    computeDeviation();

    qDebug() << "分析任务完成:";
}

// 生成当前时间戳字符串
std::string AnalysisEngine::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_now);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 保存结果
void AnalysisEngine::saveResult(const std::string& path) {
    if (deviations.empty()) {
        qDebug() << "没有偏差数据可保存";
        return;
    }

    QJsonObject json;
    json["task_ID"]  = QString::fromStdString(taskID);
    json["timestamp"] = QString::fromStdString(getCurrentTimestamp());

    QJsonObject statsObj = buildStatisticsJson();
    json.insert("statistics", statsObj);

    if (JsonHandler::writeJson(QString::fromStdString(path), json)) {
        qDebug() << "分析结果已保存到:" << QString::fromStdString(path);
    } else {
        qDebug() << "保存结果失败:" << QString::fromStdString(path);
    }
}
