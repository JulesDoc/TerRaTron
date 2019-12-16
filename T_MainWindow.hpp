#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QFileSystemWatcher>

#include "T_TronRunCheckObject.hpp"
#include "T_TronSaveNoticeToDatabaseDlg.hpp"
#include "T_NtcElect.hpp"
#include "T_TronDBBundle.hpp"
#include "T_Database.hpp"

class Ui_mainWindow;
class T_TronNtcElectWidget;

class T_MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	T_MainWindow(T_TronDBBundle &rDBBundle, QWidget *parent = nullptr);
	virtual ~T_MainWindow();

	T_TronDBBundle & getDBBundle() const { return m_rDBBundle; }

private:
	void initializeGUI();
	void validateNotice(const QString &fileName);
	void revalidateNotice();
	
private slots:
	void openFile();
	void close();
	void showAboutMessage();
	void closeEvent(QCloseEvent*);
	void showPageMessages();
	void openTronSaveNoticeToDB();
	void clearWindow();
	void handleMessageLogged(const QString &message);
	void noticeRevalidation();
	//void enableDisableButtons();
	void showWaitingNotices();
	void handleFileChanged(QString);
	void handleShowDetailsClicked();
	

signals:
	void alertOnFileUpdate();

private:
	Ui_mainWindow* m_ui;
	T_TronDBBundle &m_rDBBundle;
	QPointer<T_TronRunCheckObject> m_worker;
	QPointer<QThread> m_workerThread;
	T_TronSaveNoticeToDatabaseDlg* m_dlg;
	QFileSystemWatcher* m_watcher = nullptr;
	bool m_inValidation = false;
	bool m_toRevalidateLater = false;
	bool m_toAlertLater = false;
	bool m_noticeUpdated = true;
};