#ifndef CSERIALDEVICEIMPORTDLG_H
#define CSERIALDEVICEIMPORTDLG_H

#include <QDialog>
#include <QtConcurrentRun>
#include "serial\qserialdevice\abstractserial.h"

namespace Ui {
class CSerialDeviceImportDlg;
}

class CSerialDeviceImportDlg : public QDialog
{
    Q_OBJECT
    
public:
    AbstractSerial m_port;
    QByteArray     m_buffer;
    int            m_iostate;
    int            m_iostop;
    QFuture<void>  m_future;

    void _IOthread();
    explicit CSerialDeviceImportDlg(QWidget *parent = 0);
    ~CSerialDeviceImportDlg();
    
private:
    void stop_io();

    Ui::CSerialDeviceImportDlg *ui;

private slots:
    void ioStateChanged();
};

#endif // CSERIALDEVICEIMPORTDLG_H
