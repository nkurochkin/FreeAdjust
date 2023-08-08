#include "c2dprintview.h"
#include "c2dviewporttranslator.h"
#include "mainwindow.h"
#include <QPainter>
#include <qevent.h>
#include <QPageSetupDialog>

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

double distance(QPointF &p1,QPointF &p2);
double distance2(QPointF &p1,QPointF &p2);

C2DPrintView::C2DPrintView(MainWindow *mw,jobnet::CJob *job,QWidget *parent) :
    m_cur_arrow(Qt::ArrowCursor) ,
    m_cur_move(Qt::SizeAllCursor) ,
    m_cur_ud(Qt::SplitVCursor) ,
    m_cur_lr(Qt::SplitHCursor) ,
    QWidget(parent)
{
    m_pmw=mw;
    m_job=job;
//    m_ui =ui;
    m_pview.rx()=0;
    m_pview.ry()=0;
    m_zoom      =1.;
    m_drag      =0;
    m_flags.m_dwFlags   =0x9;

    m_tool_view.load(":/icons/2dv_view");
    setMouseTracking(true);
    m_cursor_mode=PWC_ARROW;
}

#define BTN_SIZE 18
#define BTN_COUNT 4

void C2DPrintView::mousePressEvent(QMouseEvent *ev)
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
                    unsigned bp=pp.x()/BTN_SIZE;
                    if(m_job->m_curr_view.Exist())
                        m_flags.m_dwFlags=m_job->m_curr_view->m_flags.m_dwFlags;
                    switch(bp)
                    {
                    case 0: m_flags.m_fShowNames^=1;     break;
                    case 1: m_flags.m_fShowHeights^=1;   break;
                    case 2: m_flags.m_fShowSitPoints^=1; break;
                    case 3: m_flags.m_fShowEllipses^=1;  break;
                    }
                    if(m_job->m_curr_view.Exist())
                        m_job->m_curr_view->m_flags.m_dwFlags=m_flags.m_dwFlags;
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

void C2DPrintView::mouseReleaseEvent(QMouseEvent *ev)
{
    ev;
    if(m_drag)
    {
        m_drag=0;
        m_cursor_mode=PWC_ARROW;
        setCursor(Qt::ArrowCursor);
    }
}

void C2DPrintView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    ev;
}

void C2DPrintView::wheelEvent(QWheelEvent *ev)
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

