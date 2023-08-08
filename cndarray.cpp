#include "cndarray.h"
#include <malloc.h>
#include <string.h>

#define galloc(x)         malloc(x)
#define grealloc(x,y)     realloc(x,y)
#define gfree(x)          free(x)

//****************************************************************
void CNDArray::_checkSize(unsigned nsize)
{
    if(nsize<=m_num) return;
    unsigned memSize=(nsize>>m_offPower)+1; // New number of memory blocks
    if(m_memSize<memSize)
    {
        //  realloc offset table
        m_memSize=((memSize/1000)+1)*1000;
        if(!m_mem) m_mem=(unsigned char **)galloc(m_memSize*4);
        else       m_mem=(unsigned char **)grealloc(m_mem,m_memSize*4);
    }
    while(m_memNum<memSize)
    {
        m_mem[m_memNum]=(unsigned char *)galloc(m_unit*(1L<<m_offPower));
        m_memNum++;
    }
    m_num=nsize;
}

void CNDArray::CheckSizeClear(unsigned nsize)
{
    unsigned onum=m_num;
    _checkSize(nsize);
    if(m_num>onum)
    {
        unsigned i;
        for(i=onum;i<m_num;i++)
            memset(_get(i),0,m_unit);
    }
}

//void *CNDArray::_get(unsigned idx)
//{
//    return (void *)(m_mem[idx>>m_offPower]+(idx&m_idxMask)*m_unit);
//}

void *CNDArray::Get(unsigned idx)
{
    if(idx>=m_num) return 0;
    return _get(idx);
}

unsigned CNDArray::AddSpace()
{
    unsigned idx=m_num;
    _checkSize(m_num+1);
    return idx;
}

unsigned CNDArray::AddSpace(unsigned size)
{
    unsigned idx=m_num;
    _checkSize(m_num+size);
    return idx;
}

unsigned CNDArray::AddEmptySpace(unsigned size)
{
    unsigned rv,i;
    rv=i=m_num;
    _checkSize(m_num+size);
    while(i<m_num)
    {
        memset(Get(i),0,m_unit);
        i++;
    }
    return rv;
}

unsigned CNDArray::Append(void *data)
{
    unsigned idx=m_num;
    _checkSize(m_num+1);
    memcpy((void *)(m_mem[idx>>m_offPower]+(idx&m_idxMask)*m_unit),data,m_unit);
    return idx;
}

unsigned CNDArray::AppendNew(void *data)
{
    unsigned i=0;
    while(i<m_num)
    {
        if(!memcmp(_get(i),data,m_unit)) return i;
        i++;
    }
    return Append(data);
}

void CNDArray::_clear()
{
    if(m_mem)
    {
        unsigned i=0;
        while(i<m_memNum)
        {
            gfree(m_mem[i]);
            i++;
        }
        m_memNum=0;
        m_num     =0;
        gfree(m_mem);
        m_mem    =NULL;
        m_memSize=0;
    }
}

void CNDArray::Clear()
{
    _clear();
}

void CNDArray::Delete(unsigned idx)
{
    if(idx>=m_num) return;
    unsigned memIdx=idx>>m_offPower;
    unsigned sIdx  =idx&m_idxMask;
    void  *last;

    //unsigned rest=(m_idxMask-memIdx)*m_unit;
    unsigned rest=(m_idxMask-sIdx)*m_unit;
    if(rest) // Check
    {
        last=(void *)(m_mem[memIdx]+sIdx*m_unit);
        memmove(last,(void *)((unsigned char *)last+m_unit),rest);
    }
    last=(void *)(m_mem[memIdx]+m_idxMask*m_unit);
    memIdx++;
    rest=m_idxMask*m_unit;

    while(memIdx<m_memNum)
    {
        memcpy(last,m_mem[memIdx],m_unit);
        memmove(m_mem[memIdx],(void *)((unsigned char *)m_mem[memIdx]+m_unit),rest);
        last=(void *)(m_mem[memIdx]+m_idxMask*m_unit);
        memIdx++;
    }
    m_num--;
}

void CNDArray::FastDelete(unsigned idx)
{
    if(idx>m_num) return;
    m_num--;
    if(idx==m_num) return;
    memcpy(_get(idx),_get(m_num),m_unit);
}

unsigned CNDArray::Insert(unsigned at,void *data)
{
    if(at>=m_num)
        at=m_num;
    _checkSize(m_num+1);
    if((at+1)!=m_num)
    {   // Раздвинуть данные
        unsigned ai=m_num-1;
        while((at+1)!=ai)
        {
            memcpy((void *)(m_mem[ai>>m_offPower]+(ai&m_idxMask)*m_unit),
                   (void *)(m_mem[(ai-1)>>m_offPower]+((ai-1)&m_idxMask)*m_unit),m_unit);
            ai--;
        }
    }
    memcpy((void *)(m_mem[at>>m_offPower]+(at&m_idxMask)*m_unit),data,m_unit);
    return at;
}

unsigned CNDArray::Put(unsigned at,void *data,int fill)
{
    unsigned onum=m_num;
    _checkSize(at+1);
    while(onum<at)
    {
        memset((void *)(m_mem[onum>>m_offPower]+(onum&m_idxMask)*m_unit),fill,m_unit);
        onum++;
    }
    memcpy((void *)(m_mem[at>>m_offPower]+(at&m_idxMask)*m_unit),data,m_unit);
    return at;
}

void CNDArray::Init(unsigned unit,unsigned cluster)
{
    Clear();
    m_unit=unit;
    if(!m_unit) m_unit=1;
    if(!cluster)
    {
        m_idxMask =0xFFF;
        m_offPower=12;
    }else
    {
        m_offPower=1;
        m_idxMask=1L;
        while(m_offPower<24 && (m_idxMask+1)<cluster)
        {
            m_idxMask=(m_idxMask<<1)|1;
            m_offPower++;
        }
    }
}

CNDArray::CNDArray()
{
    m_mem      =0;
    m_memSize  =0;
    m_memNum   =0;
    m_idxMask  =0xFFF;
    m_offPower =12;
    m_unit     =1;

    m_num        =0;
}

CNDArray::CNDArray(unsigned unit,unsigned cluster)
{
    m_mem      =0;
    m_memSize  =0;
    m_memNum   =0;
    m_num        =0;
    //m_idxMask  =0xFFF;
    //m_offPower =12;
    //m_unit     =1;
    m_unit=unit;
    if(!m_unit) m_unit=1;
    if(!cluster)
    {
        m_idxMask =0xFFF;
        m_offPower=12;
    }else
    {
        m_offPower=1;
        m_idxMask=1L;
        while(m_offPower<24 && (m_idxMask+1)<cluster)
        {
            m_idxMask=(m_idxMask<<1)|1;
            m_offPower++;
        }
    }
}

CNDArray::~CNDArray()
{
    Clear();
}
