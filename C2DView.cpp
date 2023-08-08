#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <qevent.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

#include "CNodeTI.h"
#include "CStationTI.h"
#include "CObservTI.h"


double distance(QPointF &p1,QPointF &p2)
{
    double dx=p2.x()-p1.x();
    double dy=p2.y()-p1.y();
    return sqrt(dx*dx+dy*dy);
}

double distance2(QPointF &p1,QPointF &p2)
{
    double dx=p2.x()-p1.x();
    double dy=p2.y()-p1.y();
    return dx*dx+dy*dy;
}

C2DView::C2DView(jobnet::CJob *job,Ui::MainWindow *ui)
{
    m_job=job;
    m_ui =ui;
    m_pview.rx()=0;
    m_pview.ry()=0;
    m_uview.rx()=0;
    m_uview.ry()=0;
    m_zoom=1.;
    m_drag=0;
    m_dwFlags=0x9;
    m_mode   =0;

    m_tool_view.load(":/icons/2dv_view");
}

#define BTN_SIZE 18
#define BTN_COUNT 4

void C2DView::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::MidButton)
    {
        m_drag=1;
        m_drag_vp=ev->pos();
        if(!m_mode)
            m_drag_wp=m_pview;
        else
            m_drag_wp=m_uview;
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
                    switch(bp)
                    {
                    case 0: m_fShowNames^=1; break;
                    case 1: m_fShowHeights^=1; break;
                    case 2: m_fShowSitPoints^=1; break;
                    case 3: m_fShowEllipses^=1; break;
                    }
                    update();
                    return;
                }
            }
        }

        m_drag=1;
        m_drag_vp=ev->pos();
        if(!m_mode)
            m_drag_wp=m_pview;
        else
            m_drag_wp=m_uview;
    }
}

void C2DView::mouseReleaseEvent(QMouseEvent *ev)
{
    ev;
    if(m_drag)
    {
        m_drag=0;
    }
}

void C2DView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    ev;
}

void C2DView::wheelEvent(QWheelEvent *ev)
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

void C2DView::mouseMoveEvent(QMouseEvent *ev)
{
    if(m_drag)
    {
        QPoint pos=ev->pos();
        double dx=pos.x()-m_drag_vp.x();
        double dy=pos.y()-m_drag_vp.y();

        if(!m_mode)
        {
            m_pview.ry()=m_drag_wp.y()-dx/m_zoom;
            m_pview.rx()=m_drag_wp.x()+dy/m_zoom;
        }else
        {
            m_uview.ry()=m_drag_wp.y()-dx/m_zoom;
            m_uview.rx()=m_drag_wp.x()+dy/m_zoom;
        }
        update();
    }
}


#define QP_TO_SCR(v) { QPointF t((v.y()-ly)*m_zoom,h-(v.x()-lx)*m_zoom); v=t; }