void C2DPrintView::mouseMoveEvent(QMouseEvent *ev)
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
void C2DPrintView::paintEvent ( QPaintEvent * event )
{
    event;
    QPainter dc(this);
    double scale=500.;
    if(m_job->m_curr_view.Exist())
        scale=m_job->m_curr_view->m_scale;

    double gsize=0.1*scale;
    paintAdjusted(dc,gsize);

    // Рисуем кнопари
    {
        if(m_job->m_curr_view.Exist())
            m_flags.m_dwFlags=m_job->m_curr_view->m_flags.m_dwFlags;
        QBrush   br_bl(QColor(0xDF,0xDF,0xDF,0xFF));
        QPen     pen_w(QColor(0x80,0x80,0x80,0xFF));
        unsigned i=0;
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

void paintGreed(QPainter &dc,jobnet::C2DViewportTranslator &vt,double gsize,double lx,double ly,double dx,double dy)
{
    QPen     pen_line1(QColor(0x80,0x80,0x80,0xFF));
    QPen     pen_line2(QColor(0x80,0x80,0x80,0xFF));

    {// Вертикальная сетка
        int    iy=ceil(ly/gsize);
        double ily=iy*gsize;
        double iry=floor((ly+dy)/gsize)*gsize;
        while(ily<=iry)
        {
            if(iy%5)
                dc.setPen(pen_line1);
            else
                dc.setPen(pen_line2);

            int    ix=ceil(lx/gsize);
            double ilx=ix*gsize;
            double irx=floor((lx+dx)/gsize)*gsize;
            while(ilx<=irx)
            {
                QPointF p1=vt.ne_xy(ilx-0.005*vt.m_scale,ily);
                QPointF p2=vt.ne_xy(ilx+0.005*vt.m_scale,ily);
                dc.drawLine(p1,p2);
                //dc.drawLine(x,y-5,x,y+5);
                ilx+=gsize;
                ix++;
            }
            ily+=gsize;
            iy++;
        }
    }
    {// Горизонтальная сетка
        int    ix=ceil(lx/gsize);
        double ilx=ix*gsize;
        double irx=floor((lx+dx)/gsize)*gsize;
        while(ilx<=irx)
        {
            if(ix%5)
                dc.setPen(pen_line1);
            else
                dc.setPen(pen_line2);

            int    iy=ceil(ly/gsize);
            double ily=iy*gsize;
            double iry=floor((ly+dy)/gsize)*gsize;
            while(ily<=iry)
            {
                QPointF p1=vt.ne_xy(ilx,ily-0.005*vt.m_scale);
                QPointF p2=vt.ne_xy(ilx,ily+0.005*vt.m_scale);
                dc.drawLine(p1,p2);
                ily+=gsize;
                iy++;
            }
            ilx+=gsize;
            ix++;
        }
    }
}

#define DIR_DISTANCE 500.

void C2DPrintView::paintAdjusted( QPainter &dc,double gsize )
{

    QBrush   br_bg(QColor(0xBF,0xBF,0xBF,0xFF));

    dc.setRenderHint(QPainter::Antialiasing, 0);
    double h=(double)height();
    double w=(double)width();
    dc.fillRect(0,0,w,h,br_bg);
    //double ly=m_pview.y()-(w*0.5)/m_zoom;
    //double lx=m_pview.x()-(h*0.5)/m_zoom;
    double scale=500;
    if(m_job->m_curr_view.Exist())
        scale    =m_job->m_curr_view->m_scale;

    jobnet::C2DViewportTranslator vt(m_pview.y()-(w*0.5)/m_zoom,m_pview.x()-(h*0.5)/m_zoom,1./m_zoom,h,scale);

    if(m_job->m_curr_view.Exist())
    { // Зона печати
        jobnet::CHClass<jobnet::C2DPrintViewport> vp=m_job->m_curr_view;
        if(vp->m_pages.GetQ())
        {
            QBrush   br_page(QColor(0xFF,0xFF,0xFF,0xFF));
            QPen     pen_page(QColor(0x80,0x80,0x80,0xFF));
            dc.setPen(pen_page);
            dc.setBrush(br_page);
            unsigned i;
            for(i=0;i<vp->m_pages.GetQ();i++)
            {
                jobnet::CHClass<jobnet::C2DPrintPage> pp=vp->m_pages[i];
                QPolygonF poly;
                poly.reserve(5);
                poly.append(vt.ne_xy(pp->m_pos));
                poly.append(vt.ne_xy(pp->m_pos.x(),pp->m_pos.y()+pp->m_size.y()));
                poly.append(vt.ne_xy(pp->m_pos.x()+pp->m_size.x(),pp->m_pos.y()+pp->m_size.y()));
                poly.append(vt.ne_xy(pp->m_pos.x()+pp->m_size.x(),pp->m_pos.y()));
                poly.append(vt.ne_xy(pp->m_pos));

                dc.drawPolygon(poly);
            }
            for(i=0;i<vp->m_pages.GetQ();i++)
            {
                jobnet::CHClass<jobnet::C2DPrintPage> pp=vp->m_pages[i];
                QPolygonF poly;
                poly.reserve(5);
                poly.append(vt.ne_xy(pp->m_pos));
                poly.append(vt.ne_xy(pp->m_pos.x(),pp->m_pos.y()+pp->m_size.y()));
                poly.append(vt.ne_xy(pp->m_pos.x()+pp->m_size.x(),pp->m_pos.y()+pp->m_size.y()));
                poly.append(vt.ne_xy(pp->m_pos.x()+pp->m_size.x(),pp->m_pos.y()));
                poly.append(vt.ne_xy(pp->m_pos));
                dc.drawPolyline(poly);
            }
        }

        {
            QBrush   br_sel(QColor(0x00,0x00,0xFF,0x7F));
            QPen     pen_sel(br_sel,3,Qt::SolidLine,Qt::RoundCap);
            QBrush   br_bg1(QColor(0x00,0x00,0xFF,0x03));
            dc.setPen(pen_sel);
            dc.setBrush(br_bg1);
            double x,y;
            vt.ne_xy(vp->m_pos.x()+vp->m_size.x(),vp->m_pos.y(),x,y);
            dc.drawRect(x,y,vp->m_size.y()*m_zoom,vp->m_size.x()*m_zoom);
        }

        paintGreed(dc,vt,gsize,
                   m_job->m_curr_view->m_pos.x(),
                   m_job->m_curr_view->m_pos.y(),
                   m_job->m_curr_view->m_size.x(),
                   m_job->m_curr_view->m_size.y()
                   );
    }

    dc.setRenderHint(QPainter::Antialiasing, 1);

    m_job->DrawNet(dc,vt,m_job->m_curr_view.m_val,0);
}

void C2DPrintView::zoomIn()
{
    if(m_zoom<32768.)
    {
        m_zoom*=2.;
        update();
    }
}

void C2DPrintView::zoomOut()
{
    if(m_zoom>0.001)
    {
        m_zoom*=0.5;
        update();
    }
}

void C2DPrintView::zoom(double val)
{
    m_zoom=val;
    update();
}

void C2DPrintView::setView(double x,double y)
{
    m_pview.rx()=x;
    m_pview.ry()=y;
    update();
}

//

#include "clipLine2D.h"

namespace jobnet
{

C2DPrintViewport::C2DPrintViewport(QString &name,QPointF &pos,QPointF &size)
{
    m_pos      =pos;
    m_size     =size;
    m_name     =name;
    m_scale    =2000.;
    m_pages_dx =0;
    m_pages_dy =0;
    m_page_size.setX(0.);
    m_page_size.setY(0.);
    m_page_align_grid.setX(100.);
    m_page_align_grid.setY(100.);
    m_mgn_top    =10.;
    m_mgn_bottom =10.;
    m_mgn_left   =10.;
    m_mgn_right  =10.;
    m_ellipse_scale  =1000.;
    m_flags.m_dwFlags=0x9;
}

#define  PAGE_COUNT_LIMIT  100

void C2DPrintViewport::update()
{
    QSizeF size=m_printer.paperSize(QPrinter::Millimeter);
    m_printer.getPageMargins(&m_mgn_left,&m_mgn_top,&m_mgn_right,&m_mgn_bottom,QPrinter::Millimeter);
    m_page_size.ry()=(size.width()-m_mgn_left-m_mgn_right)*0.001;
    m_page_size.rx()=(size.height()-m_mgn_top-m_mgn_bottom)*0.001;
    {
        m_pages.Clear();
        double px=m_page_size.x()*m_scale;
        double py=m_page_size.y()*m_scale;
        double xmax=m_pos.x()+m_size.x();
        double ymax=m_pos.y()+m_size.y();

        unsigned index=0;
        double x=m_pos.x();
        double tx;
        if(m_page_align_grid.x()!=0. && px>m_page_align_grid.x())
            x=floor(x/m_page_align_grid.x())*m_page_align_grid.x();
        do
        {
            double y=m_pos.y();
            double ty;
            if(m_page_align_grid.y()!=0. && py>m_page_align_grid.y())
                y=floor(y/m_page_align_grid.y())*m_page_align_grid.y();
            do
            {
                CHClass<C2DPrintPage> pp=new C2DPrintPage(this,QPointF(x,y),QPointF(px,py),index++);
                m_pages.Append(pp);

                y+=py;
                ty=y;
                if(m_page_align_grid.y()!=0. && py>m_page_align_grid.y())
                    y=floor(y/m_page_align_grid.y())*m_page_align_grid.y();
            }while(ty<ymax && index<PAGE_COUNT_LIMIT);
            x+=px;
            tx=x;
            if(m_page_align_grid.x()!=0. && px>m_page_align_grid.x())
                x=floor(x/m_page_align_grid.x())*m_page_align_grid.x();
        }while(tx<xmax && index<PAGE_COUNT_LIMIT);
    }
}

bool C2DPrintViewport::pageSetupDlg(QWidget *parent)
{
    QPageSetupDialog dlg(&m_printer,parent);
    dlg.exec();
    update();
    return true;
}

void C2DPrintViewport::setExtents(double xmin,double ymin,double xmax,double ymax)
{
    m_pos.setX(xmin);
    m_pos.setY(ymin);
    m_size.setX(xmax-xmin);
    m_size.setY(ymax-ymin);
}

#define TFONT_SIZE                  0.0025
#define SIGN_CONTOUR_PEN_WIDTH      0.0002
#define ELLIPSE_CONTOUR_PEN_WIDTH   0.0002
#define ELLIPSE_LINES_PEN_WIDTH     0.0001
#define OBSERVATION_LINES_PEN_WIDTH 0.0003
#define OBSERVATION_ARROW_SIZE      0.0010
#define OBSERVATION_ARROW_OFFSET    0.0100
#define TEXT_BORDER_OFFSET          0.0050

void CJob::DrawNet(QPainter &dc,C2DViewportTranslator &vt,C2DPrintViewport *vp,C2DPrintPage *pp)
{
    int i;
    QBrush   br_w(QColor(0xFF,0xFF,0xFF,0xFF));
    QBrush   br_black(QColor(0x00,0x00,0x00,0xFF));
    QPen     pen_text(QColor(0x00,0x00,0x00,0xFF));
    print_flags flags;
    flags.m_dwFlags =0xA;
    double scale    =500;
    double ell_scale=500.;
    if(vp)
    {
        flags.m_dwFlags=vp->m_flags.m_dwFlags;
        scale    =vp->m_scale;
        ell_scale=vp->m_ellipse_scale;
    }
    QPen     pen_side(br_black,vt.pl_dl(SIGN_CONTOUR_PEN_WIDTH),Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    double fsize=vt.pl_dl(TFONT_SIZE);
    if(fsize<5.)
        fsize=5.;
    QFont tfont(QString("Arial"),(int)fsize,-1,true);

    //*****   Эллипсы ошибок   ***********
    if(flags.m_fShowEllipses)
    {

        QBrush   br_pen_eeb(QColor(0x00,0x00,0x00,0xFF));
        QPen     pen_eeb(br_pen_eeb,vt.pl_dl(ELLIPSE_CONTOUR_PEN_WIDTH),Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
        QPen     pen_eel(br_pen_eeb,vt.pl_dl(ELLIPSE_LINES_PEN_WIDTH),Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
        QBrush   br_eea(QColor(0xFF,0xFF,0x7F,0x40));
        for(i=0;i<m_nodes.size();i++)
        {
            jobnet::CNode *np=m_nodes[i];
            QPointF ne;
            if(np->toAdjust(1))
            { // станция
                ;
            }else
                if(!flags.m_fShowSitPoints)
                    continue;

            if(np->m_fee && np->GetXY(ne))
            {
                double ea=np->ee_a;
                double eb=np->ee_b;
                double alfa=np->ee_alfa;
                dc.resetTransform();
                dc.translate(vt.ne_xy(ne));
                dc.rotate(alfa*(180./M_PI));

                double sea=vt.wl_dl(ea*0.001*ell_scale);
                double seb=vt.wl_dl(eb*0.001*ell_scale);

                /*if(act_size && np->m_feek)
                {
                    double seak=np->ee_ak*m_zoom*0.001*ell_zoom;
                    double sebk=np->ee_bk*m_zoom*0.001*ell_zoom;
                    dc.setPen(pen_eeb);
                    dc.setBrush(br_eek);
                    dc.drawEllipse(-sebk*0.5,-seak*0.5,sebk,seak);
                }*/
                {
                    dc.setPen(pen_eeb);
                    dc.setBrush(br_eea);
                    dc.drawEllipse(-seb*0.5,-sea*0.5,seb,sea);
                    dc.setPen(pen_eel);
                    dc.drawLine(-seb*0.5,0,seb*0.5,0);
                    dc.drawLine(0,-sea*0.5,0,sea*0.5);
                }
            }
        }
        dc.resetTransform();
    }

    //*****   Измерения   ***********
    {
        QBrush   br_hod(QColor(0x3F,0x3F,0xFF,0xFF));
        QBrush   br_sit(QColor(0x7F,0x7F,0x7F,0xFF));
        qreal    pw=vt.pl_dl(OBSERVATION_LINES_PEN_WIDTH);
        if(pw<1.)
            pw=1.;
        QPen     pen_hod_hd(br_hod,pw,Qt::SolidLine,Qt::RoundCap);
        QPen     pen_hod_ha(br_hod,pw,Qt::SolidLine,Qt::SquareCap);
        QPen     pen_sit(br_sit,pw,Qt::SolidLine,Qt::SquareCap);
        {
            int ds=vt.pl_dl(0.002)/pw;
            if(ds<4)
                ds=4;
            QVector<qreal> dashes;
            dashes << ds << ds/2;
            pen_sit.setDashPattern(dashes);
            pen_hod_ha.setDashPattern(dashes);
        }
        QPainterPath path;
        path.moveTo(-vt.pl_dl(OBSERVATION_ARROW_SIZE*1.5),-vt.pl_dl(OBSERVATION_ARROW_SIZE));
        path.lineTo(0,0);
        path.lineTo(-vt.pl_dl(OBSERVATION_ARROW_SIZE*1.5), vt.pl_dl(OBSERVATION_ARROW_SIZE));
        double ss=vt.pl_dl(OBSERVATION_ARROW_OFFSET);

        for(i=0;i<m_stations.size();i++)
        {
            jobnet::CHClass<jobnet::CStation> st=m_stations[i];
            QPointF st_ne;
            if(st->m_node->GetXY(st_ne))
            {
                QPointF st_xy;
                st_xy=vt.ne_xy(st_ne);
                int j;
                for(j=0;j<st->m_observations.size();j++)
                {
                    jobnet::CHClass<jobnet::CStationNode> stn=st->m_observations[j];
                    //double stn_x,stn_y;
                    //if(stn->m_fActive && stn->m_node->GetXY(stn_x,stn_y))
                    QPointF stn_ne;
                    if(stn->m_fActive && stn->m_node->GetXY(stn_ne))
                    {
                        QPointF stn_xy;
                        stn_xy=vt.ne_xy(stn_ne);
                        if(stn->m_node->toAdjust(1))
                        {
                            if(stn->m_fmHD)
                            {
                                dc.setPen(pen_hod_hd);
                            }else
                            {
                                dc.setPen(pen_hod_ha);
                            }
                            //dc.setPen(Qt::SolidLine);
                            double l=distance(st_ne,stn_ne)/scale;
                            if(l>OBSERVATION_ARROW_SIZE*3)
                            { // можно рисовать направление измерений
                                double sa;
                                if(l<OBSERVATION_ARROW_OFFSET*2.)
                                {
                                    l=distance(st_xy,stn_xy);
                                    sa=l*0.5;
                                }else
                                {
                                    l=distance(st_xy,stn_xy);
                                    sa=ss;
                                }

                                dc.resetTransform();
                                double dx=(stn_xy.x()-st_xy.x())/l;
                                double dy=(stn_xy.y()-st_xy.y())/l;
                                double alfa=atan2(dy,dx);

                                dc.translate(st_xy.x()+dx*sa,st_xy.y()+dy*sa);
                                dc.rotate(alfa*(180./M_PI));

                                dc.fillPath(path,br_hod);
                                dc.resetTransform();
                            }
                        }else
                        {
                            if(!flags.m_fShowSitPoints)
                                continue;
                            dc.setPen(pen_sit);
                        }
                        dc.drawLine(st_xy,stn_xy);                        
                        if(pp)
                        { // Задана страница, значит извстен видимый край
                          // Если линия частично оборвана
                            QPointF wp1,wp2;
                            int rv=clipLine2D(pp->m_pos,pp->m_size,st_ne,stn_ne,&wp1,&wp2);
                            if(rv)
                            {
                                //if((rv&FULL_OUTSIDE)==PART_OUTSIDE)
                                { // чето видно
                                    QPointF p1,p2;
                                    p1=vt.ne_xy(wp1);
                                    p2=vt.ne_xy(wp2);
                                    //if(rv&0xF)
                                    //{ // Первый конец отрезан
                                    //
                                    //}
                                    if(rv&0xF00)
                                    { // Второй конец отрезан
                                        dc.setFont(tfont);
                                        dc.resetTransform();
                                        double l=distance(p2,st_xy);
                                        double dx=(st_xy.x()-p2.x())/l;
                                        double dy=(st_xy.y()-p2.y())/l;
                                        double alfa=atan2(dy,dx);
                                        if(dx<0)
                                        {
                                            alfa+=M_PI;
                                            if(alfa>M_PI)
                                                alfa-=M_PI*2;
                                        }

                                        double sa=vt.pl_dl(TEXT_BORDER_OFFSET);
                                        //double sa=0.;

                                        dc.translate(p2.x()+dx*sa,p2.y()+dy*sa);
                                        dc.rotate(alfa*(180./M_PI));

                                        //Qt::AlignLeft
                                        dc.setPen(pen_text);
                                        //dc.drawText(0,0,stn->m_node->m_id);
                                        if(dx<0)
                                            dc.drawText(QRectF(-fsize*100.,-fsize*1.5,fsize*100.,fsize*2.),Qt::AlignRight,stn->m_node->m_id);
                                        else
                                            dc.drawText(QRectF(0,-fsize*1.5,fsize*100.,fsize*2),Qt::AlignLeft,stn->m_node->m_id);

                                        dc.resetTransform();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //*****   Узлы   ***********
    QPolygonF hd_point;
    {
        #define TR_SIZE 0.003
        QPointF p1(vt.pl_dl(-TR_SIZE*0.5),
                   vt.pl_dl(TR_SIZE*0.5*0.577));
        QPointF p2(vt.pl_dl(TR_SIZE*0.5),
                   vt.pl_dl(TR_SIZE*0.5*0.577));
        QPointF p3(0,vt.pl_dl(-TR_SIZE*0.5*1.155));
        hd_point.append(p1);
        hd_point.append(p2);
        hd_point.append(p3);
        hd_point.append(p1);
    }

    dc.setFont(tfont);
    for(i=0;i<m_nodes.size();i++)
    {
        jobnet::CNode *np=m_nodes[i];
        QPointF ne;
        if(np->GetXY(ne))
        {
            QPointF xy;
            xy=vt.ne_xy(ne);
            dc.resetTransform();
            dc.translate(vt.ne_xy(ne));
            if(np->m_fX && np->m_fY)
            { // Твердая точка
                dc.setPen(pen_side);
                dc.setBrush(br_w);
                dc.drawPolygon(hd_point);

                dc.setBrush(br_black);
                double l=vt.pl_dl(0.0002);
                double hl=l*0.5;
                QRectF ell(-hl,-hl,l,l);
                dc.drawEllipse(ell);
            }else
            if(np->m_stations)
            {
                dc.setPen(pen_side);
                dc.setBrush(br_w);
                {
                    double l=vt.pl_dl(0.0015);
                    double hl=l*0.5;
                    double l1=vt.pl_dl(0.0005);
                    QRectF ell(-hl,-hl,l,l);
                    dc.drawEllipse(ell);
                    dc.drawLine(QPointF(0,hl),QPointF(0,hl+l1));
                    dc.drawLine(QPointF(0,-hl),QPointF(0,-hl-l1));
                    dc.drawLine(QPointF(hl,0),QPointF(hl+l1,0));
                    dc.drawLine(QPointF(-hl,0),QPointF(-hl-l1,0));
                }
                dc.setBrush(br_black);
                {
                    double l=vt.pl_dl(0.0002);
                    double hl=l*0.5;
                    QRectF ell(-hl,-hl,l,l);
                    dc.drawEllipse(ell);
                }
            }else
            if(np->m_observations>1 || flags.m_fShowSitPoints)
            {
                dc.setPen(pen_side);
                dc.setBrush(br_w);
                {
                    double l=vt.pl_dl(0.0015);
                    double hl=l*0.5;
                    QRectF ell(-hl,-hl,l,l);
                    dc.drawEllipse(ell);
                }
                dc.setBrush(br_black);
                {
                    double l=vt.pl_dl(0.0002);
                    double hl=l*0.5;
                    QRectF ell(-hl,-hl,l,l);
                    dc.drawEllipse(ell);
                }
            }else
                continue;
            if(flags.m_fShowNames || flags.m_fShowHeights)
            {
                unsigned l    =vt.pl_dl(0.003);
                unsigned doffs=vt.pl_dl(TFONT_SIZE*1.25);
                unsigned offs=l;
                if(flags.m_fShowNames)
                {
                    dc.setPen(pen_text);
                    dc.drawText(l,offs,np->m_id);
                    offs+=doffs;
                }
                double z;
                if(flags.m_fShowHeights && np->GetZ(z))
                {
                    QString text;
                    text.sprintf("%.3f",z);
                    dc.setPen(pen_text);
                    dc.drawText(l,offs,text);
                }
            }
        }
    }
    dc.resetTransform();
}

}
