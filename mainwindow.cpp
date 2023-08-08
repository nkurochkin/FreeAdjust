#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <QSettings>

#include "CNodeTI.h"
#include "CStationTI.h"
#include "CObservTI.h"
#include "CViewportTI.h"
#include "CViewportPropertyTI.h"

//========================================
//   MainWindow
//

#define PROG_TITLE  "FreeAdjust"

void MainWindow::NewNet()
{
    ui->Import->setEnabled(1);
    ui->AdjustNetwork->setEnabled(1);
    m_job.Clear();
    m_job_file_name.clear();
    setWindowTitle(PROG_TITLE);
    UpdateInterface();
}

void MainWindow::NetOpen()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
         tr("Open Network"), "", tr("Gamma local files (*.gkf)"));
    if( !fileName.isNull() )
    {
        if(!m_job.load(fileName))
        {
            ui->AdjustNetwork->setEnabled(1);
            m_job_file_name=fileName;
            setWindowTitle(m_job_file_name);
        }else
            ui->AdjustNetwork->setEnabled(0);
        UpdateInterface();
    }
}

void MainWindow::NetClose()
{
    ui->Import->setEnabled(0);
    ui->AdjustNetwork->setEnabled(0);
    m_job.Clear();
    m_job_file_name.clear();

    setWindowTitle(PROG_TITLE);

    UpdateInterface();
}

void MainWindow::NetSaveAs()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
         tr("Save Network"), "", tr("Gamma local files (*.gkf)"));
    if( !fileName.isNull() )
    {
        getUIParameters();
        if(!m_job.save(fileName))
        {
            m_job_file_name=fileName;
            setWindowTitle(m_job_file_name);
        }
    }
}

void MainWindow::NetSave()
{
    if(!m_job_file_name.size())
        NetSaveAs();
    else
    {
        getUIParameters();
        m_job.save(m_job_file_name);
    }
}

void MainWindow::Import()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
         tr("Импорт данных"), "", tr("Device Files (*.txt *.sdr *.dat)"));
    if( !fileName.isNull() )
    {
        m_job.ImportDeviceFile(fileName,0);
        ApplyBaseList();
        UpdateInterface();
    }
}

void MainWindow::aboutDlg()
{
     QMessageBox msgBox;
     msgBox.setText(
         "FreeAdjust является пользовательским интерфейсом\n"
         "к программе gama-local версии 1.12,\n"
         "разработаной Ales Cepek (cepek@gnu.org)"
     );
     msgBox.setDetailedText(
         "Исходные тексты gama-local\n"
         "и документация доступны по адресу:\n"
         "http://www.gnu.org/software/gama/\n"
         "Исходный текст программы FreeAdjust:\n"
         "http://freereason.dyndns.org/files/fa_src.zip"
     );
     msgBox.exec();
}

void MainWindow::Exit()
{
    ::exit(0);
}

void MainWindow::AdjustNetwork()
{
    getUIParameters();
    m_job.Adjust(ui->textBrowser);
    UpdateInterface();
}

void MainWindow::zoomIn()
{
    m_ui2DView->zoomIn();
}

void MainWindow::zoomOut()
{
    m_ui2DView->zoomOut();
}

void MainWindow::zoom100()
{
    m_ui2DView->zoom(1.);
}

void MainWindow::print_print_doc(QPrinter *pr)
{
    if(m_print_doc)
        m_print_doc->print(pr);
}

void MainWindow::printAdjustedPoints()
{
     QString html = "";
     m_print_doc= new QTextDocument();

     html += "Каталог исходных координат и высот";
     html += "<html><body><table width=512 border=1 bordercolor=black cellpadding=5 cellspacing=0 >";
     html += "<tr><td>Имя</td><td align=\"center\">X</td><td align=\"center\">Y</td><td align=\"center\">h</td>";
     html += "</tr>";
     for (int j = 0; j < m_job.m_nodes.size(); j++)
     {
         if (m_job.m_nodes[j]->m_fX || m_job.m_nodes[j]->m_fY || m_job.m_nodes[j]->m_fZ)
         {
             html += "<tr><td>";
             html += m_job.m_nodes[j]->m_id;
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_fX)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->fx);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_fY)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->fy);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_fZ)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->fz);
                 html+=text;
             }
             html += "</td></tr>";
         }
     }
     html += "</table>";

     html += "Ведомость уравненных координат и высот съемочного обоснования";
     html += "<html><body><table width=512 border=1 bordercolor=black cellpadding=5 cellspacing=0 >";
     html += "<tr><td>Имя</td><td align=\"center\">X</td><td align=\"center\">Y</td><td align=\"center\">h</td>";
     html += "</tr>";

     for (int j = 0; j < m_job.m_nodes.size(); j++)
     {
         if (m_job.m_nodes[j]->toAdjust(1) && (m_job.m_nodes[j]->m_faX || m_job.m_nodes[j]->m_faY || m_job.m_nodes[j]->m_faZ))
         {
             html += "<tr><td>";
             html += m_job.m_nodes[j]->m_id;
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faX)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->ax);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faY)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->ay);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faZ)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->az);
                 html+=text;
             }
             html += "</td></tr>";
         }
     }
     html += "</table></body></html>";
     m_print_doc->setHtml(html);

     {
         QPrintPreviewDialog prevDlg(&m_printer);
         connect(&prevDlg, SIGNAL(paintRequested(QPrinter*)),
                 this, SLOT(print_print_doc(QPrinter*)));
         prevDlg.exec();
     }
     delete m_print_doc;
     m_print_doc=0;
}

