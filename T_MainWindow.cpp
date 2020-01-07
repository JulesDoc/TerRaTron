
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include "T_Logger.hpp"
#include "T_Configuration.hpp"
#include "T_MainWindow.hpp"
#include "T_TronNtcElectWidget.hpp"
#include "T_QtProgramAboutDlg.hpp"
#include "ui_mainWindow.h"
#include "ui_tronNtcElectWidget.h"
#include "T_SqlTableWidget.hpp"
#include "T_GUISqlStatement.hpp"
#include "T_WaitingNoticesDlg.hpp"

T_MainWindow::T_MainWindow(T_TronDBBundle &rDBBundle, QWidget* parent)
	:QMainWindow(parent), m_rDBBundle(rDBBundle)
{
	m_workerThread = new QThread(this);
	m_worker = new T_TronRunCheckObject(rDBBundle.getTerRaBase().getConnectionInfo()); // Orphan object
	//The instance m_worker cannot be moved to thread if it has a parent, that is why it is orphan.
	m_worker->moveToThread(m_workerThread);
	connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
	m_workerThread->start();
	QTimer::singleShot(0, m_worker, &T_TronRunCheckObject::initialize);

	initializeGUI();
}

T_MainWindow::~T_MainWindow()
{
	m_workerThread->quit();
	m_workerThread->wait();
	delete m_ui;

}

void T_MainWindow::initializeGUI()
{
	m_ui = new Ui_mainWindow;
	m_ui->setupUi(this);

	//test
	statusBar()->showMessage(tr("System is Ready"));
	m_watcher = new QFileSystemWatcher(this);
	m_dlg = new T_TronSaveNoticeToDatabaseDlg(getDBBundle().getTerRaBase().getConnectionInfo(), this);

	m_ui->tronNtcElectWidget->getComboBox()->setVisible(false);
	m_ui->dockWidget->setWidget(m_ui->tronNtcElectWidget->getBrowser());
	m_ui->stackedWidget->setVisible(false);
	m_ui->actionSaveNoticeToDatabase->setEnabled(false);
	m_ui->actionRevalidate_saved_file->setEnabled(false);
	m_ui->actionClose_file->setEnabled(false);

	connect(m_worker, &T_TronRunCheckObject::runCheckCompleted, m_ui->tronNtcElectWidget, &T_TronNtcElectWidget::setRunCheckResult);
	connect(m_worker, &T_TronRunCheckObject::runCheckCompleted, this, &T_MainWindow::noticeRevalidation);
	connect(m_ui->tronNtcElectWidget, &T_TronNtcElectWidget::setResultCompleted, this, &T_MainWindow::showPageMessages);
	connect(m_ui->actionAbout_TerRaTron, &QAction::triggered, this, &T_MainWindow::showAboutMessage);
	connect(m_ui->actionOpen_file, &QAction::triggered, this, &T_MainWindow::openFile);
	connect(m_ui->close_toolButton, &QToolButton::clicked, m_ui->tronNtcElectWidget, &T_TronNtcElectWidget::clearAll);
	connect(m_ui->close_toolButton, &QToolButton::clicked, this, &T_MainWindow::clearWindow);
	connect(m_ui->actionClose_file, &QAction::triggered, m_ui->tronNtcElectWidget, &T_TronNtcElectWidget::clearAll);
	connect(m_ui->actionClose_file, &QAction::triggered, this, &T_MainWindow::clearWindow);
	connect(m_ui->actionSaveNoticeToDatabase, &QAction::triggered, this, &T_MainWindow::openTronSaveNoticeToDB);
	connect(m_ui->actionRevalidate_saved_file, &QAction::triggered, this, &T_MainWindow::revalidateNotice);
	connect(T_Logger::singleInstance(), &T_Logger::infoMessageLogged, this, &T_MainWindow::handleMessageLogged);
	connect(m_ui->actionShow_waiting_notices, &QAction::triggered, this, &T_MainWindow::showWaitingNotices);
	connect(m_dlg, SIGNAL(accepted()), this, SLOT(showWaitingNotices()));
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChanged(QString)));
	connect(this, SIGNAL(alertOnFileUpdate()), m_ui->tronNtcElectWidget, SLOT(handleAlertFileNotUpdated()));
	connect(m_ui->validation_details_pushButton, &QPushButton::clicked, this, &T_MainWindow::handleShowDetailsClicked);
}

void T_MainWindow::revalidateNotice()
{
	validateNotice(m_ui->tronNtcElectWidget->getNtcElect().getFileInfo().absoluteFilePath());
}

void T_MainWindow::showWaitingNotices()
{
	T_WaitingNoticesDlg dlg(getDBBundle(), this);
	dlg.exec();
}

void T_MainWindow::handleMessageLogged(const QString &message)
{
	m_ui->validation_details_plainTextEdit->appendPlainText(message);
}

void T_MainWindow::openFile()
{
	if (m_inValidation)
	{
		const int msg = QMessageBox::warning(this, "Validation in progress", "Please, wait until it finishes", QMessageBox::Ok);
		return;
	}

	QDir currentDir;
	QString fileName = QFileDialog::getOpenFileName(this, QString(), T_Configuration::getSettingsPath(),
		tr("All files (*.*);;xml (*.xml);;txt (*.txt)"));

	if (fileName.isEmpty()) return;

	m_watcher->addPath(fileName);
	T_Configuration::setSettingsPath(currentDir.absoluteFilePath(fileName));
	QFont font("Times", 10, QFont::Bold);
	m_ui->validationMessage_label->setText(QString("Validating file %1").arg(fileName));
	validateNotice(fileName);
}

