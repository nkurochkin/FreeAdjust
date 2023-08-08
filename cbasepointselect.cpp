#include "cbasepointselect.h"
#include "ui_cbasepointselect.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QSettings>

class CBP_TI : public QTableWidgetItem
{
public:
    jobnet::CHClass<CBasePoint> m_node;

    virtual void changed()=0;
    virtual void updateText()=0;
    CBP_TI(jobnet::CHClass<CBasePoint> &node):QTableWidgetItem() //node->m_name)
    {
        m_node=node;
    }
};

class CBPName_TI : public CBP_TI
{
public:
    void changed()
    {

    }

    void updateText()
    {
        if(m_node.data())
            setText(m_node->m_name);
    }
    CBPName_TI(jobnet::CHClass<CBasePoint> &node):CBP_TI(node)
    {
        updateText();
    }
};

class CBPX_TI : public CBP_TI
{
public:
    int m_lock;
    void changed()
    {

    }

    void updateText()
    {
        QString text;
        if(m_node->m_fXY)
            text.sprintf("%.3f",m_node->m_xy.rx());
        setText(text);
    }
    CBPX_TI(jobnet::CHClass<CBasePoint> &node):CBP_TI(node)
    {
        m_lock=1;
        updateText();
        m_lock=0;
    }
};

class CBPY_TI : public CBP_TI
{
public:
    int m_lock;
    void changed()
    {

    }

    void updateText()
    {
        QString text;
        if(m_node->m_fXY)
            text.sprintf("%.3f",m_node->m_xy.ry());
        setText(text);
    }
    CBPY_TI(jobnet::CHClass<CBasePoint> &node):CBP_TI(node)
    {
        m_lock=1;
        updateText();
        m_lock=0;
    }
};

class CBPZ_TI : public CBP_TI
{
public:
    int m_lock;
    void changed()
    {

    }

    void updateText()
    {
        QString text;
        if(m_node->m_fZ)
            text.sprintf("%.3f",m_node->m_z);
        setText(text);
    }
    CBPZ_TI(jobnet::CHClass<CBasePoint> &node):CBP_TI(node)
    {
        m_lock=1;
        updateText();
        m_lock=0;
    }
};



CBasePointSelect::CBasePointSelect(QWidget *parent,jobnet::CHCArray<CBasePoint> &baselist,QString &filename,int mode) :
    QDialog(parent), ui(new Ui::CBasePointSelect)
{
    ui->setupUi(this);
    m_mode     =mode;
    m_filename =filename;

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(
                ui->tableWidget->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Name")<<tr("X")<<tr("Y")<<tr("H");
        ui->tableWidget->setHorizontalHeaderLabels(list);
    }

    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->loadList,  SIGNAL(released()), this, SLOT(LoadList()));
    connect(ui->saveList,  SIGNAL(released()), this, SLOT(SaveList()));
    connect(ui->tableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(selChanged()));


    int i;
    for(i=0;i<baselist.size();i++)
    {
        jobnet::CHClass<CBasePoint> bp1=baselist[i];
        jobnet::CHClass<CBasePoint> bp=new CBasePoint(bp1);
        m_list.Append(bp);
    }
    m_changed=false;
    m_lock_updates=0;
    LoadTable();
}

CBasePointSelect::~CBasePointSelect()
{
    delete ui;
}

void CBasePointSelect::selChanged()
{
    QList<QTableWidgetItem *> list=ui->tableWidget->selectedItems();
    if(list.size())
    {
        m_selected=((CBP_TI *)list[0])->m_node;
    }else
    {
        m_selected=0;
    }
}

void CBasePointSelect::LoadTable()
{
    if(m_lock_updates)
        return;
    m_lock_updates++;
    ui->tableWidget->setRowCount(m_list.size());
    int i;
    for(i=0;i<m_list.size();i++)
    {
        {
            CBPName_TI *wi=new CBPName_TI(m_list[i]);
            ui->tableWidget->setItem(i,0,wi);
        }
        {
            CBPX_TI *wi=new CBPX_TI(m_list[i]);
            ui->tableWidget->setItem(i,1,wi);
        }
        {
            CBPY_TI *wi=new CBPY_TI(m_list[i]);
            ui->tableWidget->setItem(i,2,wi);
        }
        {
            CBPZ_TI *wi=new CBPZ_TI(m_list[i]);
            ui->tableWidget->setItem(i,3,wi);
        }
    }
    ui->tableWidget->sortByColumn(0,Qt::AscendingOrder);
    m_lock_updates--;
}

