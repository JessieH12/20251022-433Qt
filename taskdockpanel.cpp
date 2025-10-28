#include "taskdockpanel.h"
#include "ui_taskdockpanel.h"
#include <QDateTime>
#include <QMessageBox>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include "DataFetcher.h"
#include "AnalysisEngine.h"
#include "reportgenerator.h"


TaskDockPanel::TaskDockPanel(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TaskDockPanel)
{
    ui->setupUi(this);

    // 设置默认字体和背景风格
    setStyleSheet(R"(
        QWidget { font-family: "Microsoft YaHei"; }
        QPushButton {
            background-color: #0078D7; color: white;
            border-radius: 4px; padding: 6px 12px; font-weight: bold;
        }
        QPushButton:hover { background-color: #2894E0; }
        QListWidget {
            border: 1px solid #BFBFBF; border-radius: 4px;
            selection-background-color: #0078D7;
            selection-color: white;
        }
    )");

    // 信号槽连接
    connect(ui->btnApply, &QPushButton::clicked, this, &TaskDockPanel::onApplyTask); //申请数据并校样
    connect(ui->btnRefresh, &QPushButton::clicked, this, &TaskDockPanel::onRefreshHistory); //刷新列表
    connect(ui->btnView, &QPushButton::clicked, this, &TaskDockPanel::onViewResult); //查看结果
    connect(ui->listHistory, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item) {
        emit historyTaskSelected(item->text());
    });

    //自动监视 tasks 目录变化并刷新
    QString tasksDir = QDir::currentPath() + "/tasks";
    QDir().mkpath(tasksDir);  // 确保 tasks 目录存在

    watcher = new QFileSystemWatcher(this);
    watcher->addPath(tasksDir);

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [=](const QString &path) {
        emit logMessageRequested("校样功能", QString("检测到任务目录变动：%1").arg(path));
        onRefreshHistory();
    });

    emit logMessageRequested("校样功能", QString("任务目录监视已启动：%1").arg(tasksDir));

    // 程序启动后立即刷新一次历史任务列表
    QTimer::singleShot(0, this, &TaskDockPanel::onRefreshHistory);
//    onRefreshHistory();
}

TaskDockPanel::~TaskDockPanel()
{
    delete ui;
}

//自动生成任务id
QString TaskDockPanel::makeTaskId() const {
    return QString("T%1").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
}

//显示任务申请成功，开始运行校样
void TaskDockPanel::onApplyTask() {
/*    QString tpl = ui->comboTemplate->currentText();
    QString tid = makeTaskId();
    QApplication::clipboard()->setText(tid);

    updateCurrentTask(tpl, tid);
    emit taskApplied(tpl, tid);

    QMessageBox::information(this, "任务申请成功",
                             QString("样板：%1\n任务ID：%2").arg(tpl, tid));

    // 在主窗口日志输出中显示
    emit logMessageRequested("校样功能",
                             QString("任务申请成功，任务ID：%1").arg(tid));*/

    QString taskType = ui->comboTemplate->currentText(); //样板类型
    QString taskID = makeTaskId();  // 自动生成任务ID

    QApplication::clipboard()->setText(taskID);

    updateCurrentTask(taskType, taskID);
    emit taskApplied(taskType, taskID);

//    taskId += "_" + taskType;

    QString basePath = QDir::currentPath() + "/tasks";
    QString taskPath = basePath + "/" + taskID;
    // 确保任务目录存在
    QDir().mkpath(taskPath);

//=====DataFetcher模块=================================================================================================

    // 创建 DataFetcher 实例
    DataFetcher *data_fetcher = new DataFetcher(this);

    connect(data_fetcher, &DataFetcher::logMessageRequested,
            this,    &TaskDockPanel::logMessageRequested);

    QStringList keyList = data_fetcher->scanTask();
    data_fetcher->loadManifest(data_fetcher->ManifestAddress);

    // createTask(保存路径, 模型id, 精度, 任务id指针,任务名称)
    QString taskName = taskType;
    data_fetcher->createTask(basePath, "1", 10, taskID, taskName);

    // 输出结果提示
    QMessageBox::information(this, "任务创建完成",
                             QString("任务 %1 已创建！").arg(taskID));
//=====DataFetcher模块=================================================================================================

//=====AnalysisEngine模块=================================================================================================

    AnalysisEngine *analysis_engine = new AnalysisEngine(this);

    connect(analysis_engine, &AnalysisEngine::logMessageRequested,
            this,    &TaskDockPanel::logMessageRequested);

    emit logMessageRequested("校样功能", QString("校样分析开始"));

    QString jsonPath = taskPath + "/task.json"; // JSON文件路径
    QJsonObject jsonObj;
    if (!JsonHandler::readJson(jsonPath, &jsonObj)) {
        qDebug() << "无法读取JSON文件:" << jsonPath;
        emit logMessageRequested("校样功能",
                                     QString("❌ 任务：%1 无法读取JSON文件").arg(taskID));
    }

    // 从JSON中读取数据
    taskID = jsonObj["taskID"].toString();
    qDebug() << "taskID: " + taskID;
    taskPath = jsonObj["path"].toString();
    QString designName = jsonObj["designName"].toString();   // 点云文件名
    QString measureName = jsonObj["measureName"].toString(); // 模型文件名

    // 拼接路径
    QString measuredCloudPath = taskPath + "/" + designName;  // 点云文件路径
    QString idealModelPath = taskPath + "/" + measureName;    // 模型文件路径

//    qDebug() << "开始分析任务:" << taskID;
//    qDebug() << "理想模型路径:" << idealModelPath;
//    qDebug() << "测量点云路径:" << measuredCloudPath;

    emit logMessageRequested("校样功能",
                                 QString("开始分析任务: %1").arg(taskID));
    emit logMessageRequested("校样功能",
                                 QString("理想模型路径: %1").arg(idealModelPath));
    emit logMessageRequested("校样功能",
                                 QString("测量点云路径: %1").arg(measuredCloudPath));

    // 执行分析
//    AnalysisEngine engine;
    analysis_engine->runAnalysis(taskID.toStdString(),
                       idealModelPath.toStdString(),
                       measuredCloudPath.toStdString());

    // 保存结果
    QString resultPath = taskPath + "/result.json";
    analysis_engine->saveResult(resultPath.toStdString());
//=====AnalysisEngine模块=================================================================================================

//====ReportGenerator模块=================================================================================================
    ReportGenerator rg = ReportGenerator( taskPath +"/output.pdf");
    rg.generatePDF(taskPath+"/result.json");
//====ReportGenerator模块=================================================================================================
}

