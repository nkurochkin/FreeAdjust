#ifndef CBASEPOINTSELECT_H
#define CBASEPOINTSELECT_H

#include <QDialog>
#include <QList>
#include <CJob.h>
#include "chcarray.h"

namespace Ui {
class CBasePointSelect;
}

class CBasePointSelect : public QDialog
{
    Q_OBJECT

public:
    enum{
        CBP_EDIT=0,
        CBP_SELECT
    };

    jobnet::CHCArray<CBasePoint>        m_list;
    QString                             m_filename;
    jobnet::CHClass<CBasePoint>         m_selected;

    unsigned    m_lock_updates;
    unsigned    m_mode;
    bool        m_changed;

    void LoadTable();
    explicit CBasePointSelect(QWidget *parent,jobnet::CHCArray<CBasePoint> &baselist,QString &filename,int mode);
    ~CBasePointSelect();
    
private:
    Ui::CBasePointSelect *ui;

private slots:
    void LoadList();
    void SaveList();
    void selChanged();
};



#endif // CBASEPOINTSELECT_H
