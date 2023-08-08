#ifndef CSTATIONTI_H
#define CSTATIONTI_H

#include "CJob.h"
#include <QtGui/QTableWidget>

//========================================
//   CStationTI
//

class CStationTI : public QTableWidgetItem
{
public:
    jobnet::CHClass<jobnet::CStation> m_st;

    virtual void changed(){};
    virtual void updateText()=0;
    CStationTI(jobnet::CHClass<jobnet::CStation> &st):QTableWidgetItem()
    {
        m_st=st;
    }
};

class CStationNameTI : public CStationTI
{
public:
    void updateText()
    {
        if(m_st.data())
        {
            setText(m_st->m_node->m_id);
            if(m_st->m_max_error>=1.)
            {
                QColor cc(0xFF,0xD0,0xD0);
                this->setBackgroundColor(cc);
            }
        }
    }

    CStationNameTI(jobnet::CHClass<jobnet::CStation> &st):CStationTI(st)
    {
        updateText();
    }
};

class CStationHeightTI : public CStationTI
{
public:
    int m_lock;
    void changed()
    {
        if(!m_lock)
        {
            bool res;
            double val=text().toDouble(&res);
            if(res)
                m_st->m_height=val;
            updateText();
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_st.data())
        {
            QString text;
            text.sprintf("%.3f",m_st->m_height);
            setText(text);
            if(m_st->m_max_error>=1.)
            {
                QColor cc(0xFF,0xD0,0xD0);
                this->setBackgroundColor(cc);
            }
        }
        m_lock--;
    }

    CStationHeightTI(jobnet::CHClass<jobnet::CStation> &st):CStationTI(st)
    {
        m_lock=0;
        updateText();
    }
};


#endif // CSTATIONTI_H
