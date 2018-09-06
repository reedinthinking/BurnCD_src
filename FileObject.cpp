// FileObject.cpp : implementation file
//

#include "stdafx.h"
#include "BurnCD.h"
#include "FileObject.h"

#define SECTOR_SIZE	2048

// CFileObject
IMPLEMENT_DYNAMIC(CFileObject, CBaseObject)

CFileObject::CFileObject(const CString& filePath)
: CBaseObject(filePath)
, m_pStream(NULL)
{
}

CFileObject::~CFileObject()
{
    if (m_pStream != NULL)
        m_pStream->Release();
}


// CFileObject member functions

ULONGLONG CFileObject::GetSizeOnDisc()
{
    CFileStatus status;
    if (CFile::GetStatus(m_path, status))
    {
        if (status.m_size > 0)
        {
            return ((status.m_size / SECTOR_SIZE) + 1) * SECTOR_SIZE;
        }
    }

    return 0;
}

IStream* CFileObject::GetStream()
{
    if (m_pStream == NULL)
    {
        SHCreateStreamOnFileEx(m_path, 
            STGM_READ|STGM_SHARE_DENY_NONE|STGM_DELETEONRELEASE,
            FILE_ATTRIBUTE_NORMAL, 
            FALSE, 
            NULL, 
            &m_pStream);
    }

    return m_pStream;
}

