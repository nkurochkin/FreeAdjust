#include "c2dpageview.h"
#include "c2dviewporttranslator.h"

#include "mainwindow.h"
#include <QPainter>
#include <qevent.h>
//#include <QPageSetupDialog>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

double distance(QPointF &p1,QPointF &p2);
double distance2(QPointF &p1,QPointF &p2);

C2DPageView::C2DPageView(MainWindow *mw,jobnet::CJob *job,QWidget *parent):QWidget(parent)
{
    m_pmw=mw;
    m_job=job;
//    m_ui =ui;
    m_pview.rx()=0;
    m_pview.ry()=0;
    m_zoom      =1.;
    m_drag      =0;
    m_flags.m_dwFlags   =0x9;
    m_page_num          =0;

    m_tool_view.load(":/icons/2dv_view");
    setMouseTracking(true);
    m_cursor_mode=PWC_ARROW;
}

#define BTN_SIZE 18
#define BTN_COUNT 7

void C2DPageView::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::MidButton)
    {
        m_drag_vp=ev->pos();
        m_drag_wp=m_pview;
        m_drag=1;
        m_cursor_mode=PWC_ARROW;
        setCursor(Qt::ClosedHandCursor);
    }else
    if(ev->button()==Qt::LeftButton)
    {
        // попадание в кнопки
        {
            QPoint pp=ev->pos();
            if(pp.y()<BTN_SIZE)
            {
                if(pp.x()<(BTN_COUNT*BTN_SIZE))
                {
                    if(m_job->m_curr_view.Exist())
                        m_flags.m_dwFlags=m_job->m_curr_view->m_flags.m_dwFlags;
                    unsigned bp=pp.x()/BTN_SIZE;
                    bool do_update=false;
                    switch(bp)
                    {
                    case 0: m_flags.m_fShowNames^=1; do_update=1; break;
                    case 1: m_flags.m_fShowHeights^=1; do_update=1; break;
                    case 2: m_flags.m_fShowSitPoints^=1; do_update=1; break;
                    case 3: m_flags.m_fShowEllipses^=1; do_update=1; break;
                    case 5:
                        if(m_page_num)
                        {
                            m_page_num--;
                            if(m_job->m_curr_view.Exist())
                            {
                                unsigned pq=m_job->m_curr_view->m_pages.GetQ();
                                if(pq)
                                {
                                    if(m_page_num>pq)
                                        m_page_num=pq-1;
                                }else
                                    m_page_num=0;
                            }
                            do_update=1;
                        }
                        break;
                    case 6:
                        if(m_job->m_curr_view.Exist())
                        {
                            unsigned pq=m_job->m_curr_view->m_pages.GetQ();
                            if(pq)
                            {
                                if(m_page_num+1<pq)
                                {
                                    m_page_num++;
                                    do_update=1;
                                }
                            }
                        }
                    }
                    if(m_job->m_curr_view.Exist())
                        m_job->m_curr_view->m_flags.m_dwFlags=m_flags.m_dwFlags;
                    if(do_update)
                        update();
                    return;
                }
            }
        }

        m_drag_vp=ev->pos();
        switch(m_cursor_mode)
        {
        case PWC_ARROW:
            m_drag_wp=m_pview;
            m_drag=1;
            setCursor(Qt::ClosedHandCursor);
            break;
        case PWC_VP_MOVE_VIEW:
            m_drag_wp=m_job->m_curr_view->m_pos;
            m_drag=1;
            break;
        case PWC_VP_TOP:
            m_drag_wp=m_job->m_curr_view->m_size;
            m_drag=1;
            break;
        case PWC_VP_BOTTOM:
            m_drag_wp=m_job->m_curr_view->m_pos;
            m_drag_sp=m_job->m_curr_view->m_size;
            m_drag=1;
            break;
        case PWC_VP_LEFT:
            m_drag_wp=m_job->m_curr_view->m_pos;
            m_drag_sp=m_job->m_curr_view->m_size;
            m_drag=1;
            break;
        case PWC_VP_RIGHT:
            m_drag_wp=m_job->m_curr_view->m_size;
            m_drag=1;
            break;
        }
    }
}

void C2DPageView::mouseReleaseEvent(QMouseEvent *ev)
{
    ev;
    if(m_drag)
    {
        m_drag=0;
        m_cursor_mode=PWC_ARROW;
        setCursor(Qt::ArrowCursor);
    }
}

void C2DPageView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    ev;
}

