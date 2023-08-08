#ifndef CVIEWPORTTI_H
#define CVIEWPORTTI_H

#include "CJob.h"

//========================================
//   CViewportTI
//

class CViewportTI : public QTreeWidgetItem
{
public:
    jobnet::CHClass<jobnet::C2DPrintViewport> m_viewport;
//    jobnet::CJob *m_host;

    jobnet::C2DPrintViewport *viewport(){ return m_viewport.Lock();  };
    virtual jobnet::C2DPrintPage *page(){ return 0;  };
    //virtual jobnet::C2DPrintPage *page(){ return m_viewport.Lock();  };
    //virtual void changed(unsigned column)=0;
    virtual void updateText()=0;
    CViewportTI(jobnet::CHClass<jobnet::C2DPrintViewport> &vp):QTreeWidgetItem()
    {
        m_viewport=vp;
    }
};

class CViewportMainTI : public CViewportTI
{
public:
    void updateText()
    {
        setText(0,m_viewport->m_name);
    }

    CViewportMainTI(jobnet::CHClass<jobnet::C2DPrintViewport> &vp):CViewportTI(vp)
    {
        updateText();
    }
};

#endif // CVIEWPORTTI_H
