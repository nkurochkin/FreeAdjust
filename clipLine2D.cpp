#include "clipLine2D.h"

//typedef struct
//{
//        unsigned char l,r;
//}CLIPB;

typedef union{
        unsigned rc;
        struct
        {
            unsigned l:8;
            unsigned r:8;
        }b;
        //CLIPB  b;
}CLIPU;

class CClipLine2D
{
    CLIPU tc;
    unsigned char cs;
    QPointF m_clip_min;
    QPointF m_clip_max;
    QPointF m_l1;
    QPointF m_l2;
    QPointF *m_cline1;
    QPointF *m_cline2;
    QPointF  m_delta;
public:
    CClipLine2D(QPointF &clip_min,QPointF &clip_max,QPointF &l1,QPointF &l2,QPointF *cline1,QPointF *cline2)
    {
        tc.rc=0;
        m_clip_min=clip_min;
        m_clip_max=clip_max;
        m_l1 =l1;
        m_l2 =l2;
        m_cline1=cline1;
        m_cline2=cline2;
        if(m_cline1)
            *m_cline1=m_l1;
        if(m_cline2)
            *m_cline2=m_l2;
    }

    int _clipSide2D1X();
    int _clipSide2D1Y();
    int _clipSide2D2X();
    int _clipSide2D2Y();
    int _clipSide2D1();
    int _clipSide2D2();
    int clip2D();
};