void C2DPageView::wheelEvent(QWheelEvent *ev)
{
    if(!m_drag)
    {
        if(ev->delta()>0)
            zoomIn(); //m_zoom*=2.;
        else
            zoomOut(); //m_zoom*=0.5;
        update();
    }
}

void C2DPageView::mouseMoveEvent(QMouseEvent *ev)
{
    if(m_drag)
    {
        QPoint pos=ev->pos();
        double dx=pos.x()-m_drag_vp.x();
        double dy=pos.y()-m_drag_vp.y();

        switch(m_cursor_mode)
        {
        case PWC_ARROW:
            m_pview.ry()=m_drag_wp.y()-dx/m_zoom;
            m_pview.rx()=m_drag_wp.x()+dy/m_zoom;
            break;
        case PWC_VP_MOVE_VIEW:
            {
                m_job->m_curr_view->m_pos.rx()=m_drag_wp.x()-dy/m_zoom;
                m_job->m_curr_view->m_pos.ry()=m_drag_wp.y()+dx/m_zoom;
                m_job->m_curr_view->update();
            }
            break;
        case PWC_VP_TOP:
            {
                double val=m_drag_wp.x()-dy/m_zoom;
                if(val<0.)
                    val=0;
                m_job->m_curr_view->m_size.rx()=val;
                m_job->m_curr_view->update();
            }
            break;
        case PWC_VP_BOTTOM:
            {
                double delta=dy/m_zoom;
                double sv=m_drag_sp.x()+delta;
                if(sv<0.)
                    delta-=sv;
                m_job->m_curr_view->m_pos.rx()=m_drag_wp.x()-delta;
                m_job->m_curr_view->m_size.rx()=m_drag_sp.x()+delta;
                m_job->m_curr_view->update();
            }
            break;
        case PWC_VP_LEFT:
            {
                double delta=dx/m_zoom;
                double sv=m_drag_sp.y()-delta;
                if(sv<0.)
                    delta+=sv;
                m_job->m_curr_view->m_pos.ry()=m_drag_wp.y()+delta;
                m_job->m_curr_view->m_size.ry()=m_drag_sp.y()-delta;
                m_job->m_curr_view->update();
            }
            break;
        case PWC_VP_RIGHT:
            {
                double val=m_drag_wp.y()+dx/m_zoom;
                if(val<0.)
                    val=0;
                m_job->m_curr_view->m_size.ry()=val;
                m_job->m_curr_view->update();
            }
            break;
        }
        update();
    }else
    {
        PW_CURSOR_ENUM cmode;
        cmode=PWC_ARROW;
        if(m_job->m_curr_view.Exist())
        {
            QPoint pos=ev->pos();
            double h=(double)height();
            double w=(double)width();
            pos.ry()=h-pos.y();
            double ly=m_pview.y()-(w*0.5)/m_zoom;
            double lx=m_pview.x()-(h*0.5)/m_zoom;

            jobnet::CHClass<jobnet::C2DPrintViewport>  vp=m_job->m_curr_view;
            QPointF lpos((vp->m_pos.y()-ly)*m_zoom,(vp->m_pos.x()-lx)*m_zoom);
            QPointF hpos((vp->m_pos.y()+vp->m_size.y()-ly)*m_zoom,(vp->m_pos.x()+vp->m_size.x()-lx)*m_zoom);

            if(pos.y()>lpos.y() && pos.y()<hpos.y())
            {
                if(fabs(pos.x()-lpos.x())<3)
                    cmode=PWC_VP_LEFT;
                else
                if(fabs(pos.x()-hpos.x())<3)
                    cmode=PWC_VP_RIGHT;
                else
                if(pos.x()>lpos.x() && pos.x()<hpos.x())
                    cmode=PWC_VP_MOVE_VIEW;
            }else
            if(pos.x()>lpos.x() && pos.x()<hpos.x())
            {
                if(fabs(pos.y()-lpos.y())<3)
                    cmode=PWC_VP_BOTTOM;
                else
                if(fabs(pos.y()-hpos.y())<3)
                    cmode=PWC_VP_TOP;
                else
                if(pos.y()>lpos.y() && pos.y()<hpos.y())
                    cmode=PWC_VP_MOVE_VIEW;
            }
        }
        if(cmode!=m_cursor_mode)
        {
            switch(cmode)
            {
            case PWC_ARROW:
                setCursor(Qt::ArrowCursor);
                break;
            case PWC_VP_MOVE_VIEW:
                setCursor(Qt::SizeAllCursor);
                break;
            case PWC_VP_TOP:
            case PWC_VP_BOTTOM:
                setCursor(Qt::SizeVerCursor);
                break;
            case PWC_VP_LEFT:
            case PWC_VP_RIGHT:
                setCursor(Qt::SizeHorCursor);
                break;
            }
            m_cursor_mode=cmode;
        }
    }
}


