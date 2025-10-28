#include "reportgenerator.h"

#include "JsonHandler.h"

const QString title = "检测报告";
const QString titleFont = "宋体";
const QString articleFont = "宋体";
const QString taskId = "task_id";
const QString analysisType = "analysis_type";
const QString analysisTimestamp = "analysis_timestamp";

const QString statistics = "statistics";

const int CLOUD_COMPARE_ITEM_COUNT = 9;
enum class CloudCompareItem {
    Unit,
    ToleranceThreshold,
    RealPointNum,
    MeanDeviation,
    TotalPointNum,
    MaxDeviation,
    IdealPointNum,
    ExcessNum,
    ExcessRatio
};

// 将枚举映射为对应的显示字符串
QString cloudCompareItemToDisplayString(CloudCompareItem item) {
    switch (item) {
    case CloudCompareItem::Unit:               return "单位";
    case CloudCompareItem::ToleranceThreshold: return "公差阈值";
    case CloudCompareItem::RealPointNum:       return "实测点云数量";
    case CloudCompareItem::MeanDeviation:      return "平均偏差";
    case CloudCompareItem::TotalPointNum:      return "总检测点数量";
    case CloudCompareItem::MaxDeviation:       return "最大偏差";
    case CloudCompareItem::IdealPointNum:      return "理想模型点数量";
    case CloudCompareItem::ExcessNum:          return "超差数量";
    case CloudCompareItem::ExcessRatio:        return "超差比例"; // 超过75就失败
    default:                                   return "Unknown";
    }
}


ReportGenerator::ReportGenerator(QString reportPath) : reportPath(reportPath), resultData(QJsonObject())
{
}
bool ReportGenerator::generatePDF(QString path)
{
    //通过taskID找到result.json
    QString resultPath = path;
    //通过result.json生成resultData对象
    loadResult(resultPath);


    QPdfWriter pdfWriter(reportPath);
    pdfWriter.setPageSize(QPagedPaintDevice::A4);
    pdfWriter.setResolution(300);
    pdfWriter.setTitle("测试报告");
    pdfWriter.setCreator("ReportGenerator v1.0");

    paintPdf(&pdfWriter);
    return false;
}



void ReportGenerator::paintPdf(QPagedPaintDevice* device)
{
    QJsonObject statisticsJsonO = this->resultData[statistics].toObject();
    QPainter* pPainter = new QPainter(device);
    int nPdfWidth = pPainter->viewport().width();
    int nPdfHeight = pPainter->viewport().height();

    //绘制标题
    int y = 10;
    pPainter->setFont(QFont(titleFont, 18, 36));
    pPainter->drawText(QRect(0, y, nPdfWidth, 100), Qt::AlignCenter, title);

    //绘制报告相关信息
    y += 200;
    pPainter->setFont(QFont(titleFont, 12, 20));
    int nLineSpace = 100;
    int nLineHeight = 60;
    pPainter->drawText(QRect(100, y, nPdfWidth, nLineHeight), Qt::AlignLeft, QString("任务ID：%1").arg(this->resultData[taskId].toString()));
    y += nLineSpace;
    pPainter->drawText(QRect(100, y, nPdfWidth, nLineHeight), Qt::AlignLeft, QString("分析类型：%1").arg(this->resultData[analysisType].toString()));
    y += nLineSpace;
    pPainter->drawText(QRect(100, y, nPdfWidth, nLineHeight), Qt::AlignLeft, QString("分析时间：%1").arg(this->resultData[analysisTimestamp].toString()));
    y += nLineSpace;


    //绘制每个操作记录的各项标题 （序号:测量项目：测量值 = 1：2：2）
    y += 150;
    pPainter->setFont(QFont(articleFont, 14, 28));
    int nUnitW = nPdfWidth / (1 + 2 + 2);

    int nNoWidth = nUnitW * 1;

    int nVary = nNoWidth;
    int nVaryWidth = nUnitW * 2;
    int nValue = nVary + nVaryWidth;
    int nValueWidth = nUnitW * 2;

    nLineHeight = 80;
    pPainter->drawText(QRect(0, y, nNoWidth, nLineHeight), Qt::AlignCenter, "序号");
    pPainter->drawText(QRect(nVary, y, nVaryWidth, nLineHeight), Qt::AlignCenter, "测量项目");
    //    pPainter->drawText(QRect(nExplainX, y, nExplainWidth, nLineHeight), Qt::AlignCenter, "操作说明");
    pPainter->drawText(QRect(nValue, y, nValueWidth, nLineHeight), Qt::AlignCenter, "测量值");

    //绘制一条分隔线
    y += 100;
    pPainter->setPen(QPen(QBrush(QColor(144, 166, 188)), 8));
    pPainter->drawLine(QLineF(0, y, nPdfWidth, y));


    qDebug() << statisticsJsonO;
    //绘制染色操作记录,假设此刻有25条记录需要绘制
    for (int i = 0; i < CLOUD_COMPARE_ITEM_COUNT; ++i)
    {
        CloudCompareItem item = static_cast<CloudCompareItem>(i);
        //判断是否应该另起一页(320:根据计算,绘制每条记录大概需要320点）
        if (y + 320 >= nPdfHeight)
        {
            device->newPage();
            y = 10;
        }

        y += 50;
        pPainter->setFont(QFont("宋体", 12, 20));
        pPainter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));

        nLineSpace = 80;
        nLineHeight = 50;

        pPainter->drawText(QRect(0, y, nNoWidth, nLineHeight), Qt::AlignCenter, QString("%1").arg(i + 1));
        pPainter->drawText(QRect(nVary, y, nVaryWidth, nLineHeight), Qt::AlignCenter, cloudCompareItemToDisplayString(item));
        if (statisticsJsonO[cloudCompareItemToDisplayString(item)].isString()) {
            pPainter->drawText(QRect(nValue, y, nValueWidth, nLineHeight), Qt::AlignCenter, statisticsJsonO[cloudCompareItemToDisplayString(item)].toString());
        }
        else pPainter->drawText(QRect(nValue, y, nValueWidth, nLineHeight), Qt::AlignCenter, QString::number(statisticsJsonO[cloudCompareItemToDisplayString(item)].toDouble()));

        y += 70;
        pPainter->setPen(QPen(QBrush(QColor(166, 188, 222)), 3));
        pPainter->drawLine(QLineF(0, y, nPdfWidth, y));
    }

    delete pPainter;
}



bool ReportGenerator::loadResult(QString path)
{
    JsonHandler::readJson(path, &this->resultData);
    return true;
}

bool ReportGenerator::checkTolerance(double tol)
{
    return false;
}