void MainWindow::printSitPoints()
{
     QString html = "";
     m_print_doc= new QTextDocument();

     html += "Ведомость координат и высот тахеометрии";
     html += "<html><body><table width=512 border=1 bordercolor=black cellpadding=5 cellspacing=0 >";
     html += "<tr><td>Имя</td><td align=\"center\">X</td><td align=\"center\">Y</td><td align=\"center\">h</td>";
     html += "</tr>";

     for (int j = 0; j < m_job.m_nodes.size(); j++)
     {
         if (!m_job.m_nodes[j]->toAdjust(1) && (m_job.m_nodes[j]->m_faX || m_job.m_nodes[j]->m_faY || m_job.m_nodes[j]->m_faZ))
         {
             html += "<tr><td>";
             html += m_job.m_nodes[j]->m_id;
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faX)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->ax);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faY)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->ay);
                 html+=text;
             }
             html += "</td><td align=\"right\">";
             if(m_job.m_nodes[j]->m_faZ)
             {
                 QString text;
                 text.sprintf("%.3f",m_job.m_nodes[j]->az);
                 html+=text;
             }
             html += "</td></tr>";
         }
     }
     html += "</table></body></html>";
     m_print_doc->setHtml(html);

     {
         QPrintPreviewDialog prevDlg(&m_printer);
         connect(&prevDlg, SIGNAL(paintRequested(QPrinter*)),
                 this, SLOT(print_print_doc(QPrinter*)));
         prevDlg.exec();
     }
     delete m_print_doc;
     m_print_doc=0;
}

void MainWindow::printAdjustLogSlot(QPrinter *pr)
{
    ui->textBrowser->print(pr);
}

void MainWindow::printAdjustLog()
{
     QPrintPreviewDialog prevDlg(&m_printer);
     connect(&prevDlg, SIGNAL(paintRequested(QPrinter*)),
             this, SLOT(printAdjustLogSlot(QPrinter*)));
     prevDlg.exec();
}

void MainWindow::UpdateNodes()
{
    if(m_lock_updates)
        return;
    m_lock_updates++;
    ui->PointsWidget->setRowCount(m_job.m_nodes.size());
    unsigned i;
    for(i=0;i<m_job.m_nodes.size();i++)
    {
        {
            CNodeTypeTI *wi=new CNodeTypeTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,0,wi);
            ui->PointsWidget->item(i,0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        {
            CNodeNameTI *wi=new CNodeNameTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,1,wi);
        }
        {
            CNodeXTI *wi=new CNodeXTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,2,wi);
        }
        {
            CNodeYTI *wi=new CNodeYTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,3,wi);
        }
        {
            CNodeZTI *wi=new CNodeZTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,4,wi);
        }
        {
            CNodeADJTI *wi=new CNodeADJTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,5,wi);
        }
        {
            CNodeCommentTI *wi=new CNodeCommentTI(m_job.m_nodes[i]);
            ui->PointsWidget->setItem(i,6,wi);
        }
    }
    m_lock_updates--;
}