//#define QP_TO_SCR(v) { v.rx()=(v.x()-ly)*m_zoom; v.ry()=h-(v.y()-lx)*m_zoom; }
void C2DPageView::paintEvent ( QPaintEvent * event )
{
    event;
    QPainter dc(this);
    double scale=500.;
    if(m_job->m_curr_view.Exist())
        scale=m_job->m_curr_view->m_scale;
    double gsize=0.1*scale;
    paintAdjusted(dc,gsize);
    //switch(m_mode)
    //{
    //case 0: paintAdjusted(dc,gsize); break;
    //case 1: paintUnadjusted(dc,gsize); break;
    //}

    // Рисуем кнопари
    {
        QBrush   br_bl(QColor(0xDF,0xDF,0xDF,0xFF));
        QPen     pen_w(QColor(0x80,0x80,0x80,0xFF));
        unsigned i=0;
        if(m_job->m_curr_view.Exist())
            m_flags.m_dwFlags=m_job->m_curr_view->m_flags.m_dwFlags;
        dc.setPen(pen_w);
        dc.setBrush(br_bl);
        dc.setRenderHint(QPainter::Antialiasing, 0);
        if(m_flags.m_fShowNames)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(0,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(16,0,16,16));

        if(m_flags.m_fShowHeights)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(32,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(48,0,16,16));

        if(m_flags.m_fShowSitPoints)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(64,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(80,0,16,16));

        if(m_flags.m_fShowEllipses)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(96,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(112,0,16,16));

        if(m_job->m_curr_view.Exist())
        {
            unsigned pq=m_job->m_curr_view->m_pages.GetQ();
            if(m_page_num)
                dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128   ,0,16,16));
            else
                dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128+16,0,16,16));
            if(m_page_num+1<pq)
                dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128+32,0,16,16));
            else
                dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128+48,0,16,16));
        }else
        {
            dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128+16,0,16,16));
            dc.drawImage(QRect(1+16+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(128+48,0,16,16));
        }
    }
    {
        QPen     pen_height(QColor(0x00,0x00,0xFF,0xFF));
        QString text;
        if(gsize>=1)
            text.sprintf("%.0f м",gsize);
        else
            text.sprintf("%.2f м",gsize);
        dc.setPen(pen_height);
        dc.drawText(width()-40,16,text);
    }
}

extern void paintGreed(QPainter &dc,jobnet::C2DViewportTranslator &vt,double gsize,double lx,double ly,double dx,double dy);

#define DIR_DISTANCE 500.

