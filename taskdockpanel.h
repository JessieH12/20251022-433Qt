#ifndef TASKDOCKPANEL_H
#define TASKDOCKPANEL_H

#include <QWidget>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TaskDockPanel; }
QT_END_NAMESPACE

class TaskDockPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TaskDockPanel(QWidget *parent = nullptr);
    ~TaskDockPanel();

signals:
    void taskApplied(const QString &templateType, const QString &taskId);
    void historyTaskSelected(const QString &taskId);
    void logMessageRequested(const QString &title, const QString &message);

private slots:
    void onApplyTask();
    void onRefreshHistory();
    void onViewResult();

private:
    Ui::TaskDockPanel *ui;

    QString makeTaskId() const;
    void updateCurrentTask(const QString &tpl, const QString &tid);
    void showTaskResult(const QString &taskDir);
};

#endif // TASKDOCKPANEL_H