void MainWindow::UpdateANodes()
{
    if(m_lock_updates)
        return;
    m_lock_updates++;
    ui->APointsWidget->setRowCount(m_job.m_nodes.size());
    unsigned i;
    for(i=0;i<m_job.m_nodes.size();i++)
    {
        {
            CNodeTypeTI *wi=new CNodeTypeTI(m_job.m_nodes[i]);
            ui->APointsWidget->setItem(i,0,wi);
            ui->APointsWidget->item(i,0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        {
            CANodeNameTI *wi=new CANodeNameTI(m_job.m_nodes[i]);
            ui->APointsWidget->setItem(i,1,wi);
            ui->APointsWidget->item(i,1)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        {
            CANodeXTI *wi=new CANodeXTI(m_job.m_nodes[i]);
            ui->APointsWidget->setItem(i,2,wi);
            ui->APointsWidget->item(i,2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        {
            CANodeYTI *wi=new CANodeYTI(m_job.m_nodes[i]);
            ui->APointsWidget->setItem(i,3,wi);
            ui->APointsWidget->item(i,3)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        {
            CANodeZTI *wi=new CANodeZTI(m_job.m_nodes[i]);
            ui->APointsWidget->setItem(i,4,wi);
            ui->APointsWidget->item(i,4)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
    }
    m_lock_updates--;
}

void MainWindow::UpdateNodeInfo(jobnet::CNode *nd)
{
    if(m_lock_updates)
        return;
    m_lock_updates++;
    unsigned i;
    for(i=0;i<ui->PointsWidget->rowCount();i++)
    {
        CNodeTI *nti=(CNodeTI *)ui->PointsWidget->item(i,0);
        if(nti && nti->m_node.data()==nd)
            nti->updateText();
        nti=(CNodeTI *)ui->PointsWidget->item(i,6);
        if(nti && nti->m_node.data()==nd)
            nti->updateText();
    }
    for(i=0;i<ui->APointsWidget->rowCount();i++)
    {
        CNodeTI *nti=(CNodeTI *)ui->APointsWidget->item(i,0);
        if(nti && nti->m_node.data()==nd)
            nti->updateText();
    }
    m_lock_updates--;
}

void MainWindow::UpdateScene()
{
    double xmin,ymin,xmax,ymax;
    double sxmin,symin,sxmax,symax;
    if(m_lock_updates)
        return;
    if(m_job.getextents(xmin,ymin,xmax,ymax))
    {
        m_ui2DView->getpExtents(sxmin,symin,sxmax,symax);
        bool rv=true;
        if(sxmin>xmax || sxmax<xmin || symin>ymax || symax<ymin)
            rv=false;
        if(!rv)
        {
            rv=true;
            unsigned i;
            for(i=0;i<m_job.m_stations.size();i++)
            {
                if(m_job.m_stations[i]->m_node->m_uFlags& (NFL_AX|NFL_AY))
                {
                    double x,y;
                    x=m_job.m_stations[i]->m_node->ax;
                    y=m_job.m_stations[i]->m_node->ay;
                    m_ui2DView->setView(x,y);
                    m_ui2DPrintView->setView(x,y);
                    rv=false;
                    break;
                }
            }
        }
        if(rv)
        {
            m_ui2DView->update();
            m_ui2DPrintView->update();
        }
        //double x=(xmin+xmax)*0.5;
        //double y=(ymin+ymax)*0.5;
        //m_ui2DView->setView(x,y);
        //m_ui2DPrintView->setView(x,y);

        if(m_job.m_views.GetQ())
        {  // Изменить размер основного вида
            m_job.m_views[0]->setExtents(xmin,ymin,xmax,ymax);
        }
    }
}

void MainWindow::UpdateViewportsTree()
{
    ui->treeViewports->clear();
    unsigned i;
    for(i=0;i<m_job.m_views.GetQ();i++)
    {
        jobnet::CHClass<jobnet::C2DPrintViewport> vv=m_job.m_views[i];
        CViewportTI *twi=new CViewportMainTI(vv);
        ui->treeViewports->addTopLevelItem(twi);
    }
    treeViewportsSelectionChanged();
}

void MainWindow::RefreshViewportsParam()
{
    unsigned rc=ui->tableViewportsProperty->rowCount();
    unsigned i;
    for(i=0;i<rc;i++)
    {
        CViewportPropertyTI *vp=(CViewportPropertyTI *)ui->tableViewportsProperty->item(i,1);
        if(vp)
            vp->updateText();
    }
}

void MainWindow::UpdateStations()
{
    if(m_lock_updates)
        return;
    m_lock_updates++;
    ui->StationsWidget->setRowCount(m_job.m_stations.size());
    unsigned i;
    for(i=0;i<m_job.m_stations.size();i++)
    {
        ui->StationsWidget->setItem(i,0,new CStationNameTI(m_job.m_stations[i]));
        ui->StationsWidget->setItem(i,1,new CStationHeightTI(m_job.m_stations[i]));
    }
    m_lock_updates--;
}

void MainWindow::UpdateObservations()
{
    ui->ObservationsWidget->clear();
    if(!m_curr_st.isNull())
    {
        unsigned i;
        for(i=0;i<m_curr_st->m_observations.size();i++)
        {
            jobnet::CHClass<jobnet::CStationNode> stnode=m_curr_st->m_observations[i];
            CObservNameTI *osn=new CObservNameTI(stnode);
            ui->ObservationsWidget->addTopLevelItem(osn);
            unsigned j;
            for(j=0;j<stnode->m_list.size();j++)
            {
                osn->addChild(new CObservVal(stnode,stnode->m_list[j]));
            }
            if(stnode->m_node->m_stations || (stnode->m_node->m_fX && stnode->m_node->m_fY))
                osn->setExpanded(1);
        }
    }
}

void MainWindow::getUIParameters()
{
    m_job.m_title=ui->projectShortDescription->toPlainText();
    {
        QByteArray text=ui->adjust_algo->itemText(ui->adjust_algo->currentIndex()).toAscii();
        m_job.m_adjust_algo=text.data();
    }
    {
        bool ok=false;
        double val=ui->sigma_apriory->text().toDouble(&ok);
        if(ok)
            m_job.m_sigma_apr=val;
    }
    {
        bool ok=false;
        double val=ui->confidence_probability->text().toDouble(&ok);
        if(ok)
        {
            if(val<0)
                val=0;
            if(val>1.)
                val=1.;
            m_job.m_conf_pr=val;
        }
    }
    {
        bool ok=false;
        double val=ui->tol_abs->text().toDouble(&ok);
        if(ok)
        {
            if(val<0)
                val=0.;
            m_job.m_tol_abs=val;
        }
    }
    {
        bool ok=false;
        double val=ui->stddev_direction->text().toDouble(&ok);
        if(ok)
        {
            if(val<0.01)
                val=0.01;
            m_job.m_direction_stdev=val;
        }
    }
    {
        bool ok=false;
        double val=ui->stddev_angle->text().toDouble(&ok);
        if(ok)
        {
            if(val<0.01)
                val=0.01;
            m_job.m_angle_stdev=val;
        }
    }
    {
        bool ok=false;
        double val=ui->stddev_distance->text().toDouble(&ok);
        if(ok)
        {
            if(val<0.01)
                val=0.01;
            m_job.m_distance_stdev=val;
        }
    }
    {
        bool ok=false;
        double val=ui->stddev_zenith_angle->text().toDouble(&ok);
        if(ok)
        {
            if(val<0.01)
                val=0.01;
            m_job.m_zenith_angle_stdev=val;
        }
    }

    {
        unsigned val=ui->sigma_act->currentIndex();
        if(val<2)
            m_job.m_sigma_act=(jobnet::trsd_enum) val;

    }
    {
        unsigned val=ui->update_cc->currentIndex();
        if(val)
            val=1;
        m_job.m_ucc=val;
    }
    m_job.m_adjust_sit=ui->adjustEndNodes->checkState();
    UpdateParams();
}

void MainWindow::UpdateParams()
{
    ui->projectShortDescription->setText(m_job.m_title);
    {
        QString text;
        text.sprintf("%.1f",m_job.m_sigma_apr);
        ui->sigma_apriory->setText(text);
    }
    {
        QString text;
        text.sprintf("%.2f",m_job.m_conf_pr);
        ui->confidence_probability->setText(text);
    }
    {
        QString text;
        text.sprintf("%.0f",m_job.m_tol_abs);
        ui->tol_abs->setText(text);
    }
    ui->sigma_act->setCurrentIndex(m_job.m_sigma_act);
    ui->update_cc->setCurrentIndex(m_job.m_ucc);

    {
        QString text;
        text.sprintf("%.1f",m_job.m_direction_stdev);
        ui->stddev_direction->setText(text);
    }
    {
        QString text;
        text.sprintf("%.1f",m_job.m_angle_stdev);
        ui->stddev_angle->setText(text);
    }
    {
        QString text;
        text.sprintf("%.1f",m_job.m_zenith_angle_stdev);
        ui->stddev_zenith_angle->setText(text);
    }
    {
        QString text;
        text.sprintf("%.1f",m_job.m_distance_stdev);
        ui->stddev_distance->setText(text);
    }
}


void MainWindow::UpdateInterface()
{
    UpdateParams();
    UpdateNodes();
    UpdateANodes();
    UpdateStations();
    UpdateScene();
    UpdateViewportsTree();
}

void MainWindow::ApplyBaseList()
{
    if(!m_baselist.size())
        return;
    int i;
    for(i=0;i<m_job.m_nodes.size();i++)
    {
        QString nn=m_job.m_nodes[i]->m_id;
        int j;
        for(j=0;j<m_baselist.size();j++)
        {
            if(m_baselist[j]->m_name==nn)
            {
                if(m_baselist[j]->m_fXY)
                    m_job.m_nodes[i]->fixXY(m_baselist[j]->m_xy);
                if(m_baselist[j]->m_fZ)
                    m_job.m_nodes[i]->fixZ(m_baselist[j]->m_z);
                break;
            }
        }
    }
}


void MainWindow::stationSelchanged(QTableWidgetItem * current, QTableWidgetItem * previous)
{
    CStationTI *sti=(CStationTI *)current;
    if(!sti)
    {
        ui->ObservationsWidget->clear();
        m_curr_st=0;
    }else
    if(m_curr_st!=sti->m_st)
    {
        m_curr_st=sti->m_st;
        UpdateObservations();
    }
    m_ui2DView->update();
}

void MainWindow::observationsClicked(QTreeWidgetItem *item, int column)
{
    //if(item)
    //    ((CObservTI *)item)->changed(column);
}

void MainWindow::observationsSelectionChanged()
{
    m_ui2DView->update();
}

void MainWindow::observationsChanged(QTreeWidgetItem *item, int column)
{
    if(item)
        ((CObservTI *)item)->changed(column);
}

void MainWindow::pointChanged(int row,int col)
{
    CNodeTI *nti=(CNodeTI *)ui->PointsWidget->item(row,col);
    if(nti)
        nti->changed();
}

void MainWindow::stationChanged(int row,int col)
{
    CStationTI *sti=(CStationTI *)ui->StationsWidget->item(row,col);
    if(sti)
        sti->changed();
}

void MainWindow::viewportPropertyChanged(int row,int col)
{
    CViewportPropertyTI *vti=(CViewportPropertyTI *)ui->tableViewportsProperty->item(row,col);
    if(vti)
        vti->changed();
}

void MainWindow::treeViewportsSelectionChanged()
{
    QList<QTreeWidgetItem *> list=ui->treeViewports->selectedItems();
    m_job.m_curr_view=0;
    m_job.m_curr_page=0;
    if(list.size())
    {
        m_job.m_curr_view=((CViewportTI *)list[0])->viewport();
        m_job.m_curr_page=((CViewportTI *)list[0])->page();
    }
    if(m_job.m_curr_view.Exist())
    {  // Заполнение таблицы свойствами вида
        //ui->tableViewportsProperty
        ui->tableViewportsProperty->setRowCount(CViewportPropertyTI::TE_COUNT);
        unsigned cc=0;

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("Имя")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_NAME));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("Масштаб карты")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_SCALE));

        ui->tableViewportsProperty->setItem(cc,0,new QTableWidgetItem(tr("Масштаб эллипса ошибок")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_ELLIPSE_SCALE));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("сетка страниц Y")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_PAGE_GRID_X));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("сетка страниц X")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_PAGE_GRID_Y));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("верх рамки страницы")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_MARGIN_TOP));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("низ рамки страницы")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_MARGIN_BOTTOM));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("лево рамки страницы")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_MARGIN_LEFT));

        ui->tableViewportsProperty->setItem(cc  ,0,new QTableWidgetItem(tr("право рамки страницы")));
        ui->tableViewportsProperty->setItem(cc++,1,new CViewportPropertyTI(this,m_job.m_curr_view,CViewportPropertyTI::TE_MARGIN_RIGHT));
    }else
    {
        ui->tableViewportsProperty->setRowCount(0);
    }
    m_ui2DPrintView->update();
    m_ui2DPageView->update();
}

