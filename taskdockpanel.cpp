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
    QString tpl = ui->comboTemplate->currentText();
    QString tid = makeTaskId();
    QApplication::clipboard()->setText(tid);

    updateCurrentTask(tpl, tid);
    emit taskApplied(tpl, tid);

    QMessageBox::information(this, "任务申请成功",
                             QString("样板：%1\n任务ID：%2").arg(tpl, tid));

    // 在主窗口日志输出中显示
    emit logMessageRequested("校样功能",
                             QString("任务申请成功，任务ID：%1").arg(tid));
}

//更新当前任务状态
void TaskDockPanel::updateCurrentTask(const QString &tpl, const QString &tid) {
    ui->labelCurTaskId->setText("任务ID：" + tid);
    ui->labelCurTemplate->setText("样板类型：" + tpl);
    ui->labelCurStatus->setText("状态：已申请");
}

//显示历史任务
void TaskDockPanel::onRefreshHistory() {
    ui->listHistory->clear();
    QString baseDir = QDir::currentPath() + "/tasks";
    qDebug() << "任务目录路径：" << baseDir;

    emit logMessageRequested("校样功能",
                             QString("任务目录路径：%1").arg(baseDir));

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
    emit logMessageRequested("校样功能",
                             QString("历史任务显示完成"));
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

    double mean = obj["meanError"].toDouble();
    double maxE = obj["maxError"].toDouble();
    int total = obj["pointCount"].toInt();
    QJsonArray points = obj["points"].toArray();

    int exceed = 0;
    for (auto v : points)
        if (v.toObject()["distance"].toDouble() > 0.5) exceed++;
    double passRate = total ? 100.0 * (1.0 - (double)exceed / total) : 0.0;

    ui->labelMeanErr->setText("平均误差：" + QString::number(mean, 'f', 3));
    ui->labelMaxErr->setText("最大误差：" + QString::number(maxE, 'f', 3));
    ui->labelPassRate->setText("合格率：" + QString::number(passRate, 'f', 2) + "%");
}
