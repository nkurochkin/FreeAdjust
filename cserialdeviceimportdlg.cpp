#include "mainwindow.h"
#include "cserialdeviceimportdlg.h"
#include "ui_cserialdeviceimportdlg.h"

#include "serial\qserialdeviceenumerator\serialdeviceenumerator.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

static void _IOFunc(CSerialDeviceImportDlg *data)
{
    data->_IOthread();
}

#define REG_KEY_COM_PORT  "com_port/port"
#define REG_KEY_BAUD_RATE "com_port/baud_rate"
#define REG_KEY_CBX_BITS  "com_port/cbx_bits"
#define REG_KEY_CBX_PARITY    "com_port/cbx_parity"
#define REG_KEY_CBX_STOPBITS  "com_port/cbx_stopbits"

CSerialDeviceImportDlg::CSerialDeviceImportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSerialDeviceImportDlg)
{
    QSettings com_setup("FRG","FreeAdjust");

    ui->setupUi(this);
    ui->textBrowser->setFontFamily("Lucida Console");
    ui->textBrowser->setLocale(QLocale("Windows-1251"));
    ui->textBrowser->setWordWrapMode(QTextOption::NoWrap);

    {
        SerialDeviceEnumerator *senum=SerialDeviceEnumerator::instance();
        QStringList list=senum->devicesAvailable();
        ui->cbx_com_port->addItems(list);

        QString val=com_setup.value(REG_KEY_COM_PORT).toString();
        int idx=0;
        int cidx=ui->cbx_com_port->findText(val,Qt::MatchExactly);
        if(cidx!=-1)
            idx=cidx;
        ui->cbx_com_port->setCurrentIndex(idx);
    }
    {
        QStringList list;
        list << "1200" << "2400" << "4800" << "9600" <<
                "14400" << "19200" << "38400" << "57600" << "115200" ;
        ui->cbx_baudrate->addItems(list);

        QString val=com_setup.value(REG_KEY_BAUD_RATE).toString();
        int idx=3;
        int cidx=ui->cbx_baudrate->findText(val,Qt::MatchExactly);
        if(cidx!=-1)
            idx=cidx;
        ui->cbx_baudrate->setCurrentIndex(idx);
    }
    {
        QStringList list;
        list << "8 bit" << "7 bit";
        ui->cbx_bits->addItems(list);

        QString val=com_setup.value(REG_KEY_CBX_BITS).toString();
        int idx=0;
        int cidx=ui->cbx_bits->findText(val,Qt::MatchExactly);
        if(cidx!=-1)
            idx=cidx;
        ui->cbx_bits->setCurrentIndex(idx);

    }
    {
        QStringList list;
        list << "None" << "Odd" << "Even";
        ui->cbx_parity->addItems(list);

        QString val=com_setup.value(REG_KEY_CBX_PARITY).toString();
        int idx=0;
        int cidx=ui->cbx_parity->findText(val,Qt::MatchExactly);
        if(cidx!=-1)
            idx=cidx;
        ui->cbx_parity->setCurrentIndex(idx);

    }
    {
        QStringList list;
        list << "1" << "1.5" << "2";
        ui->cbx_stopbits->addItems(list);

        QString val=com_setup.value(REG_KEY_CBX_STOPBITS).toString();
        int idx=0;
        int cidx=ui->cbx_stopbits->findText(val,Qt::MatchExactly);
        if(cidx!=-1)
            idx=cidx;
        ui->cbx_stopbits->setCurrentIndex(idx);
    }
    connect(ui->pushButton,   SIGNAL(released()), this, SLOT(ioStateChanged()));
    m_iostate=0;
    m_iostop =0;
}

void CSerialDeviceImportDlg::_IOthread()
{
    m_buffer.clear();
    QByteArray ba; //received data
    m_iostate=2;
    while(!m_iostop)
    {
        m_port.waitForReadyRead(200);
        qint64 len=m_port.bytesAvailable();
        if(len)
        {
            ba.clear();
            ba=m_port.read(len);
            m_buffer+=ba;
            ui->lcdNumber->display(m_buffer.size());
        }
    }
    m_iostate=0;
}

void CSerialDeviceImportDlg::stop_io()
{
    if(m_port.isOpen())
    {
        m_iostop=1;
        m_future.waitForFinished();
        m_port.close();
    }
}

