#ifndef C2DPRINTVIEW_H
#define C2DPRINTVIEW_H

#include <QWidget>
#include <QCursor>
#include "CJob.h"

class MainWindow;

class C2DPrintView : public QWidget
{
    Q_OBJECT
public:
    MainWindow     *m_pmw;
    jobnet::CJob   *m_job;

    QPointF       m_pview;
    double        m_zoom;

    unsigned      m_drag;
    QPoint        m_drag_vp;
    QPointF       m_drag_wp;
    QPointF       m_drag_sp;

    QImage        m_tool_view;

    jobnet::print_flags m_flags;

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

    QCursor m_cur_arrow;
    QCursor m_cur_move;
    QCursor m_cur_ud;
    QCursor m_cur_lr;


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

    explicit C2DPrintView(MainWindow *mw,jobnet::CJob *job,QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // C2DPRINTVIEW_H
