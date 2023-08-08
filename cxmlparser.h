#ifndef CXMLPARSER_H
#define CXMLPARSER_H

#include <QObject>
//#include <QPointer>
#include <QSharedDataPointer>
#include <QString>

class CXML_AttrParser
{
public:
    virtual CXML_AttrParser *AddAttr(std::string &name){ return 0; };
    virtual void SetEntity(std::string &att,std::string &text){};
    virtual ~CXML_AttrParser(){};
};

class CXMLParser;
class CXML_tag
{
public:
    CXML_tag        *m_parent;
    CXML_AttrParser *m_parser;
    std::string      m_name;

    void SetEntity(std::string &text);
    void SetEntity(std::string &att,std::string &text);
    CXML_AttrParser *AddAttr(std::string &att);
    CXML_tag(CXMLParser *pp,const CXML_tag *parent,std::string &name);
    ~CXML_tag();
};

class CXMLParser
{
public:
    FILE *m_xml;
    QList<CXML_tag *>  m_stack;
    std::string        m_entity;
    std::string        m_buffer;
    std::string        m_tname;
    CXML_AttrParser   *m_parser;
    std::string        m_codepage;

    void Push();
    void Pop();
    int  Parse();
    void xml_sript();
    void EntityCodepageCheck();

    CXMLParser(FILE *,CXML_AttrParser *);
   ~CXMLParser();
};

#endif // CXMLPARSER_H
