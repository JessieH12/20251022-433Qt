#ifndef TASKDOCKPANEL_H
#define TASKDOCKPANEL_H

#include <QWidget>
#include <QJsonObject>

class QComboBox;
class QLineEdit;
class QListWidget;
class QLabel;

class TaskDockPanel : public QWidget
{
	Q_OBJECT

public:
	explicit TaskDockPanel(QWidget *parent = nullptr);
	void loadHistoryTasks();        // ������ʷ����
	void showTaskResult(const QString &taskDir); // ��ʾ���

signals:
	void taskApplied(const QString &templateName, const QString &taskId);
	void historyTaskSelected(const QString &taskId);

private slots:
	void onApplyTask();
	void onRefreshHistory();
	void onViewResult();

private:
	QString makeTaskId() const;
	void updateCurrentTask(const QString &templateName, const QString &taskId);

	// �ؼ�ָ��
	QComboBox *comboTemplate;
//	QLineEdit *editTaskId;
	QLabel *labelCurTaskId;
	QLabel *labelCurTemplate;
	QLabel *labelCurStatus;
	QListWidget *listHistory;
	QLabel *labelMeanErr;
	QLabel *labelMaxErr;
	QLabel *labelPassRate;
};

#endif // TASKDOCKPANEL_H