void T_MainWindow::handleShowDetailsClicked()
{
	if (m_ui->validation_details_pushButton->isChecked())
	{
		m_ui->validation_details_pushButton->setText("Hide details");
	}
	else
	{
		m_ui->validation_details_pushButton->setText("Show details");
	}
}


void T_MainWindow::handleFileChanged(QString file)
{
	QFileDialog *fileDialog = this->findChild<QFileDialog*>();
	if (fileDialog)
		fileDialog->deleteLater();

	if (m_inValidation)
	{
		const int msg = QMessageBox::warning(this, "External modifications detected",
			QString("File %1 has been modified, please, revalidate it before send it to the database").arg(file), QMessageBox::Ok);


		m_toAlertLater = true;
		return;
	}
	else {
		const int msg = QMessageBox::warning(this, QString("External modifications detected"),
			QString("Changes detected in file: %1. Do you want to revalidate it?").arg(file), QMessageBox::Yes | QMessageBox::No);
		if (msg == QMessageBox::Yes)
		{
			validateNotice(file);
		}
		else {
			m_noticeUpdated = false;
			emit alertOnFileUpdate();
		}
	}

}

void T_MainWindow::noticeRevalidation()
{
	if (m_toAlertLater)
	{
		m_noticeUpdated = false;
		emit alertOnFileUpdate();
		m_toAlertLater = false;
	}
	return;
}

void T_MainWindow::validateNotice(const QString &fileName)
{
	m_ui->validation_details_plainTextEdit->clear();
	m_ui->close_toolButton->clicked();
	m_ui->stackedWidget->setVisible(true);
	m_ui->stackedWidget->setCurrentIndex(1);
	m_inValidation = true;
	m_noticeUpdated = true;
	bool isApplyDeepValidation = false;
	if (m_ui->actionApply_deep_validation->isChecked()) isApplyDeepValidation = true;
	QMetaObject::invokeMethod(m_worker, "runCheckFile", Q_ARG(QString, fileName), Q_ARG(bool, isApplyDeepValidation));
}

void T_MainWindow::showPageMessages()
{
	m_ui->stackedWidget->setVisible(true);
	m_ui->stackedWidget->setCurrentIndex(0);
	m_inValidation = false;
	m_ui->actionSaveNoticeToDatabase->setEnabled(true);
	m_ui->actionRevalidate_saved_file->setEnabled(true);
	m_ui->tronNtcElectWidget->getComboBox()->setVisible(true);
	m_ui->actionClose_file->setEnabled(true);
}

void T_MainWindow::openTronSaveNoticeToDB()
{
	if (!m_noticeUpdated)
	{
		QString fileName = m_ui->tronNtcElectWidget->getNtcElect().getFileInfo().baseName();
		const int msg = QMessageBox::warning(this, "File not updated",
			QString("File %1 must be revalidated before being sent to the database").arg(fileName), QMessageBox::Ok);
		return;
	}

	if (m_ui->tronNtcElectWidget->getNtcElect().hasErrors())
	{
		const int msg = QMessageBox::warning(this, "The notices has errors", "Please, correct them and try again", QMessageBox::Ok);
		return;
	}
	else if (!m_ui->tronNtcElectWidget->getNtcElect().hasErrors())
	{
		if (m_ui->tronNtcElectWidget->getNtcElect().hasWarnings())
		{
			const int msg = QMessageBox::warning(this, "The notice has warnings", "Do you want to continue?", QMessageBox::Yes | QMessageBox::No);
			if (msg == QMessageBox::No)
			{
				return;
			}
		}
	}

	QAction *pAction = qobject_cast<QAction*>(sender());
	m_dlg->setWindowTitle(pAction->objectName());
	m_dlg->launchSaveToDBDlg(m_ui->tronNtcElectWidget->getNtcElect());
}

void T_MainWindow::clearWindow()
{
	if (m_inValidation)
	{
		const int msg = QMessageBox::warning(this, "Validation in progress", "Please, wait until it finishes", QMessageBox::Ok);
		return;
	}

	m_ui->stackedWidget->setVisible(false);
	m_ui->actionSaveNoticeToDatabase->setEnabled(false);
	m_ui->actionRevalidate_saved_file->setEnabled(false);
	m_ui->tronNtcElectWidget->getComboBox()->setVisible(false);
	m_ui->actionClose_file->setEnabled(false);
}

void T_MainWindow::closeEvent(QCloseEvent* cEvent)
{
	cEvent->accept();
	T_Configuration::saveSettings();
}

void T_MainWindow::close()
{
	QApplication::quit();
	T_Configuration::saveSettings();
}

void T_MainWindow::showAboutMessage()
{
	const QString version = QLatin1Literal("1.0");
	T_QtProgramAboutDlg dlg(version);
	RCT_DatabaseConnectionInfo rcConnectionInfo = getDBBundle().getTerRaBase().getConnectionInfo();
	dlg.showAboutMessage(rcConnectionInfo, this);
}
