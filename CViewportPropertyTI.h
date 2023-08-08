#ifndef CVIEWPORTPROPERTY_H
#define CVIEWPORTPROPERTY_H

#include "mainwindow.h"
#include "CJob.h"

class CViewportPropertyTI : public QTableWidgetItem
{
public:
    enum TYPES_ENUM
    {
        TE_NAME=0,
        TE_SCALE,
        TE_ELLIPSE_SCALE,
        TE_PAGE_GRID_X,
        TE_PAGE_GRID_Y,
        TE_MARGIN_TOP,
        TE_MARGIN_BOTTOM,
        TE_MARGIN_LEFT,
        TE_MARGIN_RIGHT,
        TE_COUNT
    };

    int m_lock;
    MainWindow     *m_pmw;
    jobnet::CHClass<jobnet::C2DPrintViewport> m_viewport;
    TYPES_ENUM m_type;

    void updateText()
    {
        m_lock++;
        if(m_viewport.Exist())
        {
            switch(m_type)
            {
            case TE_NAME:
                setText(m_viewport->m_name);
                break;
            case TE_SCALE:
                {
                    QString text;
                    text.sprintf("%.0f",m_viewport->m_scale);
                    setText(text);
                }
                break;
            case TE_ELLIPSE_SCALE:
                {
                    QString text;
                    text.sprintf("%.0f",m_viewport->m_ellipse_scale);
                    setText(text);
                }
                break;
            case TE_PAGE_GRID_X:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_page_align_grid.x());
                    setText(text);
                }
                break;
            case TE_PAGE_GRID_Y:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_page_align_grid.y());
                    setText(text);
                }
                break;
            case TE_MARGIN_TOP:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_mgn_top);
                    setText(text);
                }
                break;
            case TE_MARGIN_BOTTOM:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_mgn_bottom);
                    setText(text);
                }
                break;
            case TE_MARGIN_LEFT:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_mgn_left);
                    setText(text);
                }
                break;
            case TE_MARGIN_RIGHT:
                {
                    QString text;
                    text.sprintf("%.3f",m_viewport->m_mgn_right);
                    setText(text);
                }
                break;
            }
        }
        m_lock--;
    }
    void changed()
    {
        if(!m_lock)
        {
            switch(m_type)
            {
            case TE_NAME:
                break;
            case TE_SCALE:
                {
                    bool res;
                    double val=text().toDouble(&res);
                    if(res && val>0.)
                    {
                        m_viewport->m_scale=val;
                        m_viewport->update();
                        m_pmw->m_ui2DPrintView->update();
                        m_pmw->m_ui2DPageView->update();
                    }
                    updateText();
                }
                break;
            case TE_ELLIPSE_SCALE:
                {
                    bool res;
                    double val=text().toDouble(&res);
                    if(res && val>0.)
                    {
                        m_viewport->m_ellipse_scale=val;
                        //m_viewport->update();
                        m_pmw->m_ui2DPrintView->update();
                        m_pmw->m_ui2DPageView->update();
                    }
                    updateText();
                }
                break;
            case TE_PAGE_GRID_X:
                {
                    bool res;
                    double val=text().toDouble(&res);
                    if(res && val>0.)
                    {
                        m_viewport->m_page_align_grid.rx()=val;
                        m_viewport->update();
                        m_pmw->m_ui2DPrintView->update();
                        m_pmw->m_ui2DPageView->update();
                    }
                    updateText();
                }
                break;
            case TE_PAGE_GRID_Y:
                {
                    bool res;
                    double val=text().toDouble(&res);
                    if(res && val>0.)
                    {
                        m_viewport->m_page_align_grid.ry()=val;
                        m_viewport->update();
                        m_pmw->m_ui2DPrintView->update();
                        m_pmw->m_ui2DPageView->update();
                    }
                    updateText();
                }
                break;
            case TE_MARGIN_TOP:
            case TE_MARGIN_BOTTOM:
            case TE_MARGIN_LEFT:
            case TE_MARGIN_RIGHT:
                break;
            }
        }
    }

    CViewportPropertyTI(MainWindow *pmw,jobnet::CHClass<jobnet::C2DPrintViewport> &vp,TYPES_ENUM type):QTableWidgetItem()
    {
        m_pmw     =pmw;
        m_viewport=vp;
        m_lock    =0;
        m_type    =type;
        updateText();
    }
};


#endif // CVIEWPORTPROPERTY_H