int CClipLine2D::_clipSide2D1X()
{
    QPointF tmp;
    double  tt;

    if(tc.b.l & X_LESS) tmp.rx()=m_clip_min.x();
    else                tmp.rx()=m_clip_max.x();

    tt=(tmp.x()-m_l1.x())/m_delta.x();
    tmp.ry()=m_l1.y()+m_delta.y()*tt;
    //tmp.z=val->l1.z+val->delta.z*tt;

    if(tmp.y()<m_clip_min.y())
    {
        if(tc.b.r & Y_LESS)
        {
            cs=(tc.b.l&X_OUT)|Y_LESS;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    if(tmp.y()>=m_clip_max.y())
    {
        if(tc.b.r & Y_MORE)
        {
            cs=(tc.b.l&X_OUT)|Y_MORE;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    {
        if(m_cline1)
            *m_cline1=tmp;
        return 0;
    }
    cs=0;
    return 1;
}

int CClipLine2D::_clipSide2D1Y()
{
    QPointF tmp;
    double  tt;

    if(tc.b.l & Y_LESS) tmp.ry()=m_clip_min.y();
    else                tmp.ry()=m_clip_max.y();

    tt=(tmp.y()-m_l1.y())/m_delta.y();
    tmp.rx()=m_l1.x()+m_delta.x()*tt;
    //tmp.z=val->l1.z+val->delta.z*tt;

    if(tmp.x()<m_clip_min.x())
    {
        if(tc.b.r & X_LESS)
        {
            cs=(tc.b.l&Y_OUT)|X_LESS;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    if(tmp.x()>=m_clip_max.x())
    {
        if(tc.b.r & X_MORE)
        {
            cs=(tc.b.l&Y_OUT)|X_MORE;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    {
        if(m_cline1)
            *m_cline1=tmp;
        return 0;
    }
    cs=0;
    return 1;
}


int CClipLine2D::_clipSide2D2X()
{
    QPointF tmp;
    double  tt;

    if(tc.b.r & X_LESS) tmp.rx()=m_clip_min.x();
    else                tmp.rx()=m_clip_max.x();

    tt=(tmp.x()-m_l1.x())/m_delta.x();
    tmp.ry()=m_l1.y()+m_delta.y()*tt;
    //tmp.z=val->l1.z+val->delta.z*tt;

    if(tmp.y()<m_clip_min.y())
    {
        if(tc.b.l & Y_LESS)
        {
            cs=(tc.b.r&X_OUT)|Y_LESS;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    if(tmp.y()>m_clip_max.y())
    {
        if(tc.b.l & Y_MORE)
        {
            cs=(tc.b.r&X_OUT)|Y_MORE;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    {
        if(m_cline2)
            *m_cline2=tmp;
        return 0;
    }
    cs=0;
    return 1;
}

int CClipLine2D::_clipSide2D2Y()
{
    QPointF tmp;
    double  tt;

    if(tc.b.r & Y_LESS) tmp.ry()=m_clip_min.y();
    else                tmp.ry()=m_clip_max.y();

    tt=(tmp.y()-m_l1.y())/m_delta.y();
    tmp.rx()=m_l1.x()+m_delta.x()*tt;
    //tmp.z=val->l1.z+val->delta.z*tt;

    if(tmp.x()<m_clip_min.x())
    {
        if(tc.b.l & X_LESS)
        {
            cs=(tc.b.r&Y_OUT)|X_LESS;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    if(tmp.x()>m_clip_max.x())
    {
        if(tc.b.l & X_MORE)
        {
            cs=(tc.b.r&Y_OUT)|X_MORE;
            return tc.rc|=PART_OUTSIDE;
        }
    }else
    {
        if(m_cline2)
            *m_cline2=tmp;
        return 0;
    }
    cs=0;
    return 1;
}


int CClipLine2D::_clipSide2D1()
{
    int rv;
    if((tc.b.l & X_OUT) && (rv=_clipSide2D1X())<=0)
        return rv;
    if((tc.b.l & Y_OUT) && (rv=_clipSide2D1Y())<=0)
        return rv;
    return 1;
}

int CClipLine2D::_clipSide2D2()
{
    int rv;
    if((tc.b.r & X_OUT) && (rv=_clipSide2D2X())<=0)
        return rv;
    if((tc.b.r & Y_OUT) && (rv=_clipSide2D2Y())<=0)
        return rv;
    return 1;
}


int CClipLine2D::clip2D()
{
    int           rv;
     //  Check first line end
    if(m_l1.x()<m_clip_min.x()) tc.b.l|=X_LESS;
     else
      if(m_l1.x()>=m_clip_max.x()) tc.b.l|=X_MORE;

    if(m_l1.y()<m_clip_min.y()) tc.b.l|=Y_LESS;
     else
        if(m_l1.y()>=m_clip_max.y()) tc.b.l|=Y_MORE;

     //  Check second line end
    if(m_l2.x()<m_clip_min.x()) tc.b.r|=X_LESS;
     else
        if(m_l2.x()>=m_clip_max.x()) tc.b.r|=X_MORE;

    if(m_l2.y()<m_clip_min.y()) tc.b.r|=Y_LESS;
     else
        if(m_l2.y()>=m_clip_max.y()) tc.b.r|=Y_MORE;

     if(tc.rc)
     {  //  One or both line ends outside the clipped area
         //  Check if complete outside
        if(tc.b.l&tc.b.r)
            //	line outside the clipped
            //     region
            return  tc.rc|=FULL_OUTSIDE;
        //**************************************
        //  The parts of line may be inside
        //    the clipped area
        //**************************************

        m_delta.rx()=m_l2.x()-m_l1.x();
        m_delta.ry()=m_l2.y()-m_l1.y();
        //val.delta.z=val.l2.z-val.l1.z;

        //   Checking first side of the line
        if(tc.b.l)
        {
            if((rv=_clipSide2D1())<0) return tc.rc;
            else
            if(rv==1) return tc.rc|=ERR1_OUTSIDE; //  error in floating point
        }
        if(tc.b.r)
        {
            if((rv=_clipSide2D2())<0) return tc.rc;
            else
            if(rv==1) return tc.rc|=ERR2_OUTSIDE; //  error in floating point
        }
     }
     return tc.rc;
}

int  clipLine2D(QPointF &clip_min,QPointF &clip_size,QPointF &l1,QPointF &l2,QPointF *cline1,QPointF *cline2)
{
    CClipLine2D val(clip_min,QPointF(clip_min.x()+clip_size.x(),clip_min.y()+clip_size.y()),l1,l2,cline1,cline2);
    return val.clip2D();
}
