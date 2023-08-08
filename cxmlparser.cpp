#include "cxmlparser.h"

void CXMLParser::EntityCodepageCheck()
{
//    if(m_codepage.Length())
//        m_entity.Recode(m_codepage,"windows-1251");
}

void CXMLParser::xml_sript()
{
    if(!m_entity.size())
        return;
    char *str=(char *)m_entity.c_str();
    char *ee=strstr(str,"encoding=");
    if(ee!=0)
    {
        char *str1=strchr(ee+1,'"');
        if(str1!=0)
        {
            str1++;
            char *str2=strchr(str1,'"');
            if(str2!=0)
            {
                m_codepage.clear();
                m_codepage.append(str1,(unsigned)str2-(unsigned)str1);
                strlwr((char *)m_codepage.c_str());
                if(m_codepage=="windows-1251")
                    m_codepage.clear();
            }
        }
    }
}

void CXML_tag::SetEntity(std::string &text)
{
    if(m_parser)
        m_parser->SetEntity(m_name,text);
    else
    if(m_parent)
        m_parent->SetEntity(m_name,text);
}

void CXML_tag::SetEntity(std::string &att,std::string &text)
{
    if(m_parser)
        m_parser->SetEntity(att,text);
}

CXML_tag::CXML_tag(CXMLParser *pp,const CXML_tag *parent,std::string &name)
{
    m_parser=0;
    m_parent = (CXML_tag *)parent;
    m_name   = name;
    if(m_parent)
        m_parser=m_parent->AddAttr(m_name);
    else
    if(pp && pp->m_parser)
        m_parser=pp->m_parser->AddAttr(m_name);
}

CXML_tag::~CXML_tag()
{
    if(m_parser)
        delete m_parser;
}

CXML_AttrParser *CXML_tag::AddAttr(std::string &att)
{
    if(m_parser)
       return m_parser->AddAttr(att);
    return 0;
}

CXMLParser::CXMLParser(FILE *in,CXML_AttrParser *parser)
{
    m_xml=in;
    m_parser=parser;
}

CXMLParser::~CXMLParser()
{
    if(m_xml)
        fclose(m_xml);
    if(m_parser)
        delete m_parser;
}

enum XMLS_ENUM{
    XMLS_SYNC,
    XMLS_TAG,
    XMLS_TAGE,
    XMLS_SCRIPT,
    XMLS_SCRIPTN,
    XMLS_SCRIPTE,
    XMLS_SCRIPT_ENTITY,
    XMLS_SCRIPT_ENTITYE,
    XMLS_COMMENT1,
    XMLS_TAGNAME,
    XMLS_TAGDATA,
    XMLS_TAGDATAE,
    XMLS_TAGDATA_ATN,
    XMLS_TAGDATA_ATD1,
    XMLS_TAGDATA_ATD2,
    XMLS_TAGDATA_ATD3
};

void CXMLParser::Push()
{
    CXML_tag *tag;
    if(m_stack.size())
    {
        CXML_tag *ptag=m_stack[m_stack.size()-1];
        tag=new CXML_tag(0,ptag,m_tname);
    }else
    {
        tag=new CXML_tag(this,0,m_tname);
    }
    m_stack.append((CXML_tag *)tag);
}

void CXMLParser::Pop()
{
    CXML_tag* tag;
    if(m_stack.size())
    {
        tag=m_stack[m_stack.size()-1];
        m_stack.removeAt(m_stack.size()-1);
        delete tag;
    }
}


