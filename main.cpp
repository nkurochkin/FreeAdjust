#include <QtGui>
#include "mainwindow.h"

MainWindow *pmw;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(fadjust);

    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("fadjust_") + locale);
    a.installTranslator(&translator);

    MainWindow w;
    pmw=&w;
    w.show();

    return a.exec();
}
