#ifndef COBSERVTI_H
#define COBSERVTI_H

#include "CJob.h"

//========================================
//   CObservTI
//

class CObservTI : public QTreeWidgetItem
{
public:
    jobnet::CHClass<jobnet::CStationNode> m_stnode;
    jobnet::CJob *m_host;

    virtual void changed(unsigned column)=0;
    virtual void updateText()=0;
    CObservTI(jobnet::CHClass<jobnet::CStationNode> &stnode):QTreeWidgetItem()
    {
        m_stnode=stnode;
    }
};

class CObservNameTI : public CObservTI
{
public:

    void changed(unsigned column)
    {
        if(!column)
            m_stnode->m_fActive=checkState(0)==Qt::Checked;
    }

    void updateText()
    {
        QColor cc(0xE0,0xE0,0xE0,0xFF);
        if(m_stnode.data())
        {
            if(m_stnode->m_node->toAdjust(1))
                cc.setRgb(0xFF,0xFF,0xE0);
            setBackgroundColor(0,cc);
            setBackgroundColor(1,cc);
            setBackgroundColor(2,cc);
            setBackgroundColor(3,cc);
            setBackgroundColor(4,cc);
            setText(0,m_stnode->m_node->m_id);
            {
                QString text;
                m_stnode->m_node->getFlags(text);
                setText(1,text);
            }
            setFlags(flags() | Qt::ItemIsUserCheckable);
            setCheckState(0,m_stnode->m_fActive?Qt::Checked:Qt::Unchecked);
        }
    }

    CObservNameTI(jobnet::CHClass<jobnet::CStationNode> &stnode):CObservTI(stnode)
    {
        updateText();
    }
};

class CObservVal : public CObservTI
{
public:
    jobnet::CHClass<jobnet::CStationObservation> m_obs;

private:
    void update_stdev()
    {
        QString text;
        if(m_obs->m_stdev!=0.)
            text.sprintf("%.1f",m_obs->m_stdev);
        setText(2,text);
    }

    void update_height()
    {
        QString text;
            text.sprintf("%.3f",m_obs->m_height);
        setText(3,text);
    }

    void update_corrections()
    {
        if(m_obs->m_fcval)
        {
            QString text;
            text.sprintf("%.3f",m_obs->m_dev);
            setText(5,text);
            text.sprintf("%.3f",m_obs->m_devint);
            setText(6,text);
        }else
        {
            setText(5,"");
            setText(6,"");
        }
    }

public:
    void updateText()
    {
        setCheckState(0,m_obs->m_fActive?Qt::Checked:Qt::Unchecked);
        if(m_obs.data())
        {
            if(m_obs->m_fcval && m_obs->m_devint>=1.)
            {
                QColor cc(0xFF,0xD0,0xD0);
                setBackgroundColor(0,cc);
                setBackgroundColor(1,cc);
                setBackgroundColor(2,cc);
                setBackgroundColor(3,cc);
                setBackgroundColor(4,cc);
                setBackgroundColor(5,cc);
                setBackgroundColor(6,cc);
            }
            switch(m_obs->m_type)
            {
            case jobnet::ST_OBS_HA:
                setText(0,"HA");
                {
                    QString text;
                    pmw->m_ha_units.ToViewString(m_obs->m_val,text);
                    setText(1,text);
                }
                update_stdev();
                setText(3,"");
                if(m_obs->m_fcval)
                {
                    QString text;
                    pmw->m_ha_units.ToViewString(m_obs->m_cval,text);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            case jobnet::ST_OBS_SD:
                setText(0,"SD");
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_val);
                    setText(1,text);
                }
                update_stdev();
                update_height();
                if(m_obs->m_fcval)
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_cval);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            case jobnet::ST_OBS_HD:
                setText(0,"HD");
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_val);
                    setText(1,text);
                }
                update_stdev();
                update_height();
                if(m_obs->m_fcval)
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_cval);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            case jobnet::ST_OBS_VD:
                setText(0,"VD");
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_val);
                    setText(1,text);
                }
                update_stdev();
                update_height();
                if(m_obs->m_fcval)
                {
                    QString text;
                    text.sprintf("%.3f",m_obs->m_cval);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            case jobnet::ST_OBS_VA:
                setText(0,"VA");
                {
                    QString text;
                    pmw->m_va_units.ToViewString(m_obs->m_val,text);
                    setText(1,text);
                }
                update_stdev();
                update_height();
                if(m_obs->m_fcval)
                {
                    QString text;
                    pmw->m_ha_units.ToViewString(m_obs->m_cval,text);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            case jobnet::ST_OBS_ZA:
                setText(0,"ZA");
                {
                    QString text;
                    pmw->m_va_units.ToViewString(m_obs->m_val,text);
                    setText(1,text);
                }
                update_stdev();
                update_height();
                if(m_obs->m_fcval)
                {
                    QString text;
                    pmw->m_ha_units.ToViewString(m_obs->m_cval,text);
                    setText(4,text);
                }else
                    setText(4,"");
                update_corrections();
                break;
            }
        }
    }

    void changed(unsigned column)
    {
        switch(column)
        {
        case 0:
            m_obs->m_fActive=(checkState(0)==Qt::Checked);
            break;
        case 1: updateText(); break;
        case 2:
            {
                bool ok=false;
                double val=text(2).toDouble(&ok);
                if(ok)
                    m_obs->m_stdev=val;
                else
                    m_obs->m_stdev=0.;
                update_stdev();
            }
            break;
        case 3:
            if(m_obs->m_type!=jobnet::ST_OBS_HA)
            {
                bool ok=false;
                double val=text(3).toDouble(&ok);
                if(ok)
                    m_obs->m_height=val;
                update_height();
            }else
                setText(3,"");
            break;
        }
    }


    CObservVal(jobnet::CHClass<jobnet::CStationNode> &stnode,jobnet::CHClass<jobnet::CStationObservation> &obs):CObservTI(stnode)
    {
        m_obs=obs;
        updateText();
        setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
    }
};


#endif // COBSERVTI_H