int CXMLParser::Parse()
{
    int state=XMLS_SYNC;
    // Основной цикл
    do{
        int ival=fgetc(m_xml);
        if(ival==EOF)
            return 1;
        char val=ival;
        m_buffer+=val;
        switch(state)
        {
        case XMLS_SYNC: // Синхронизация и поиск тегов
            if(val=='<') // Поиск тегов
            {
                if(m_stack.size() && m_entity.size())
                {
                    EntityCodepageCheck();
                    m_stack[m_stack.size()-1]->SetEntity(m_entity);
                    m_entity.clear();
                }
                m_tname.clear();
                m_buffer.clear();
                m_buffer+=val;
                state=XMLS_TAG;
            }else // Содержимое, если есть его хозяин ведем накопление
            if(m_stack.size())
            {
                if(val!=0xD && val!=0xA) // Пропускаем переносы строк
                    m_entity+=val;
            }
            break;
        case XMLS_TAG: // Распознование тегов
            switch(val)
            {
            case '/': // конец тега
                state=XMLS_TAGE;
                break;
            case '?': // Скрипт
                state=XMLS_SCRIPT;
                break;
            case '!': // коммнтарий
                state=XMLS_COMMENT1;
                break;
            case '\n':
            case '\r':
            case ' ':
                break;
            default:
                m_tname+=val;
                state=XMLS_TAGNAME;
            }
            break;
        case XMLS_COMMENT1:
            if(val=='>')
                state=XMLS_SYNC;
            break;
        case XMLS_TAGNAME:
            switch(val)
            {
            case '>':
                {
                    Push();
                    state=XMLS_SYNC;
                    m_buffer.clear();
                }
                break;
            case '\n':
            case '\r':
            case ' ':
                {
                    Push();
                    state=XMLS_TAGDATA;
                }
                break;
            case '/': // Пустой тег
                state=XMLS_TAGDATAE;
                m_buffer.clear();
                break;
            default:
                m_tname+=val;
            }
            break;
        case XMLS_TAGDATA:
            switch(val)
            {
            case '>':
                state=XMLS_SYNC;
                m_buffer.clear();
                break;
            case '/':
                {
                    Pop();
                    m_buffer.clear();
                    state=XMLS_TAGDATAE;
                }
                break;
            case '\n':
            case '\r':
            case ' ':
                break;
            default:
                m_tname.clear();
                m_tname+=val;
                state=XMLS_TAGDATA_ATN;
            }
            break;
        case XMLS_TAGDATA_ATN:
            switch(val)
            {
            case '>':
                state=XMLS_SYNC;
                m_buffer.clear();
                break;
            case '\n':
            case '\r':
            case ' ':
                state=XMLS_TAGDATA_ATD1;
                break;
            case '=':
                state=XMLS_TAGDATA_ATD2;
                break;
            default:
                m_tname+=val;
            }
            break;
        case XMLS_TAGDATA_ATD1:
            switch(val)
            {
            case '>':
                state=XMLS_SYNC;
                m_buffer.clear();
                break;
            case '=':
                state=XMLS_TAGDATA_ATD2;
                break;
            }
            break;
        case XMLS_TAGDATA_ATD2:
            switch(val)
            {
            case '>':
                state=XMLS_SYNC;
                m_buffer.clear();
                break;
            case '"':
                state=XMLS_TAGDATA_ATD3;
                m_entity.clear();
                break;
            }
            break;
        case XMLS_TAGDATA_ATD3:
            switch(val)
            {
            case '"':
                state=XMLS_TAGDATA;
                if(m_tname.size() && m_entity.size())
                {
                    EntityCodepageCheck();
                    m_stack[m_stack.size()-1]->SetEntity(m_tname,m_entity);
                }
                m_entity.clear();
                break;
            default:
                m_entity+=val;
            }
            break;
        case XMLS_TAGE:
            switch(val)
            {
            case '>':
                {
                    Pop();
                    state=XMLS_SYNC;
                    m_buffer.clear();
                }
                break;
            case '\n':
            case '\r':
            case ' ':
                {
                    Pop();
                    m_buffer.clear();
                    state=XMLS_TAGDATAE;
                }
                break;
            default:
                m_tname+=val;
            }
            break;
        case XMLS_TAGDATAE:
            switch(val)
            {
            case '>':
                state=XMLS_SYNC;
                m_buffer.clear();
                break;
            }
            break;
        case XMLS_SCRIPT:
            switch(val)
            {
            case '?':
                state=XMLS_SCRIPTE;
                m_buffer.clear();
                m_buffer+=val;
                break;
            case ' ':
                break;
            default:
                if(val>='A' && val<='z')
                {
                    m_tname+=val;
                    state=XMLS_SCRIPTN;
                }
            }
            break;
        case XMLS_SCRIPTN:
            switch(val)
            {
            case '?':
                state=XMLS_SCRIPTE;
                m_buffer.clear();
                m_buffer+=val;
                break;
            case ' ':
                state=XMLS_SCRIPT_ENTITY;
                m_entity.clear();
                break;
            default:
                if(val>='A' && val<='z')
                {
                    m_tname+=val;
                }
            }
            break;
        case XMLS_SCRIPTE:
            if(val=='>')
            {
                state=XMLS_SYNC; // Скрипт обработан
            }else
            {
                m_tname+=m_buffer;
                state=XMLS_SCRIPTN;
            }
            break;
        case XMLS_SCRIPT_ENTITY:
            switch(val)
            {
            case '?':
                state=XMLS_SCRIPT_ENTITYE;
                m_buffer.clear();
                m_buffer+=val;
                break;
            default:
                 m_entity+=val;
            }
            break;
        case XMLS_SCRIPT_ENTITYE:
            if(val=='>')
            {
                state=XMLS_SYNC; // Скрипт обработан
                if(m_tname=="xml")
                    xml_sript();
            }else
            {
                m_entity+=m_buffer;
                state=XMLS_SCRIPT_ENTITY;
            }
            break;
        default:
            //::MessageBeep(MB_OK);
            return -1;
        }
    }while(1);
}