void MainWindow::currentMapScaleIndexChanged(const QString &scale)
{
    bool ok=false;
    double val=scale.toDouble(&ok);
//    if(ok)
//        m_ui2DView->setScale(val);
//    else
//        m_ui2DView->setScale(0.);
}

void MainWindow::pointsContextMenu(QPoint mp)
{
    QPoint gp;
    gp=ui->PointsWidget->viewport()->mapToGlobal(mp);
    QList<QTableWidgetItem *> list=ui->PointsWidget->selectedItems();
    if(list.size())
    {
        QMenu menu;
        menu.addAction(m_actPointsSelectBasePoint);
        menu.addAction(m_actPointsRemoveBasePoint);
        QAction *act=menu.exec(gp);
        if(!act)
            return;
        if(act->text()=="Выбрать пункт")
        {
            jobnet::CHClass<CBasePoint> bp=selectBasePoint();
            if(bp.Exist())
            {
                jobnet::CHClass<jobnet::CNode> node=((CNodeTI *)list[0])->m_node;
                if(bp->m_fXY)
                {
                    node->fixXY(bp->m_xy.x(),bp->m_xy.y());
                }else
                    node->unfixXY();
                if(bp->m_fZ)
                {
                    node->fixZ(bp->m_z);
                }else
                    node->unfixZ();
                node->m_comment=bp->m_name;
                int row=list[0]->row();
                ((CNodeTI *)ui->PointsWidget->item(row,0))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,1))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,2))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,3))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,4))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,5))->updateText();
                ((CNodeTI *)ui->PointsWidget->item(row,6))->updateText();
            }
        }else
        if(act->text()=="Удалить пункт")
        {
            jobnet::CHClass<jobnet::CNode> node=((CNodeTI *)list[0])->m_node;
            node->unfixXY();
            node->unfixZ();
            int row=list[0]->row();
            ((CNodeTI *)ui->PointsWidget->item(row,0))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,1))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,2))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,3))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,4))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,5))->updateText();
            ((CNodeTI *)ui->PointsWidget->item(row,6))->updateText();
        }
    }
}