void C2DView::paintEvent ( QPaintEvent * event )
{
    event;
    QPainter dc(this);
    double gsize=0.01;
    while((gsize*m_zoom)<10)
        gsize*=10.;

    if(!m_drag) // Можно менять режим
    {
        m_mode=0;
        QList<QTableWidgetItem *> list=m_ui->StationsWidget->selectedItems();
        if(list.size())
        {
            jobnet::CNode *n=((CStationTI *)list[0])->m_st->m_node;
            if(!n->m_faX || !n->m_faY)
            {
                m_station=((CStationTI *)list[0])->m_st;
                m_station->previewCompute();
                m_mode=1;
            }
        }
    }
    switch(m_mode)
    {
    case 0: paintAdjusted(dc,gsize); break;
    case 1: paintUnadjusted(dc,gsize); break;
    }

    // Рисуем кнопари
    {
        QBrush   br_bl(QColor(0xDF,0xDF,0xDF,0xFF));
        QPen     pen_w(QColor(0x80,0x80,0x80,0xFF));
        unsigned i=0;
        dc.setPen(pen_w);
        dc.setBrush(br_bl);
        dc.setRenderHint(QPainter::Antialiasing, 0);
        if(m_fShowNames)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(0,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(16,0,16,16));

        if(m_fShowHeights)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(32,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(48,0,16,16));

        if(m_fShowSitPoints)
        {
            dc.drawRect(BTN_SIZE*i,0,BTN_SIZE,BTN_SIZE);
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(64,0,16,16));
        }else
            dc.drawImage(QRect(1+BTN_SIZE*i++,1,16,16),m_tool_view,QRect(80,0,16,16));

        if(m_fShowEllipses)
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

void C2DView::paintGreed(QPainter &dc,double gsize,double lx,double ly)
{ // Рисуем сетку на фоне
    QPen     pen_line1(QColor(0x80,0x80,0x80,0x40));
    QPen     pen_line2(QColor(0x80,0x80,0x80,0x80));
    double h=(double)height();
    double w=(double)width();

    {// Вертикальная сетка
        int    ii=ceil(ly/gsize);
        double ily=ii*gsize;
        double iry=floor((ly+w/m_zoom)/gsize)*gsize;
        while(ily<=iry)
        {
            double x=(ily-ly)*m_zoom;
            if(ii%5)
                dc.setPen(pen_line1);
            else
                dc.setPen(pen_line2);
            dc.drawLine(x,0,x,h);
            ily+=gsize;
            ii++;
        }
    }
    {// Горизонтальная сетка
        dc.setPen(pen_line1);
        int    ii=ceil(lx/gsize);
        double ilx=ii*gsize;
        double irx=floor((lx+h/m_zoom)/gsize)*gsize;
        while(ilx<=irx)
        {
            double y=h-(ilx-lx)*m_zoom;
            if(ii%5)
                dc.setPen(pen_line1);
            else
                dc.setPen(pen_line2);
            dc.drawLine(0,y,w,y);
            ilx+=gsize;
            ii++;
        }
    }
}

#define DIR_DISTANCE 500.

void C2DView::paintUnadjusted( QPainter &dc,double gsize )
{
    //QBrush   bg_br(QColor(0xFF,0xFF,0xFF,0xFF));
    QBrush   br_w(QColor(0xFF,0xFF,0xFF,0xFF));
    QBrush   br_r(QColor(0xFF,0x00,0x00,0xFF));
    QBrush   br_y(QColor(0xFF,0xFF,0x00,0xFF));

    QPen     pen_pic(QColor(0x80,0x80,0x80,0xFF));
    QBrush   bg_br(QColor(0xF0,0xFF,0xFF,0xFF));
    QPen     pen_st(QColor(0x00,0x00,0xFF,0xFF));
    QPen     pen_sit(QColor(0x00,0x00,0x00,0x80));
    QPen     pen_dir(QColor(0xFF,0x00,0x00,0xFF));
    QPen     pen_hod(QColor(0x00,0x00,0xFF,0xFF));
    QPen     pen_name(QColor(0x00,0x00,0x00,0xFF));
    QPen     pen_height(QColor(0x00,0x00,0xFF,0xFF));

    {
        QVector<qreal> dashes;
        dashes << 4 << 3;
        pen_sit.setDashPattern(dashes);
        pen_dir.setDashPattern(dashes);
    }

    dc.setRenderHint(QPainter::Antialiasing, 0);
    double h=(double)height();
    double w=(double)width();
    dc.fillRect(0,0,w,h,bg_br);
    double ly=m_uview.y()-(w*0.5)/m_zoom;
    double lx=m_uview.x()-(h*0.5)/m_zoom;
    int i;

    paintGreed(dc,gsize,lx,ly);
    dc.setRenderHint(QPainter::Antialiasing, 1);

    if(!m_station.Exist())
        return;

    // прорисовка выбранных измерений
    {
        QBrush   br_sel(QColor(0x00,0xFF,0x00,0xFF));
        QList<QTreeWidgetItem *> list=m_ui->ObservationsWidget->selectedItems();
        if(list.size())
        {
            QPointF xy1;
            QPen p_sel(br_sel,3,Qt::SolidLine,Qt::RoundCap);
            xy1.rx()=0.;
            xy1.ry()=0.;
            QP_TO_SCR(xy1);
            for(i=0;i<list.size();i++)
            {
                jobnet::CHClass<jobnet::CStationNode> stn=((CObservTI *)list[i])->m_stnode;
                if(stn->m_st==m_station)
                {
                    QPointF xy2;
                    if(stn->m_fXY)
                    {
                        xy2.rx()=stn->m_pv_pos.x;
                        xy2.ry()=stn->m_pv_pos.y;
                        QP_TO_SCR(xy2);
                        dc.setBrush(br_sel);
                        dc.setPen(pen_st);
                        dc.drawEllipse(xy2.x()-6,xy2.y()-6,13,13);

                        dc.setPen(p_sel);
                        dc.drawLine(xy1,xy2);
                    }else
                    if(stn->m_fdir)
                    {
                        xy2.rx()=cos(stn->m_pvdir)*DIR_DISTANCE;
                        xy2.ry()=sin(stn->m_pvdir)*DIR_DISTANCE;
                        QP_TO_SCR(xy2);
                        dc.setBrush(br_sel);
                        dc.setPen(pen_st);
                        dc.drawEllipse(xy2.x()-6,xy2.y()-6,13,13);

                        dc.setPen(p_sel);
                        dc.drawLine(xy1,xy2);
                    }
               }
            }
        }
    }
    //*****   Измерения   ***********
    {
        QPointF st_xy;
        st_xy.rx()=0.;
        st_xy.ry()=0.;
        QP_TO_SCR(st_xy);
        for(i=0;i<m_station->m_observations.size();i++)
        {
            jobnet::CHClass<jobnet::CStationNode> stn=m_station->m_observations[i];
            if(stn->m_fActive)
            {
                if(stn->m_fXY)
                {
                    QPointF stn_xy;
                    stn_xy.ry()=stn->m_pv_pos.y;
                    stn_xy.rx()=stn->m_pv_pos.x;
                    QP_TO_SCR(stn_xy);
                    if(stn->m_node->toAdjust(1))
                    {
                        dc.setPen(pen_hod);
                    }else
                    {
                        //if(!m_fShowSitPoints)
                        //    continue;
                        dc.setPen(pen_sit);
                        //dc.setPen(Qt::DashLine);
                    }
                    dc.drawLine(st_xy,stn_xy);
                }else
                if(stn->m_fdir)
                {
                    QPointF stn_xy;
                    stn_xy.rx()=cos(stn->m_pvdir)*DIR_DISTANCE;
                    stn_xy.ry()=sin(stn->m_pvdir)*DIR_DISTANCE;
                    QP_TO_SCR(stn_xy);
                    dc.setPen(pen_dir);
                    dc.drawLine(st_xy,stn_xy);
                }
            }
        }
        // Станция
        {
            dc.setPen(pen_st);
            if(m_station->m_node->m_fX && m_station->m_node->m_fY)
                dc.setBrush(br_r);
            else
                dc.setBrush(br_y);
            dc.drawEllipse(st_xy.x()-3,st_xy.y()-3,7,7);
            dc.setPen(pen_name);
            dc.drawText(st_xy.x()+6,st_xy.y()+6,m_station->m_node->m_id);
        }
        for(i=0;i<m_station->m_observations.size();i++)
        {
            jobnet::CHClass<jobnet::CStationNode> stn=m_station->m_observations[i];
            if(stn->m_fActive && (stn->m_fXY || stn->m_fdir))
            {
                QPointF stn_xy;
                if(stn->m_fXY)
                {
                    stn_xy.ry()=stn->m_pv_pos.y;
                    stn_xy.rx()=stn->m_pv_pos.x;
                }else
                {
                    stn_xy.rx()=cos(stn->m_pvdir)*DIR_DISTANCE;
                    stn_xy.ry()=sin(stn->m_pvdir)*DIR_DISTANCE;
                }
                QP_TO_SCR(stn_xy);
                if(stn->m_node->m_stations)
                {
                    dc.setPen(pen_st);
                    if(stn->m_node->m_fX && stn->m_node->m_fY)
                        dc.setBrush(br_r);
                    else
                        dc.setBrush(br_y);
                    dc.drawEllipse(stn_xy.x()-3,stn_xy.y()-3,7,7);
                }else
                {
                    dc.setPen(pen_pic);
                    if(stn->m_node->m_fX && stn->m_node->m_fY)
                    {
                        dc.setBrush(br_r);
                    }else
                    if(stn->m_node->m_observations>1)
                    {
                        dc.setBrush(br_w);
                    }else
                    {
                        //if(!m_fShowSitPoints)
                        //    continue;
                        dc.setBrush(br_w);
                    }
                    dc.drawEllipse(stn_xy.x()-2,stn_xy.y()-2,5,5);
                }
                if(m_fShowNames || m_fShowHeights)
                {
                    unsigned offs=6;
                    if(m_fShowNames)
                    {
                        dc.setPen(pen_name);
                        dc.drawText(stn_xy.x()+6,stn_xy.y()+offs,stn->m_node->m_id);
                        offs+=10;
                    }
                    if(m_fShowHeights && stn->m_fZ)
                    {
                        QString text;
                        text.sprintf("%.3f",stn->m_pv_pos.z);
                        dc.setPen(pen_height);
                        dc.drawText(stn_xy.x()+6,stn_xy.y()+offs,text);
                    }
                }
            }
        }
    }

}

void C2DView::paintAdjusted( QPainter &dc,double gsize )
{

    QBrush   bg_br(QColor(0xFF,0xFF,0xFF,0xFF));
    QBrush   br_w(QColor(0xFF,0xFF,0xFF,0xFF));
    QBrush   br_r(QColor(0xFF,0x00,0x00,0xFF));
    QBrush   br_y(QColor(0xFF,0xFF,0x00,0xFF));

    QPen     pen_eeb(QColor(0x00,0x00,0x00,0x40));
    QPen     pen_eel(QColor(0x00,0x00,0x00,0x20));
    QBrush   br_eef(QColor(0xBF,0xBF,0xBF,0x3F));
    QBrush   br_eea(QColor(0xFF,0xFF,0x00,0x7F));
    QBrush   br_eek(QColor(0xFF,0x00,0x00,0x3F));
    QBrush   br_sel(QColor(0x00,0xFF,0x00,0xFF));
    QPen     pen_sel(br_sel,3,Qt::SolidLine,Qt::RoundCap);

    QPen     pen_pic(QColor(0x80,0x80,0x80,0xFF));
    QPen     pen_st(QColor(0x00,0x00,0xFF,0xFF));
    QPen     pen_sit(QColor(0x00,0x00,0x00,0x20));
    QBrush   br_hod(QColor(0x00,0x00,0xFF,0x40));
    QPen     pen_hod_hd(QColor(0x00,0x00,0xFF,0x40));
    QPen     pen_hod_ha(QColor(0x00,0x00,0xFF,0x40));
    QPen     pen_name(QColor(0x00,0x00,0x00,0xFF));
    QPen     pen_height(QColor(0x00,0x00,0xFF,0xFF));

    QBrush   br_err(QColor(0xFF,0x00,0x00,0x80));
    QPen     pen_hod_err(br_err,2,Qt::SolidLine,Qt::RoundCap);

    {
        QVector<qreal> dashes;
        dashes << 4 << 3;
        pen_sit.setDashPattern(dashes);
        pen_hod_ha.setDashPattern(dashes);
    }

    dc.setRenderHint(QPainter::Antialiasing, 0);
    double h=(double)height();
    double w=(double)width();
    dc.fillRect(0,0,w,h,bg_br);
    double ly=m_pview.y()-(w*0.5)/m_zoom;
    double lx=m_pview.x()-(h*0.5)/m_zoom;

    paintGreed(dc,gsize,lx,ly);
    dc.setRenderHint(QPainter::Antialiasing, 1);

    int i;
    //*****   Эллипсы ошибок   ***********
    if(m_fShowEllipses)
    for(i=0;i<m_job->m_nodes.size();i++)
    {
        jobnet::CNode *np=m_job->m_nodes[i];
        QPointF xy;
        if(np->toAdjust(1))
        { // станция
            ;
        }else
            if(!m_fShowSitPoints)
                continue;

        if(np->m_fee && np->GetXY(xy))
        {
            QP_TO_SCR(xy);
            double ea=np->ee_a;
            double eb=np->ee_b;
            double alfa=np->ee_alfa;
            dc.resetTransform();
            //dc.translate((y-ly)*m_zoom,h-(x-lx)*m_zoom);
            dc.translate(xy.x(),xy.y());
            dc.rotate(alfa*(180./M_PI));

            double sea=ea*m_zoom*0.001;
            double seb=eb*m_zoom*0.001;
            bool act_size=false;
            if(sea>20 || seb>20)
                act_size=true;

            if(act_size && np->m_feek)
            {
                double seak=np->ee_ak*m_zoom*0.001;
                double sebk=np->ee_bk*m_zoom*0.001;
                dc.setPen(pen_eeb);
                dc.setBrush(br_eek);
                dc.drawEllipse(-sebk*0.5,-seak*0.5,sebk,seak);
            }
            if(act_size)
            {
                dc.setPen(pen_eeb);
                dc.setBrush(br_eea);
                dc.drawEllipse(-seb*0.5,-sea*0.5,seb,sea);
                dc.setPen(pen_eel);
                dc.drawLine(-seb*0.5,0,seb*0.5,0);
                dc.drawLine(0,-sea*0.5,0,sea*0.5);
            }else
            {
                //double rc=50;
                //if(eb>rc || ea>rc)
                //    dc.setBrush(br_eek);
                //else
                    dc.setBrush(br_eef);
                dc.setPen(pen_eeb);
                dc.drawEllipse(-eb*0.5,-ea*0.5,eb,ea);
                dc.setPen(pen_eel);
                dc.drawLine(-eb*0.5,0,eb*0.5,0);
                dc.drawLine(0,-ea*0.5,0,ea*0.5);
            }
        }
    }
    dc.resetTransform();

    // прорисовка выбранных измерений
    {
        QList<QTreeWidgetItem *> list=m_ui->ObservationsWidget->selectedItems();
        for(i=0;i<list.size();i++)
        {
            QPointF xy1;
            if(((CObservTI *)list[i])->m_stnode->m_st->m_node->GetXY(xy1))
            {
                QP_TO_SCR(xy1);
                QPointF xy2;
                if(((CObservTI *)list[i])->m_stnode->m_node->GetXY(xy2))
                {
                    QP_TO_SCR(xy2);
                    dc.setBrush(br_sel);
                    dc.setPen(pen_st);
                    dc.drawEllipse(xy2.x()-6,xy2.y()-6,13,13);
                    //dc.drawEllipse((y2-ly)*m_zoom-6,h-(x2-lx)*m_zoom-6,13,13);

                    dc.setPen(pen_sel);
                    dc.drawLine(xy1,xy2);
                    //dc.drawLine((y1-ly)*m_zoom,h-(x1-lx)*m_zoom,(y2-ly)*m_zoom,h-(x2-lx)*m_zoom);
                }
           }
        }
    }

    //*****   Измерения   ***********
    for(i=0;i<m_job->m_stations.size();i++)
    {
        jobnet::CHClass<jobnet::CStation> st=m_job->m_stations[i];
        //double st_x,st_y;
        //if(st->m_node->GetXY(st_x,st_y))
        QPointF st_xy;
        if(st->m_node->GetXY(st_xy))
        {
            QP_TO_SCR(st_xy);
            int j;
            for(j=0;j<st->m_observations.size();j++)
            {
                jobnet::CHClass<jobnet::CStationNode> stn=st->m_observations[j];
                //double stn_x,stn_y;
                //if(stn->m_fActive && stn->m_node->GetXY(stn_x,stn_y))
                QPointF stn_xy;
                if(stn->m_fActive && stn->m_node->GetXY(stn_xy))
                {
                    QP_TO_SCR(stn_xy);
                    if(stn->m_node->toAdjust(1))
                    {
                        if(stn->m_max_error>=1.)
                        { // Подчеркивание ошибок
                            dc.setPen(pen_hod_err);
                            dc.drawLine(st_xy,stn_xy);
                        }
                        if(stn->m_fmHD)
                        {
                            dc.setPen(pen_hod_hd);
                        }else
                        {
                            dc.setPen(pen_hod_ha);
                        }
                        //dc.setPen(Qt::SolidLine);
                        double l=distance(st_xy,stn_xy);
                        if(l>60)
                        { // можно рисовать направление измерений
                            dc.resetTransform();
                            double dx=(stn_xy.x()-st_xy.x())/l;
                            double dy=(stn_xy.y()-st_xy.y())/l;
                            double alfa=atan2(dy,dx);

                            dc.translate(st_xy.x()+dx*30,st_xy.y()+dy*30);
                            dc.rotate(alfa*(180./M_PI));

                            //dc.drawLine(0,0,-8,-5);
                            //dc.drawLine(0,0,-8, 5);
                            QPainterPath path;
                            path.moveTo(-8,-5);
                            path.lineTo(0,0);
                            path.lineTo(-8, 5);
                            if(stn->m_max_error>=1.)
                                dc.fillPath(path,br_err);
                            else
                                dc.fillPath(path,br_hod);

                            dc.resetTransform();
                        }
                    }else
                    {
                        if(!m_fShowSitPoints)
                            continue;
                        dc.setPen(pen_sit);
                        //dc.setPen(Qt::DashLine);
                    }
                    dc.drawLine(st_xy,stn_xy);
                }
            }
        }
    }

 // прорисовка выбранных станций
    {
        QBrush   br_sel(QColor(0x00,0xFF,0xFF,0xFF));
        QList<QTableWidgetItem *> list=m_ui->StationsWidget->selectedItems();
        for(i=0;i<list.size();i++)
        {
            double x,y;
            if(((CStationTI *)list[i])->m_st->m_node->GetXY(x,y))
            {
                dc.setBrush(br_sel);
                dc.setPen(pen_st);
                dc.drawEllipse((y-ly)*m_zoom-6,h-(x-lx)*m_zoom-6,13,13);
            }
        }
    }

    //*****   Узлы   ***********
    for(i=0;i<m_job->m_nodes.size();i++)
    {
        jobnet::CNode *np=m_job->m_nodes[i];
        double x,y;
        if(np->GetXY(x,y))
        {
            if(np->m_stations)
            {
                dc.setPen(pen_st);
                if(np->m_fX && np->m_fY)
                    dc.setBrush(br_r);
                else
                    dc.setBrush(br_y);
                dc.drawEllipse((y-ly)*m_zoom-3,h-(x-lx)*m_zoom-3,7,7);
            }else
            {
                dc.setPen(pen_pic);
                if(np->m_fX && np->m_fY)
                {
                    dc.setBrush(br_r);
                }else
                if(np->m_observations>1)
                {
                    dc.setBrush(br_w);
                }else
                {
                    if(!m_fShowSitPoints)
                        continue;
                    dc.setBrush(br_w);
                }
                dc.drawEllipse((y-ly)*m_zoom-2,h-(x-lx)*m_zoom-2,5,5);
            }
            if(m_fShowNames || m_fShowHeights)
            {
                unsigned offs=6;
                if(m_fShowNames)
                {
                    dc.setPen(pen_name);
                    dc.drawText((y-ly)*m_zoom+6,h-(x-lx)*m_zoom+offs,np->m_id);
                    offs+=10;
                }
                double z;
                if(m_fShowHeights && np->GetZ(z))
                {
                    QString text;
                    text.sprintf("%.3f",z);
                    dc.setPen(pen_height);
                    dc.drawText((y-ly)*m_zoom+6,h-(x-lx)*m_zoom+offs,text);
                }
            }
            //QGraphicsSimpleTextItem *gsi=m_scene.addSimpleText(np->m_id);
            //gsi->setPos(y*scale+6,-x*scale+6);
        }
    }
}

void C2DView::zoomIn()
{
    if(m_zoom<32768.)
    {
        m_zoom*=2.;
        update();
    }
}

void C2DView::zoomOut()
{
    if(m_zoom>0.001)
    {
        m_zoom*=0.5;
        update();
    }
}

void C2DView::zoom(double val)
{
    m_zoom=val;
    update();
}

void C2DView::setView(double x,double y)
{
//    if(m_mode==0)
    {
        m_pview.rx()=x;
        m_pview.ry()=y;
    }/*else
    {
        m_uview.rx()=x;
        m_uview.ry()=y;
    }*/
    update();
}

void C2DView::getpExtents(double &xmin,double &ymin,double &xmax,double &ymax)
{
    double dx=((double)height()*0.5)/m_zoom;
    double dy=((double)width()*0.5)/m_zoom;
    xmin=m_pview.x()-dx;
    ymin=m_pview.y()-dy;
    xmax=xmin+dx;
    ymax=ymin+dy;
}
