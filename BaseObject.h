#pragma once

// CBaseObject command target

class CBaseObject : public CObject
{
	DECLARE_DYNAMIC(CBaseObject)

protected:
	CString m_path;

	CBaseObject(const CString& filePath);
public:
	virtual ~CBaseObject();

	inline CString GetPath() {return m_path;}
	virtual CString		GetName();
	virtual ULONGLONG	GetSizeOnDisc() = 0;

};


