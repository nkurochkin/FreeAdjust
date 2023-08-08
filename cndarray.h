#ifndef CNDARRAY_H
#define CNDARRAY_H

class CNDArray
{
public:
    unsigned char**  m_mem;     //  Offset table
    unsigned   m_memSize; //  OFT memory size
    unsigned   m_memNum;  //  OFT actual size

    unsigned   m_idxMask;  //  Index mask
    unsigned   m_offPower; //  Offset shift count
    unsigned   m_unit;

    void _checkSize(unsigned);
public:
    void CheckSizeClear(unsigned);

    unsigned  m_num;
    unsigned  Num(){ return m_num; }
    unsigned  GetQ(){ return m_num; }
    unsigned  size(){ return m_num; }

    unsigned  AddSpace(void);       // Append space for 1 object
    unsigned  AddSpace(unsigned);  // Append space for N objects
    unsigned  AddEmptySpace(unsigned);  // Append space for N objects and Clear
    unsigned  Append(void *);
    unsigned  AppendNew(void *);
    void   _clear();
    void   Clear();
    void   Delete(unsigned);
    void   FastDelete(unsigned);
    void*  Get(unsigned);

    void* _get(unsigned idx)
    {
        return (void *)(m_mem[idx>>m_offPower]+(idx&m_idxMask)*m_unit);
    }

    void   Init(unsigned unit,unsigned cluster);
    unsigned  Insert(unsigned,void *);
    unsigned  Put(unsigned,void *,int);
    void   Reset()
    {
        m_num=0;
    }
    void   CheckSize(unsigned size)
    {
        _checkSize(size);
    }

    CNDArray();
    CNDArray(unsigned unit,unsigned cluster=0);
   ~CNDArray();
};

#endif // CNDARRAY_H
