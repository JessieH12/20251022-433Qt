#pragma once
#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H


#include<qstring.h>
#include<qjsonobject.h>
#include <QApplication>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QDateTime>
#include <QMarginsF>

class ReportGenerator {
private:
    //报告生成路径
    QString reportPath;
    //结果数据
    QJsonObject resultData;

    void paintPdf(QPagedPaintDevice* device);

public:
    ReportGenerator(QString reportPath);
    //生成pdf
    bool generatePDF(QString taskID);
    //读取result.json
    bool loadResult(QString path);
    //检查数据
    bool checkTolerance(double tol);

};


#endif // REPORTGENERATOR_H
