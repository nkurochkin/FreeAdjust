#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeWidget>
#include <QGraphicsScene>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include "CJob.h"
#include "cangleunits.h"
#include "c3dpoint.h"
#include "chcarray.h"
#include "c2dprintview.h"
#include "c2dpageview.h"

namespace Ui {
    class MainWindow;
}

class C2DView : public QWidget
{
public:
    jobnet::CJob   *m_job;
    Ui::MainWindow *m_ui;

    QPointF       m_pview;
    QPointF       m_uview;
    double        m_zoom;

    unsigned      m_drag;
    QPoint        m_drag_vp;
    QPointF       m_drag_wp;

    jobnet::CHClass<jobnet::CStation> m_station;
    int           m_mode;

    QImage        m_tool_view;

    union{
        unsigned m_dwFlags;
        struct{
            unsigned m_fShowNames:1;
            unsigned m_fShowHeights:1;
            unsigned m_fShowSitPoints:1;
            unsigned m_fShowEllipses:1;
        };
    };

    void paintGreed(QPainter &dc,double gsize,double lx,double ly);
    void paintAdjusted(QPainter &dc,double gsize);
    void paintUnadjusted(QPainter &dc,double gsize);

    void paintEvent ( QPaintEvent * event );
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);


    void zoomIn();
    void zoomOut();
    void zoom(double);
    void setView(double x,double y);
    void getpExtents(double &xmin,double &ymin,double &xmax,double &ymax);

    C2DView(jobnet::CJob *job,Ui::MainWindow *ui);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    jobnet::CJob m_job;
    QString     m_job_file_name;
    jobnet::CHClass<jobnet::CStation> m_curr_st;
    CAngleUnits m_ha_units;
    CAngleUnits m_va_units;
    C2DView         *m_ui2DView;
    C2DPrintView    *m_ui2DPrintView;
    C2DPageView     *m_ui2DPageView;
    //QGraphicsScene m_scene;
    unsigned    m_lock_updates;
    QPrinter    m_printer;
    //QList<jobnet::CHClass<CBasePoint> > m_baselist;
    jobnet::CHCArray<CBasePoint> m_baselist;
    QString     m_baselist_file_name;

    void ApplyBaseList();
    void UpdateParams();
    void UpdateInterface();
    void UpdateNodes();
    void UpdateANodes();
    void UpdateStations();
    void UpdateObservations();
    void UpdateScene();
    void UpdateViewportsTree();

    void RefreshViewportsParam();
    void MainWindow::UpdateNodeInfo(jobnet::CNode *nd);
    CBasePoint *selectBasePoint();

    QAction *m_actPointsSelectBasePoint;
    QAction *m_actPointsRemoveBasePoint;
    QAction *m_actViewPrintSetup;
    QAction *m_actViewPrint;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void getUIParameters();
    QTextDocument  *m_print_doc;

private slots:
    void Exit();
    void NewNet();
    void NetOpen();
    void NetSave();
    void NetSaveAs();
    void NetClose();
    void Import();
    void AdjustNetwork();
    void aboutDlg();
    void printAdjustLog();
    void printAdjustLogSlot(QPrinter *);
    void printAdjustedPoints();
    void printSitPoints();
    void print_print_doc(QPrinter *);

    void pointsContextMenu(QPoint);

    void BaseListEdit();

    void exportTXT();
    void exportCSV();
    void exportDXF();

    void importCOMdevice();

    void zoomIn();
    void zoomOut();
    void zoom100();

    void stationSelchanged(QTableWidgetItem * current, QTableWidgetItem * previous);
    void observationsClicked(QTreeWidgetItem *item, int column);
    void observationsChanged(QTreeWidgetItem *item, int column);
    void observationsSelectionChanged();
    void treeViewportsSelectionChanged();
    void pointChanged(int row,int cell);
    void stationChanged(int row,int cell);
    void viewportPropertyChanged(int row,int col);

    void currentTabChanged_main(int);

    void currentMapScaleIndexChanged(const QString &);

    void viewPrintSetup();
    void viewPrint();
};

int LoadBaseList(jobnet::CHCArray<CBasePoint> &,QString &);

extern MainWindow *pmw;


#endif // MAINWINDOW_H
