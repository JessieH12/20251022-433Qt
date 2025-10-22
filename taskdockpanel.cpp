#include "taskdockpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QDateTime>
#include <QMessageBox>
#include <QClipboard>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QApplication>

TaskDockPanel::TaskDockPanel(QWidget *parent)
    : QWidget(parent)
{
    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);

    // ============ 任务申请 ============
    QLabel *titleApply = new QLabel(QStringLiteral("任务申请"));
    titleApply->setStyleSheet("font-weight:bold; font-size:12pt;");
    mainLayout->addWidget(titleApply);

    comboTemplate = new QComboBox();
    comboTemplate->addItems({
        QStringLiteral("A型样板"),
        QStringLiteral("B型样板"),
        QStringLiteral("C型样板")
    });
    mainLayout->addWidget(new QLabel(QStringLiteral("样板类型：")));
    mainLayout->addWidget(comboTemplate);

//    editTaskId = new QLineEdit();
//    editTaskId->setPlaceholderText(QStringLiteral("任务ID将自动生成"));
//    editTaskId->setReadOnly(true);
//    mainLayout->addWidget(editTaskId);

    // 按钮：申请校样任务
    QHBoxLayout *btnLayout1 = new QHBoxLayout();
    QPushButton *btnApply = new QPushButton(QStringLiteral("申请校样"));
    btnLayout1->addWidget(btnApply);
    mainLayout->addLayout(btnLayout1);

    connect(btnApply, &QPushButton::clicked, this, &TaskDockPanel::onApplyTask);

    // ============ 当前任务 ============
    QLabel *titleCur = new QLabel(QStringLiteral("当前任务"));
    titleCur->setStyleSheet("font-weight:bold; font-size:12pt;");
    mainLayout->addWidget(titleCur);

    labelCurTaskId = new QLabel(QStringLiteral("任务ID：—"));
    labelCurTemplate = new QLabel(QStringLiteral("样板类型：—"));
    labelCurStatus = new QLabel(QStringLiteral("状态：—"));
    mainLayout->addWidget(labelCurTaskId);
    mainLayout->addWidget(labelCurTemplate);
    mainLayout->addWidget(labelCurStatus);

    // ============ 历史任务 ============
    QLabel *titleHistory = new QLabel(QStringLiteral("历史任务"));
    titleHistory->setStyleSheet("font-weight:bold; font-size:12pt;");
    mainLayout->addWidget(titleHistory);

    listHistory = new QListWidget();
    listHistory->setMinimumHeight(100);
    mainLayout->addWidget(listHistory);

    QHBoxLayout *btnLayout2 = new QHBoxLayout();
    QPushButton *btnView = new QPushButton(QStringLiteral("查看结果"));
    QPushButton *btnRefresh = new QPushButton(QStringLiteral("刷新列表"));
    btnLayout2->addWidget(btnView);
    btnLayout2->addWidget(btnRefresh);
    mainLayout->addLayout(btnLayout2);

    connect(btnRefresh, &QPushButton::clicked, this, &TaskDockPanel::onRefreshHistory);
    connect(btnView, &QPushButton::clicked, this, &TaskDockPanel::onViewResult);
    connect(listHistory, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item) {
        emit historyTaskSelected(item->text());
    });

    // ============ 任务结果 ============
    QLabel *titleResult = new QLabel(QStringLiteral("任务结果"));
    titleResult->setStyleSheet("font-weight:bold; font-size:12pt;");
    mainLayout->addWidget(titleResult);

    labelMeanErr = new QLabel(QStringLiteral("平均误差：—"));
    labelMaxErr = new QLabel(QStringLiteral("最大误差：—"));
    labelPassRate = new QLabel(QStringLiteral("合格率：—"));
    mainLayout->addWidget(labelMeanErr);
    mainLayout->addWidget(labelMaxErr);
    mainLayout->addWidget(labelPassRate);

    // 样式统一
    setStyleSheet(R"(
        QWidget { background-color: #f8f9fa; font-family: "Microsoft YaHei"; }
        QPushButton { background-color: #2980b9; color: white; border-radius: 4px; padding: 5px; }
        QPushButton:hover { background-color: #3498db; }
        QListWidget { border: 1px solid #bdc3c7; background: white; }
    )");
}

// 自动生成任务ID
QString TaskDockPanel::makeTaskId() const {
    return QString("T%1").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
}

// 点击“申请任务”时自动生成任务ID
void TaskDockPanel::onApplyTask() {
    QString tpl = comboTemplate->currentText();

    // 自动生成任务ID
    QString tid = makeTaskId();
//    editTaskId->setText(tid);
    QApplication::clipboard()->setText(tid);

    updateCurrentTask(tpl, tid);
    emit taskApplied(tpl, tid);

    QMessageBox::information(this,
        QStringLiteral("任务申请成功"),
        QString(QStringLiteral("样板：%1\n任务ID：%2")).arg(tpl, tid));
}

// 更新当前任务显示
void TaskDockPanel::updateCurrentTask(const QString &tpl, const QString &tid) {
    labelCurTaskId->setText(QStringLiteral("任务ID：") + tid);
    labelCurTemplate->setText(QStringLiteral("样板类型：") + tpl);
    labelCurStatus->setText(QStringLiteral("状态：已申请"));
}

// 刷新历史任务列表
void TaskDockPanel::onRefreshHistory() {
    listHistory->clear();
    QString baseDir = QDir::currentPath() + "/tasks";
    QDir dir(baseDir);
    if (!dir.exists()) return;

    QFile f(dir.filePath("index.json"));
    if (!f.open(QIODevice::ReadOnly)) return;
    QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
    f.close();

    for (auto k : root.keys()) {
        QJsonObject obj = root[k].toObject();
        listHistory->addItem(QString("%1 - %2").arg(k, obj["template"].toString()));
    }
}

// 查看任务结果
void TaskDockPanel::onViewResult() {
    QListWidgetItem *item = listHistory->currentItem();
    if (!item) return;
    QString text = item->text();
    QString tid = text.split(" - ").first();
    QString baseDir = QDir::currentPath() + "/tasks/" + tid;
    showTaskResult(baseDir);
}

// 显示结果文件内容
void TaskDockPanel::showTaskResult(const QString &taskDir) {
    QFile f(QDir(taskDir).filePath("result.json"));
    if (!f.exists()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("该任务尚无结果文件！"));
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
    for (auto v : points) {
        if (v.toObject()["distance"].toDouble() > 0.5) exceed++;
    }
    double passRate = total ? 100.0 * (1.0 - (double)exceed / total) : 0.0;

    labelMeanErr->setText(QStringLiteral("平均误差：") + QString::number(mean, 'f', 3));
    labelMaxErr->setText(QStringLiteral("最大误差：") + QString::number(maxE, 'f', 3));
    labelPassRate->setText(QStringLiteral("合格率：") + QString::number(passRate, 'f', 2) + "%");
}
