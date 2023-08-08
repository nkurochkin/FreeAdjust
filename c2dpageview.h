#ifndef C2DPAGEVIEW_H
#define C2DPAGEVIEW_H

#include <QWidget>
#include <QCursor>
#include "CJob.h"

class MainWindow;

class C2DPageView : public QWidget
{
public:
    MainWindow     *m_pmw;
    jobnet::CJob   *m_job;

    QPointF       m_pview;
    double        m_zoom;
    unsigned      m_page_num;

    unsigned      m_drag;
    QPoint        m_drag_vp;
    QPointF       m_drag_wp;
    QPointF       m_drag_sp;

    QImage        m_tool_view;

    jobnet::print_flags   m_flags;

    enum PW_CURSOR_ENUM{
        PWC_ARROW=0,
        PWC_VP_MOVE_VIEW,
        PWC_VP_TOP,
        PWC_VP_BOTTOM,
        PWC_VP_LEFT,
        PWC_VP_RIGHT,
        PWC_UNCKNOWN
    };

    PW_CURSOR_ENUM m_cursor_mode;

    void paintAdjusted(QPainter &dc,double gsize);

    void paintEvent ( QPaintEvent * event );
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);


    void zoomIn();
    void zoomOut();
    void zoom(double);
    void setScale(double);
    void setView(double x,double y);


    C2DPageView(MainWindow *mw,jobnet::CJob *job,QWidget *parent = 0);
};

#endif // C2DPAGEVIEW_H
