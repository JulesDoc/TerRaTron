
include(../terrasys.pri)

DEFINES += 
QT += widgets network sql xml 
 
SOURCES += \
	main.cpp \
	T_MainWindow.cpp \
	T_TronRunCheckObject.cpp \
	T_TronSaveNoticeToDatabaseDlg.cpp \
	T_TronNtcElectWidget.cpp \
	T_Configuration.cpp \
	T_WaitingNoticesDlg.cpp
	
 
HEADERS += \  
	TerRaTronLib.hpp \
	T_MainWindow.hpp \
	T_TronRunCheckObject.hpp \
	T_TronSaveNoticeToDatabaseDlg.hpp \
	T_TronNtcElectWidget.hpp \
	T_Configuration.hpp \
	T_WaitingNoticesDlg.hpp
	

FORMS += \
	forms/mainWindow.ui \
	forms/tronNtcElectWidget.ui \
	forms/tronSaveNoticeToDatabaseDlg.ui \
	forms/waitingNoticesDlg.ui
	