void CSerialDeviceImportDlg::ioStateChanged()
{
    if(m_port.isOpen())
    {
        stop_io();
        ui->pushButton->setText("Начать загрузку");
        {
            QPushButton *bp=ui->buttonBox->button(QDialogButtonBox::Ok);
            if(bp)
                bp->setEnabled(true);
        }
        {
            QString text;
            if(m_buffer.size())
            {
                text=m_buffer.data();
            }else
            {
                text="no data";
            }
            ui->textBrowser->setText(text);
        }
    }else
    {
        QSettings com_setup("FRG","FreeAdjust");
        QString pval;
        pval=ui->cbx_com_port->currentText();
        com_setup.setValue(REG_KEY_COM_PORT,pval);
        m_port.setDeviceName(pval);

        m_port.setReadBufferSize(1024*64);

        if(m_port.open(QIODevice::ReadOnly))
        {
            pval=ui->cbx_baudrate->currentText();
            com_setup.setValue(REG_KEY_BAUD_RATE,pval);
            m_port.setBaudRate(pval);

            pval=ui->cbx_bits->currentText();
            com_setup.setValue(REG_KEY_CBX_BITS,pval);
            m_port.setDataBits(pval);

            pval=ui->cbx_parity->currentText();
            com_setup.setValue(REG_KEY_CBX_PARITY,pval);
            m_port.setParity(pval);

            pval=ui->cbx_stopbits->currentText();
            com_setup.setValue(REG_KEY_CBX_STOPBITS,pval);
            m_port.setStopBits(pval);

            m_port.setFlowControl(AbstractSerial::FlowControlOff);

            ui->lcdNumber->display(0);
            m_iostate=1;
            m_iostop =0;
            m_future=QtConcurrent::run(_IOFunc,this);
            ui->pushButton->setText("Завершить");
            {
                QPushButton *bp=ui->buttonBox->button(QDialogButtonBox::Ok);
                if(bp)
                    bp->setEnabled(false);
            }
        }
    }
}

CSerialDeviceImportDlg::~CSerialDeviceImportDlg()
{
    if(m_port.isOpen())
        stop_io();
    delete ui;
}

void MainWindow::importCOMdevice()
{
    CSerialDeviceImportDlg dlg(this);
    //dlg.setModal(1);
    if(dlg.exec())
    {
        if(dlg.m_buffer.size())
        {
            int rep;
            do{
                rep=0;
                QString fileName;
                fileName = QFileDialog::getSaveFileName(this,
                     tr("Сохранить данные"), "", tr("Данные (*.txt)"));
                if( !fileName.isNull() )
                {
                    FILE *out=_wfopen((wchar_t *)fileName.unicode(),L"wt");
                    if(!out)
                    {
                        QMessageBox msgBox;
                        QString text="Невозможно открыть файл\n'";
                        text+=fileName;
                        text+="'\nдля записи !";
                        msgBox.setText(text);
                        msgBox.addButton(" Выбрать другой ",QMessageBox::AcceptRole);
                        msgBox.addButton(" Не сохранять ",QMessageBox::RejectRole);
                        if(msgBox.exec()==QMessageBox::AcceptRole)
                        {
                            rep=1;
                        }
                    }else
                    {
                        fwrite(dlg.m_buffer.data(),1,dlg.m_buffer.size(),out);
                        fclose(out);

                        QMessageBox msgBox;
                        msgBox.setText("Импортировать данные в проект ?");
                        msgBox.addButton(" Да ",QMessageBox::AcceptRole);
                        msgBox.addButton(" Нет ",QMessageBox::RejectRole);
                        if(msgBox.exec()==QMessageBox::AcceptRole)
                        {
                            m_job.ImportDeviceFile(fileName,0);
                            ApplyBaseList();
                            UpdateInterface();
                        }
                    }
                }else
                {
                    QMessageBox msgBox;
                    msgBox.setText("Данные будут утеряны !");
                    msgBox.addButton(" Сохранить ",QMessageBox::AcceptRole);
                    msgBox.addButton(" Не сохранять ",QMessageBox::RejectRole);
                    if(msgBox.exec()==QMessageBox::AcceptRole)
                    {
                        rep=1;
                    }
                }
            }while(rep);
        }
    }
}

