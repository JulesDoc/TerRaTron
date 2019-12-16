#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include "T_Logger.hpp"
#include "T_Configuration.hpp"
#include "T_NtcElect.hpp"
#include "T_CfgFileParser.hpp"
#include "T_Database.hpp"
#include "T_MainWindow.hpp"
#include "T_IDWMUtils.hpp"

int main(int argc, char *argv[])
{
	QDir tmpDir = QStandardPaths::standardLocations(QStandardPaths::TempLocation).first();
	QString fileName; // = tmpDir.absoluteFilePath("terrarevq_logger.txt");
	QString sqlFileName; // = tmpDir.absoluteFilePath("terrarevq_logger.sql");
	bool bGenerateDebugMessages = true;
	T_Logger::installMessageHandler(fileName, sqlFileName, bGenerateDebugMessages);

	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(QString::fromUtf8(":/icons/tronIcon.png")));
	
	// Check IDWM
	QString errorString;
	if (!T_IDWMUtils::checkIDWMIsUpToDate(&errorString))
	{
		QMessageBox::critical(0, QString("IDWM error"), errorString);
		return 0;
	}

	// Splash
	QPixmap pixmap(QString::fromLatin1(":/icons/tronLogo.png"));
	QSplashScreen splash(pixmap);
	splash.setMask(pixmap.mask());
	splash.show();

	T_Configuration::loadSettings();

	qRegisterMetaType<T_NtcElect>("T_NtcElect");

	const T_DBTarget aDBTarget = T_CfgFileParser::exists() ? T_CfgFileParser::getDBTarget() : T_DBTarget::DEVL;
	const T_DBName aDBName(T_DBName::TRS_DB);
	// m_processingDatabase = APT_Database(new T_Database(aDBName, rcDBTarget, T_DBAppRole::PROCESSING));
	T_TronDBBundle aDBBundle(aDBTarget, aDBName);

	T_MainWindow mainWin(aDBBundle);
	mainWin.setWindowTitle(QString("TerRaTron - %1").arg(aDBTarget.asDisplayString()));
	mainWin.setWindowState(Qt::WindowMaximized);
	mainWin.show();
	splash.finish(&mainWin);

	return app.exec();
}