//更新当前任务状态
void TaskDockPanel::updateCurrentTask(const QString &taskType, const QString &taskId) {
    ui->labelCurTaskId->setText("任务ID：" + taskId);
    ui->labelCurTemplate->setText("样板类型：" + taskType);
    ui->labelCurStatus->setText("状态：已申请");
}

//显示历史任务
void TaskDockPanel::onRefreshHistory() {
    ui->listHistory->clear();
    QString baseDir = QDir::currentPath() + "/tasks";

//    emit logMessageRequested("校样功能",
//                             QString("任务目录路径：%1").arg(baseDir));

    QDir dir(baseDir);
    if (!dir.exists()) {
        dir.mkpath(baseDir);
        return;
    }

    QStringList taskDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (taskDirs.isEmpty()) {
        ui->listHistory->addItem("暂无历史任务");
        // 在主窗口日志输出中显示
        emit logMessageRequested("校样功能",
                                 QString("暂无历史任务"));
        return;
    }

    for (const QString &taskName : taskDirs) {
        QString taskPath = baseDir + "/" + taskName;
        QFile resultFile(taskPath + "/result.json");
        QString status = resultFile.exists() ? "已完成" : "进行中";
        ui->listHistory->addItem(QString("%1（%2）").arg(taskName, status));
    }

    // 在主窗口日志输出中显示
//    emit logMessageRequested("校样功能",
//                             QString("历史任务显示完成"));
}

//显示结果
void TaskDockPanel::onViewResult() {
    QListWidgetItem *item = ui->listHistory->currentItem();
    if (!item) return;

    QString text = item->text();
    QString tid = text.split("（").first().trimmed();
    QString baseDir = QDir::currentPath() + "/tasks/" + tid;
    showTaskResult(baseDir);
}

//显示任务校样结果
void TaskDockPanel::showTaskResult(const QString &taskDir) {
    QFile f(QDir(taskDir).filePath("result.json"));
    if (!f.exists()) {
        QMessageBox::warning(this, "提示", "该任务尚无结果文件！");
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) return;

    QJsonObject obj = QJsonDocument::fromJson(f.readAll()).object();
    f.close();

    double mean = obj["平均偏差"].toDouble();
    double maxE = obj["最大偏差"].toDouble();
    double rate = obj["超差比例"].toDouble();
//    QJsonArray points = obj["points"].toArray();

//    int exceed = 0;
//    for (auto v : points)
//        if (v.toObject()["distance"].toDouble() > 0.5) exceed++;
//    double passRate = total ? 100.0 * (1.0 - (double)exceed / total) : 0.0;

    ui->labelMeanErr->setText("平均偏差：" + QString::number(mean, 'f', 3));
    ui->labelMaxErr->setText("最大偏差：" + QString::number(maxE, 'f', 3));
    ui->labelPassRate->setText("超差比例" + QString::number(rate * 100, 'f', 2) + "%");
}
