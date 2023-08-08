#ifndef CNODETI_H
#define CNODETI_H

#include "CJob.h"

class CNodeTI : public QTableWidgetItem
{
public:
    jobnet::CHClass<jobnet::CNode> m_node;

    virtual void changed()=0;
    virtual void updateText()=0;
    CNodeTI(jobnet::CHClass<jobnet::CNode> &node):QTableWidgetItem(node->m_id)
    {
        m_node=node;
    }
};

class CNodeTypeTI : public CNodeTI
{
public:
    void changed()
    {

    }

    void updateText()
    {
        if(m_node.data())
        {
            QString text;
            m_node->getFlags(text);
            setText(text);
        }
    }
    CNodeTypeTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        updateText();
    }
};

class CNodeNameTI : public CNodeTI
{
public:
    void changed()
    {

    }

    void updateText()
    {
        if(m_node.data())
            setText(m_node->m_id);
    }
    CNodeNameTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        updateText();
    }
};

class CANodeNameTI : public CNodeTI
{
public:
    void changed()
    {

    }

    void updateText()
    {
        if(m_node.data())
            setText(m_node->m_id);
    }
    CANodeNameTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        updateText();
    }
};

class CNodeXTI : public CNodeTI
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
            {
                m_node->fx=val;
                m_node->m_fX=1;
            }else
            {
                m_node->m_fX=0;
            }
            updateText();
            pmw->UpdateScene();
            pmw->UpdateNodeInfo(m_node);
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_fX)
                text.sprintf("%.3f",m_node->fx);
            setText(text);
        }
        m_lock--;
    }
    CNodeXTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};

class CNodeYTI : public CNodeTI
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
            {
                m_node->fy=val;
                m_node->m_fY=1;
            }else
            {
                m_node->m_fY=0;
            }
            updateText();
            pmw->UpdateScene();
            pmw->UpdateNodeInfo(m_node);
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_fY)
                text.sprintf("%.3f",m_node->fy);
            setText(text);
        }
        m_lock--;
    }
    CNodeYTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};

class CNodeZTI : public CNodeTI
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
            {
                m_node->fz=val;
                m_node->m_fZ=1;
            }else
            {
                m_node->m_fZ=0;
            }
            updateText();
            pmw->UpdateScene();
            pmw->UpdateNodeInfo(m_node);
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_fZ)
                text.sprintf("%.3f",m_node->fz);
            setText(text);
        }
        m_lock--;
    }
    CNodeZTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};

class CANodeXTI : public CNodeTI
{
public:
    int m_lock;

    void changed()
    {
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_faX)
                text.sprintf("%.3f",m_node->ax);
            setText(text);
        }
        m_lock--;
    }
    CANodeXTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};


class CANodeYTI : public CNodeTI
{
public:
    int m_lock;

    void changed()
    {
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_faY)
                text.sprintf("%.3f",m_node->ay);
            setText(text);
        }
        m_lock--;
    }
    CANodeYTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};

class CANodeZTI : public CNodeTI
{
public:
    int m_lock;

    void changed()
    {
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            if(m_node->m_faZ)
                text.sprintf("%.3f",m_node->az);
            setText(text);
        }
        m_lock--;
    }
    CANodeZTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};



class CNodeADJTI : public CNodeTI
{
public:
    int m_lock;

    void changed()
    {
        if(!m_lock)
        {
            if(text().contains('y',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x01;
            else
            if(text().contains('Y',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x02;
            else
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03);

            if(text().contains('x',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x01;
            else
            if(text().contains('X',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03)|0x02;
            else
                m_node->m_adj_flags=(m_node->m_adj_flags&~0x03);

            if(text().contains('z',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0xC)|0x04;
            else
            if(text().contains('Z',Qt::CaseSensitive))
                m_node->m_adj_flags=(m_node->m_adj_flags&~0xC)|0x08;
            else
                m_node->m_adj_flags=(m_node->m_adj_flags&~0xC);

            updateText();
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            QString text;
            switch(m_node->m_adj_flags&0x03)
            {
            case 0x01: text+="xy"; break;
            case 0x02: text+="XY"; break;
            }
            switch(m_node->m_adj_flags&0x0C)
            {
            case 0x04: text+='z'; break;
            case 0x08: text+='Z'; break;
            }
            setText(text);
        }
        m_lock--;
    }
    CNodeADJTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};

class CNodeCommentTI : public CNodeTI
{
public:
    int m_lock;

    void changed()
    {
        if(!m_lock)
        {
            m_node->m_comment=text();
            //updateText();
        }
    }

    void updateText()
    {
        m_lock++;
        if(m_node.data())
        {
            setText(m_node->m_comment);
        }
        m_lock--;
    }
    CNodeCommentTI(jobnet::CHClass<jobnet::CNode> &node):CNodeTI(node)
    {
        m_lock=0;
        updateText();
    }
};


#endif // CNODETI_H