void C2DPageView::paintAdjusted( QPainter &dc,double gsize )
{
    QBrush   br_bg(QColor(0xBF,0xBF,0xBF,0xFF));

    dc.setRenderHint(QPainter::Antialiasing, 0);
    double h=(double)height();
    double w=(double)width();
    dc.fillRect(0,0,w,h,br_bg);

    double scale=500;
    jobnet::CHClass<jobnet::C2DPrintPage> page;
    if(m_job->m_curr_view.Exist())
    {
        scale    =m_job->m_curr_view->m_scale;
        jobnet::CHClass<jobnet::C2DPrintViewport> vp=m_job->m_curr_view;
        if(m_page_num<vp->m_pages.GetQ())
        {
            page=vp->m_pages[m_page_num];
            jobnet::C2DViewportTranslator vt(m_pview.y()-(w*0.5)/m_zoom,m_pview.x()-(h*0.5)/m_zoom,1./m_zoom,h,scale);
            QBrush   br_page(QColor(0xFF,0xFF,0xFF,0xFF));
            QPen     pen_page(QColor(0x80,0x80,0x80,0xFF));
            dc.setPen(pen_page);
            dc.setBrush(br_page);
            QPolygonF poly;
            poly.reserve(5);

            double ml=vp->m_mgn_left*0.001*scale;
            double mr=vp->m_mgn_right*0.001*scale;
            double mt=vp->m_mgn_top*0.001*scale;
            double mb=vp->m_mgn_bottom*0.001*scale;

            poly.append(vt.ne_xy(-mb,-ml));
            poly.append(vt.ne_xy(-mb,page->m_size.y()+mr));
            poly.append(vt.ne_xy(page->m_size.x()+mt,page->m_size.y()+mr));
            poly.append(vt.ne_xy(page->m_size.x()+mt,-ml));
            poly.append(vt.ne_xy(-mb,-ml));
            dc.drawPolygon(poly);
            dc.drawPolyline(poly);

            poly.clear();
            poly.reserve(5);
            poly.append(vt.ne_xy(0.,0.));
            poly.append(vt.ne_xy(0.,page->m_size.y()));
            poly.append(vt.ne_xy(page->m_size.x(),page->m_size.y()));
            poly.append(vt.ne_xy(page->m_size.x(),0.));
            poly.append(vt.ne_xy(0.,0.));

            {
                QPen pen_border(QColor(0xBF,0xBF,0xBF,0xFF));
                dc.setPen(pen_border);
                dc.drawPolyline(poly);
            }

            /*
            QRectF pr=vp->m_printer.pageRect(QPrinter::Millimeter);
            poly.clear();
            poly.reserve(5);
            {
                double n=pr.y()*0.001*scale-mb;
                double e=pr.x()*0.001*scale-ml;
                double dn=pr.height()*0.001*scale;
                double de=pr.width()*0.001*scale;

                poly.append(vt.ne_xy(n,e));
                poly.append(vt.ne_xy(n,e+de));
                poly.append(vt.ne_xy(n+dn,e+de));
                poly.append(vt.ne_xy(n+dn,e));
                poly.append(vt.ne_xy(n,e));
            }

            {
                QPen pen_border(QColor(0x00,0x00,0xBF,0xFF));
                dc.setPen(pen_border);
                dc.drawPolyline(poly);
            }
            */

            jobnet::C2DViewportTranslator vt2(m_pview.y()-(w*0.5)/m_zoom+page->m_pos.y(),m_pview.x()-(h*0.5)/m_zoom+page->m_pos.x(),1./m_zoom,h,scale);
            dc.setClipRect(QRectF(vt.ne_xy(page->m_size.x(),0.),QSizeF(vt.wl_dl(page->m_size.y()),vt.wl_dl(page->m_size.x()))));
            paintGreed(dc,vt2,0.1*scale,page->m_pos.x(),page->m_pos.y(),page->m_size.x(),page->m_size.y());

            dc.setRenderHint(QPainter::Antialiasing, 1);
            m_job->DrawNet(dc,vt2,m_job->m_curr_view.m_val,page);
            dc.setClipping(false);
        }
    }
}

void C2DPageView::zoomIn()
{
    if(m_zoom<32768.)
    {
        m_zoom*=2.;
        update();
    }
}

void C2DPageView::zoomOut()
{
    if(m_zoom>0.001)
    {
        m_zoom*=0.5;
        update();
    }
}

void C2DPageView::zoom(double val)
{
    m_zoom=val;
    update();
}

void C2DPageView::setView(double x,double y)
{
    m_pview.rx()=x;
    m_pview.ry()=y;
    update();
}


namespace jobnet
{

bool C2DPrintViewport::pagePrint(CJob &job,unsigned page_num)
{
    if(page_num<m_pages.GetQ())
    {
        jobnet::CHClass<jobnet::C2DPrintPage> page;
        page=m_pages[page_num];

        QPainter dc;
        dc.begin(&m_printer);

        QRectF pr=m_printer.paperRect();
        QRectF pgr=m_printer.pageRect();

        double vz=m_page_size.x()/pgr.height();
        //double hz=m_page_size.y()/pgr.width();
        double h=pgr.height();

        jobnet::C2DViewportTranslator vt(page->m_pos.y(),page->m_pos.x(),vz*m_scale,h,m_scale);
//        dc.setClipRect(QRectF(QPointF(ml,mb),QSizeF(pr.width()-ml-mr,h-mb-mt)));
        dc.setRenderHint(QPainter::Antialiasing, 0);
        paintGreed(dc,vt,0.1*m_scale,page->m_pos.x(),page->m_pos.y(),page->m_size.x(),page->m_size.y());
        dc.setRenderHint(QPainter::Antialiasing, 1);
        job.DrawNet(dc,vt,this,page);
//        dc.setClipping(false);
        dc.end();
    }
    return true;
}

}