void MainWindow::currentTabChanged_main(int idx)
{
    switch(idx)
    {
    case 0:
        {
            ui->mainToolBar->clear();
            ui->mainToolBar->addAction(ui->NetOpen);
            ui->mainToolBar->addAction(ui->NetSave);
            ui->mainToolBar->addSeparator();
            ui->mainToolBar->addAction(ui->AdjustNetwork);
            ui->mainToolBar->addSeparator();
            ui->mainToolBar->addAction(ui->Import);
            ui->mainToolBar->addAction(ui->action_inportCOM);

        }
        break;
    default:
        ui->mainToolBar->clear();
        ui->mainToolBar->addAction(m_actViewPrintSetup);
        ui->mainToolBar->addAction(m_actViewPrint);
    }
}


void MainWindow::viewPrintSetup()
{
    if(m_job.m_curr_view.Exist())
    {
        if(m_job.m_curr_view->pageSetupDlg(this))
        {
           RefreshViewportsParam();
           m_ui2DPrintView->update();
           m_ui2DPageView->update();
        }
    }
}

void MainWindow::viewPrint()
{
    if(m_job.m_curr_view.Exist())
    {
        m_job.m_curr_view->pagePrint(m_job,m_ui2DPageView->m_page_num);
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP1251"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));

    QCoreApplication::setOrganizationName("FRG");
    QCoreApplication::setOrganizationDomain("freereason.com");
    QCoreApplication::setApplicationName("freeadjust");

    m_ui2DPrintView=new C2DPrintView(this,&m_job);
    ui->horizontalLayout_VPE->addWidget(m_ui2DPrintView);

    m_ui2DPageView=new C2DPageView(this,&m_job);
    ui->horizontalLayout_VE->addWidget(m_ui2DPageView);

    m_ui2DView     =new C2DView(&m_job,ui);
    ui->splitter->addWidget(m_ui2DView);
    {
        int w=width();
        QSettings set;
        int l1=set.value("splitter/part").toInt();
        if(l1==0)
            l1=500;

        QList<int> list;
        list << (w*l1)/1000 << (w*(1000-l1))/1000;
        ui->splitter->setSizes (list);
    }

    {
        int w=width();
        QSettings set;
        int l1=set.value("splitter_vp/part").toInt();
        if(l1==0)
            l1=500;
        QList<int> list;
        list << (w*l1)/1000 << (w*(1000-l1))/1000;
        ui->splitter_Viewports->setSizes (list);
    }

    ui->StationsWidget->setColumnCount(3);
    ui->StationsWidget->verticalHeader()->setDefaultSectionSize(
                ui->StationsWidget->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Name")<<tr("Height")<<tr("Date");
        ui->StationsWidget->setHorizontalHeaderLabels(list);
    }

    ui->ObservationsWidget->setColumnCount(6);
//    ui->MeasurementsWidget->verticalHeader()->setDefaultSectionSize(
//    ui->MeasurementsWidget->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Name")<<tr("Value")<<tr("stdev")<<tr("height")<<tr("AValue")<<tr("dev")<<tr("devint");
        ui->ObservationsWidget->setHeaderLabels(list);
    }

    ui->PointsWidget->setColumnCount(7);
    ui->PointsWidget->verticalHeader()->setDefaultSectionSize(
                ui->PointsWidget->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Type")<<tr("Name")<<tr("X")<<tr("Y")<<tr("H")<<tr("adj")<<tr("comment");
        ui->PointsWidget->setHorizontalHeaderLabels(list);
    }
    ui->PointsWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->APointsWidget->setColumnCount(5);
    ui->APointsWidget->verticalHeader()->setDefaultSectionSize(
                ui->APointsWidget->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Type")<<tr("Name")<<tr("X")<<tr("Y")<<tr("H");
        ui->APointsWidget->setHorizontalHeaderLabels(list);
    }

    ui->treeViewports->setColumnCount(1);
    {
        QStringList list;
        list<<tr("Вид");
        ui->treeViewports->setHeaderLabels(list);
    }
    ui->treeViewports->setSelectionMode(QAbstractItemView::SingleSelection);


    ui->tableViewportsProperty->setColumnCount(2);
    ui->tableViewportsProperty->verticalHeader()->setDefaultSectionSize(
                ui->tableViewportsProperty->verticalHeader()->minimumSectionSize());
    {
        QStringList list;
        list<<tr("Качество")<<tr("Значение");
        ui->tableViewportsProperty->setHorizontalHeaderLabels(list);
        ui->tableViewportsProperty->setColumnWidth(0,150);
        ui->tableViewportsProperty->setColumnWidth(1,200);
    }
    ui->tableViewportsProperty->setSelectionMode(QAbstractItemView::SingleSelection);

    m_actPointsSelectBasePoint=new QAction(QString::fromUtf8("selectPP"),this);
    m_actPointsSelectBasePoint->setText(tr("Выбрать пункт"));
    m_actPointsRemoveBasePoint=new QAction(QString::fromUtf8("removePP"),this);
    m_actPointsRemoveBasePoint->setText(tr("Удалить пункт"));
    ui->PointsWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->PointsWidget, SIGNAL(customContextMenuRequested(QPoint)),SLOT(pointsContextMenu(QPoint)));

    m_actViewPrintSetup =new QAction(QString::fromUtf8("viewPrintSetup"),this);
    m_actViewPrintSetup->setText(tr("Настроить принтер"));
    m_actViewPrintSetup->setIcon(QIcon(":/icons/printsetup"));

    m_actViewPrint =new QAction(QString::fromUtf8("viewPrintSetup"),this);
    m_actViewPrint->setText(tr("Напечатать"));
    m_actViewPrint->setIcon(QIcon(":/icons/fileprint"));

    //fileprint

    connect(ui->Exit,      SIGNAL(triggered()), this, SLOT(Exit()));
    connect(ui->Import,    SIGNAL(triggered()), this, SLOT(Import()));
    connect(ui->NewNet,    SIGNAL(triggered()), this, SLOT(NewNet()));
    connect(ui->NetOpen,   SIGNAL(triggered()), this, SLOT(NetOpen()));
    connect(ui->NetClose,  SIGNAL(triggered()), this, SLOT(NetClose()));
    connect(ui->NetSave,   SIGNAL(triggered()), this, SLOT(NetSave()));
    connect(ui->NetSaveAs, SIGNAL(triggered()), this, SLOT(NetSaveAs()));
    connect(ui->AdjustNetwork, SIGNAL(triggered()), this, SLOT(AdjustNetwork()));
    connect(ui->zoomIn,    SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->zoomOut,   SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui->zoom100,   SIGNAL(triggered()), this, SLOT(zoom100()));
    connect(ui->actionExportTxt, SIGNAL(triggered()), this, SLOT(exportTXT()));
    connect(ui->actionExportCsv, SIGNAL(triggered()), this, SLOT(exportCSV()));
    connect(ui->actionExportDxf, SIGNAL(triggered()), this, SLOT(exportDXF()));
    connect(ui->actionAbout,     SIGNAL(triggered()), this, SLOT(aboutDlg()));
    connect(ui->action_inportCOM,SIGNAL(triggered()), this, SLOT(importCOMdevice()));
    connect(ui->printAdjustLog,  SIGNAL(triggered()), this, SLOT(printAdjustLog()));
    connect(ui->printAdjustPoints,SIGNAL(triggered()),this, SLOT(printAdjustedPoints()));
    connect(ui->printSitPoints   ,SIGNAL(triggered()),this, SLOT(printSitPoints()));
    connect(ui->editBaseList,    SIGNAL(triggered()), this, SLOT(BaseListEdit()));

    connect(ui->PointsWidget,       SIGNAL(cellChanged(int,int)), this, SLOT(pointChanged(int,int)));
    connect(ui->StationsWidget,     SIGNAL(cellChanged(int,int)), this, SLOT(stationChanged(int,int)));
    connect(ui->StationsWidget,     SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(stationSelchanged(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->ObservationsWidget, SIGNAL(itemSelectionChanged()), this, SLOT(observationsSelectionChanged()));
    connect(ui->ObservationsWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(observationsChanged(QTreeWidgetItem *, int)));
    connect(ui->treeViewports,      SIGNAL(itemSelectionChanged()), this, SLOT(treeViewportsSelectionChanged()));
    connect(ui->tableViewportsProperty,SIGNAL(cellChanged(int,int)), this, SLOT(viewportPropertyChanged(int,int)));

    connect(ui->tabWidgetMain,   SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged_main(int)));
    connect(m_actViewPrintSetup, SIGNAL(triggered()), this, SLOT(viewPrintSetup()));
    connect(m_actViewPrint,      SIGNAL(triggered()), this, SLOT(viewPrint()));

    //ui->mainToolBar->addAction(ui->NewNet);
    //ui->mainToolBar->addAction(ui->NetOpen);
    //ui->mainToolBar->addAction(ui->NetClose);
    ui->Exit->setIcon(QIcon(":/icons/exit.png"));
    ui->NetOpen->setIcon(QIcon(":/icons/open.png"));
    ui->NetSave->setIcon(QIcon(":/icons/save.png"));
    ui->Import->setIcon(QIcon(":/icons/file_import.png"));
    ui->AdjustNetwork->setIcon(QIcon(":/icons/adjust.png"));
    ui->action_inportCOM->setIcon(QIcon(":/icons/dev_import.png"));

    ui->tabWidgetMain->setCurrentIndex(0);
    currentTabChanged_main(0); // в первый раз сам он не может

    ui->sigma_act->addItem(tr("aposteriori"));
    ui->sigma_act->addItem(tr("apriori"));

    ui->update_cc->addItem(tr("no"));
    ui->update_cc->addItem(tr("yes"));

    ui->adjust_algo->addItem(tr("svd"));
    ui->adjust_algo->addItem(tr("gso"));
    ui->adjust_algo->addItem(tr("cholesky"));
    ui->adjust_algo->addItem(tr("envelope"));

    ui->map_scale->addItem("No scale");
    ui->map_scale->addItem("500");
    ui->map_scale->addItem("1000");
    ui->map_scale->addItem("2000");
    ui->map_scale->addItem("5000");
    ui->map_scale->addItem("10000");
    ui->map_scale->addItem("25000");
    ui->map_scale->addItem("50000");

    connect(ui->map_scale, SIGNAL( currentIndexChanged(const QString &)), this, SLOT(currentMapScaleIndexChanged(const QString &)));

    {
        QTextCodec   *codec1 = QTextCodec::codecForName("CP-1251");
        QTextCodec::setCodecForTr(codec1);
        QTextCodec::setCodecForLocale(codec1);
        QTextCodec::setCodecForCStrings(codec1);
    }
    ui->textBrowser->setFontFamily("Lucida Console");
    ui->textBrowser->setLocale(QLocale("Windows-1251"));
    ui->textBrowser->setWordWrapMode(QTextOption::NoWrap);

    //Lucida Console
    m_lock_updates=0;

    QSettings set;
    m_baselist_file_name=set.value("baselist/path").toString();
    if(QFile::exists(m_baselist_file_name))
    {
        LoadBaseList(m_baselist,m_baselist_file_name);
    }
    setWindowIcon(QIcon(":/icons/fadjust"));
    setWindowTitle(PROG_TITLE);
}

MainWindow::~MainWindow()
{
    {
        QList<int> list=ui->splitter->sizes();
        int size=list[0]+list[1];
        if(size)
        {
            QSettings set;
            set.setValue("splitter/part",(list[0]*1000)/size);
        }
    }
    {
        QList<int> list=ui->splitter_Viewports->sizes();
        int size=list[0]+list[1];
        if(size)
        {
            QSettings set;
            set.setValue("splitter_vp/part",(list[0]*1000)/size);
        }
    }
    delete ui;
}