//int LoadBaseList(QList<jobnet::CHClass<CBasePoint> > &list,QString &filename)
int LoadBaseList(jobnet::CHCArray<CBasePoint> &list,QString &filename)
{
    FILE *in=_wfopen((wchar_t *)filename.unicode(),L"rt");
    if(in)
    {
        list.Clear();
        char buff[512];
        while(fgets(buff,512,in))
        {
            QString name;
            double x=0.,y=0.,z=0.;
            bool   fx=0;
            bool   fy=0;
            bool   fz=0;
            char *ps;
            char *pe;
            pe=strchr(buff,';');
            if(!pe)
                continue;
            *pe=0;
            name=buff;

            ps=pe+1;
            pe=strchr(ps,';');
            if(!pe)
                continue;
            *pe=0;
            if(sscanf(ps," %lf",&x)!=-1)
                fx=1;

            ps=pe+1;
            pe=strchr(ps,';');
            if(!pe)
                continue;
            *pe=0;
            if(sscanf(ps," %lf",&y)!=-1)
                fy=1;

            ps=pe+1;
            if(sscanf(ps," %lf",&z)!=-1)
                fz=1;

            unsigned flags=0;
            if(fx || fy)
                flags=1;
            if(fz)
                flags|=2;
            if(name.size())
            {
                jobnet::CHClass<CBasePoint> bp=new CBasePoint(name,x,y,z,flags);
                list.Append(bp);
            }
        }
        fclose(in);
        return 0;
    }
    return -1;
}

void CBasePointSelect::LoadList()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
         tr("Открыть каталог пунктов"), "", tr("Текстовый файл с разделителями (*.csv)"));
    if( !fileName.isNull() )
    {
        if(!LoadBaseList(m_list,fileName))
        {
            LoadTable();
            m_filename=fileName;
            m_changed =true;
        }
    }
}

void CBasePointSelect::SaveList()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
         tr("Сохранить каталог пунктов"), "", tr("Текстовый файл с разделителями (*.csv)"));
    if( !fileName.isNull() )
    {
        FILE *out=_wfopen((wchar_t *)fileName.unicode(),L"wt");
        if(out)
        {
            int i;
            for(i=0;i<m_list.size();i++)
            {
                jobnet::CHClass<CBasePoint> bp=m_list[i];
                fprintf(out,"%s;",bp->m_name.toAscii());
                if(bp->m_fXY)
                    fprintf(out,"%.3f;%.3f;",bp->m_xy.x(),bp->m_xy.y());
                else
                    fprintf(out,";;");
                if(bp->m_fZ)
                    fprintf(out,"%.3f",bp->m_z);
                fprintf(out,"\n");
            }
            fclose(out);
            if(m_filename!=fileName)
            {
                m_filename=fileName;
                m_changed =true;
            }
        }
    }
}

//
//
//
void MainWindow::BaseListEdit()
{
    CBasePointSelect dlg(this,m_baselist,m_baselist_file_name,CBasePointSelect::CBP_EDIT);
    if(dlg.exec() && dlg.m_changed)
    {
        m_baselist=dlg.m_list;
        m_baselist_file_name=dlg.m_filename;
        QSettings set;
        set.setValue("baselist/path",m_baselist_file_name);
    }
}

CBasePoint *MainWindow::selectBasePoint()
{
    CBasePointSelect dlg(this,m_baselist,m_baselist_file_name,CBasePointSelect::CBP_SELECT);
    if(dlg.exec())
    {
        if(dlg.m_changed)
        {
            m_baselist=dlg.m_list;
            m_baselist_file_name=dlg.m_filename;
            QSettings set;
            set.setValue("baselist/path",m_baselist_file_name);
        }
        return dlg.m_selected.Lock();
    }
    return 0;
}

