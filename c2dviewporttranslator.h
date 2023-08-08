#ifndef C2DVIEWPORTTRANSLATOR_H
#define C2DVIEWPORTTRANSLATOR_H

class jobnet::C2DViewportTranslator
{
public:
    QPointF m_pos;
    double  m_pcs;
    double  m_rpcs;
    double  m_h;
    double  m_scale;

    double pl_dl(double v)
    {
        return v*m_scale*m_rpcs;
    }
    double wl_dl(double v)
    {
        return v*m_rpcs;
    }
    void ne_xy(double n,double e,double &x,double &y)
    {
        x=(e-m_pos.x())*m_rpcs;
        y=m_h-(n-m_pos.y())*m_rpcs;
    }
    QPointF ne_xy(double n,double e)
    {
        QPoint rv((e-m_pos.x())*m_rpcs,m_h-(n-m_pos.y())*m_rpcs);
        return rv;
    }
    QPointF ne_xy(QPointF &v)
    {
        QPoint rv((v.y()-m_pos.x())*m_rpcs,m_h-(v.x()-m_pos.y())*m_rpcs);
        return rv;
    }

    C2DViewportTranslator(double lx,double ly,double pcs,double h,double scale)
    {
        m_pos.setX(lx);
        m_pos.setY(ly);
        m_pcs   =pcs;
        m_rpcs  =1./pcs;
        m_h     =h;
        m_scale =scale;
    }
};

#endif // C2DVIEWPORTTRANSLATOR_H
