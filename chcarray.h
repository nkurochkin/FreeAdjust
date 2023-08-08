#ifndef CHCARRAY_H
#define CHCARRAY_H

#include "cndarray.h"
#include "chclass.h"

namespace jobnet
{

template < class T> class CHCArray : public CNDArray
{
public:
    unsigned Append(CHClass<T> &val)
    {
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }
    unsigned AppendNew(CHClass<T> &val)
    {
        unsigned i=0;
        while(i<GetQ())
        {
            if(*((CHClass<T> *)(_get(i)))==val)
                return i;
            i++;
        }
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }
    unsigned Append(T *val)
    {
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }
    unsigned AppendNew(T *val)
    {
        unsigned i=0;
        while(i<GetQ())
        {
            if(*((CHClass<T> *)(_get(i)))==val)
                return i;
            i++;
        }
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }
    unsigned Append(T &val)
    {
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }
    unsigned AppendNew(T &val)
    {
        unsigned i=0;
        while(i<GetQ())
        {
            if(*((CHClass<T> *)(_get(i)))==&val)
                return i;
            i++;
        }
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }

    unsigned Add(T &val)
    {
        unsigned i=0;
        while(i<GetQ())
        {
            if(!(*((CHClass<T> *)(_get(i)))).m_val)
            {
                *((CHClass<T> *)(_get(i)))=val;
                return i;
            }
            i++;
        }
        *((CHClass<T> *)(_get(AddEmptySpace(1))))=val;
        return GetQ()-1;
    }

    unsigned Insert(unsigned idx,CHClass<T> &val)
    {
        unsigned null=0;
        unsigned ridx=Insert(idx,&null);
        *((CHClass<T> *)(_get(ridx)))=val;
        return ridx;
    }

    unsigned Insert(unsigned idx,T *val)
    {
        unsigned null=0;
        unsigned ridx=Insert(idx,&null);
        *((CHClass<T> *)(_get(ridx)))=val;
        return ridx;
    }

    unsigned Insert(unsigned idx,T &val)
    {
        unsigned null=0;
        unsigned ridx=Insert(idx,&null);
        *((CHClass<T> *)(_get(ridx)))=val;
        return ridx;
    }

    void Set(unsigned idx,CHClass<T> &val)
    {
        CheckSizeClear(idx+1);
        *((CHClass<T> *)(_get(idx)))=val;
    }
    void Set(unsigned idx,T *val)
    {
        CheckSizeClear(idx+1);
        *((CHClass<T> *)(_get(idx)))=val;
    }
    void Set(unsigned idx,T &val)
    {
        CheckSizeClear(idx+1);
        *((CHClass<T> *)(_get(idx)))=val;
    }
    int Delete(T *val)
    {
        unsigned i=m_q;
        while(i--)
        {
            if(((CHClass<T> *)(_get(i)))->m_val==val)
            {
                ((CHClass<T> *)(_get(i)))->~CHClass();
                this->CNDArray::Delete(i);
                return i;
            }
        }
        return -1;
    }
    int Delete(T &val)
    {
        unsigned i=m_q;
        while(i--)
        {
            if(*((CHClass<T> *)(_get(i)))==&val)
            {
                ((CHClass<T> *)(_get(i)))->~CHClass();
                this->CNDArray::Delete(i);
                return i;
            }
        }
        return -1;
    }
    void Delete(int idx)
    {
        if(idx<GetQ())
        {
            ((CHClass<T> *)(_get(idx)))->~CHClass();
            this->CNDArray::Delete(idx);
        }
    }
    void Delete(int idx,int q)
    {
        while(q)
        {
            if(idx<m_num)
            {
                ((CHClass<T> *)(_get(idx)))->~CHClass();
                this->CNDArray::Delete(idx);
            }else
                break;
            q--;
        }
    }
    void SetQ(int q)
    {
        unsigned lq=m_q;
        if((unsigned)q<lq)
        {
            unsigned i=q;
            while(i<lq)
            {
                ((CHClass<T> *)(_get(i)))->~CHClass();
                i++;
            }
            this->CNDArray::SetQ(q);
        }else
        if((unsigned)q>lq)
        {
            this->CNDArray::SetQ(q);
            unsigned i=lq;
            while(i<q)
            {
                *(_get(i))=0;
                i++;
            }
        }
    }
    void Reset()
    {
        int i=0;
        while(i<GetQ())
        {
            ((CHClass<T> *)(_get(i)))->~CHClass();
            i++;
        }
        this->CNDArray::Reset();
    }
    void Clear()
    {
        int i=0;
        while(i<GetQ())
        {
            ((CHClass<T> *)(_get(i)))->~CHClass();
            i++;
        }
        this->CNDArray::Clear();
    }
    CHClass<T> & operator [](int index)
    {
        return *((CHClass<T> *)(Get(index)));
    }

    CHCArray<T> & operator =(CHCArray<T> &src)
    {
        Reset();
        int i;
        for(i=0;i<src.GetQ();i++)
            Append(*((CHClass<T> *)(src._get(i))));
        return *this;
    }

    unsigned FindLast(T *val)
    {
        unsigned i=m_num;
        while(i--)
        {
            if(((CHClass<T> *)(_get(i)))->m_val==val)
                return i;
        }
        return -1;
    }
    unsigned FindLast(T &val)
    {
        unsigned i=m_num;
        while(i--)
        {
            if(*((CHClass<T> *)(_get(i)))==&val)
                return i;
        }
    }

    unsigned Find(T *val)
    {
        unsigned i=0;
        while(i<m_q)
        {
            if(((CHClass<T> *)(_get(i)))->m_val==val)
                return i;
            i++;
        }
        return -1;
    }
    unsigned Find(T &val)
    {
        unsigned i=0;
        while(i<m_q)
        {
            if(*((CHClass<T> *)(_get(i)))==&val)
                return i;
            i++;
        }
    }

    void Compact()
    {
        unsigned i=0;
        while(i<GetQ() && ((CHClass<T> *)(_get(i)))->m_val!=0)
        {
            i++;
        }
        if(i<GetQ())
        {
            unsigned j=i;
            while(i<GetQ())
            {
                if(((CHClass<T> *)(_get(i)))->m_val)
                {
                    *((CHClass<T> *)(_get(j)))=*((CHClass<T> *)(_get(i)));
                    j++;
                }
                i++;
            }
            SetQ(j);
        }
    }

    CHCArray():CNDArray(4){}
   ~CHCArray(){ Clear(); }
};

}

#endif // CHCARRAY_H
