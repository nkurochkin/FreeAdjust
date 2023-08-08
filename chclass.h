#ifndef CHCLASS_H
#define CHCLASS_H

#include <QObject>

namespace jobnet
{

class Class
{
protected:
    QBasicAtomicInt m_lockcounter;
public:
 virtual   ~Class();
 virtual   int  Lock();
 virtual   int  Unlock();
          int   GetLockCounter(){ return m_lockcounter; }

        Class();
};

template < class T> class CHClass
{
public:
    T *m_val;
    T &operator=(T *val)
    {
        if(m_val)
          m_val->Unlock();
        m_val=val;
        return *m_val;
    }
    T &operator=(T &val)
    {
        if(&val)
          val.Lock();
        if(m_val)
          m_val->Unlock();
        m_val=&val;
        return *m_val;
    }
    T &operator=(CHClass<T> &val)
    {
        if(val.m_val)
          val.m_val->Lock();
        if(m_val)
          m_val->Unlock();
        m_val=val.m_val;
        return *m_val;
    }
    operator T* () const { return m_val; }
    T *operator->(){ return m_val; }
    T *Get(){ return m_val; }
    T *data(){ return m_val; }
    T** R(){ return &m_val; }
    void **Reference(){ return (void **)&m_val; }
    bool isNull(){ return m_val==0; }
    bool Exist(){  return m_val!=0; }
    bool operator()(){ return m_val!=NULL; }
    T *Lock(){ if(m_val) m_val->Lock(); return m_val; }
    CHClass(){ m_val=NULL; }
    CHClass(T *val){ m_val=val; }
    CHClass(T &val)
        {
                m_val=&val;
        if(m_val)
            m_val->Lock();
    }
    CHClass(CHClass<T> &val)
        {
        m_val=val.m_val;
        if(m_val)
            m_val->Lock();
        }
    ~CHClass()
    {
        if(m_val)
            m_val->Unlock();
    }
};

}

#endif // CHCLASS_H
