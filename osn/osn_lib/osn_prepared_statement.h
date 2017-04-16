#ifndef _PreparedStatement_h__
#define _PreparedStatement_h__
#include <string>
#include <vector>
#include <functional>
#include "osn.h"

//- Union for data buffer (upper-level bind -> queue -> lower-level bind)
union PreparedStatementDataUnion
{
    oBOOL boolean;
    oUINT8 ui8;
    oINT8 i8;
    oUINT16 ui16;
    oINT16 i16;
    oUINT32 ui32;
    oINT32 i32;
    oUINT64 ui64;
    oINT64 i64;
    oFLOAT32 f;
    oFLOAT64 d;
	const void *ptr;
};

//- This enum helps us differ data held in above union
enum PreparedStatementValueType
{
    TYPE_BOOL,
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_FLOAT32,
    TYPE_FLOAT64,
    TYPE_STRING,
    TYPE_FUNCTION,
	TYPE_POINT,
    TYPE_NULL
};

class OsnPreparedStatement
{
public:
    typedef std::function<OsnPreparedStatement (const OsnPreparedStatement &)> STMT_FUNC;

    struct PreparedStatementData
    {
        PreparedStatementDataUnion data;
        PreparedStatementValueType type;
        std::string str;
        STMT_FUNC func;
    };
public:
    OsnPreparedStatement(...);
	OsnPreparedStatement(const OsnPreparedStatement& right);
	OsnPreparedStatement& operator=(const OsnPreparedStatement& right);
	~OsnPreparedStatement();

	void setBool(const oUINT8 index, const oBOOL value);
	void setUInt8(const oUINT8 index, const oUINT8 value);
	void setUInt16(const oUINT8 index, const oUINT16 value);
	void setUInt32(const oUINT8 index, const oUINT32 value);
	void setUInt64(const oUINT8 index, const oUINT64 value);
	void setInt8(const oUINT8 index, const oINT8 value);
	void setInt16(const oUINT8 index, const oINT16 value);
	void setInt32(const oUINT8 index, const oINT32 value);
	void setInt64(const oUINT8 index, const oINT64 value);
	void setFloat32(const oUINT8 index, const oFLOAT32 value);
	void setFloat64(const oUINT8 index, const oFLOAT64 value);
	void setString(const oUINT8 index, const std::string& value);
	void setNull(const oUINT8 index);
    void setFunction(const oUINT8 index, const STMT_FUNC &func);
	void setPoint(const oUINT8 index, const void *ptr);

	oBOOL getBool(const oUINT8 index) const;
	oUINT8 getUInt8(const oUINT8 index) const;
	oUINT16 getUInt16(const oUINT8 index)const;
	oUINT32 getUInt32(const oUINT8 index)const;
	oUINT64 getUInt64(const oUINT8 index)const;
	oINT8 getInt8(const oUINT8 index)const;
	oINT16 getInt16(const oUINT8 index)const;
	oINT32 getInt32(const oUINT8 index)const;
	oINT64 getInt64(const oUINT8 index)const;
	oFLOAT32 getFloat32(const oUINT8 index)const;
	oFLOAT64 getFloat64(const oUINT8 index)const;
	std::string getString(const oUINT8 index)const;
    STMT_FUNC getFunction(const oUINT8 index)const;
	template<class T>
	const T* getPoint(const oUINT8 index)const
	{
		const T* ptr = NULL;
		if (index < 0 || index >= m_vecStatementData.size())
		{
			OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
		}
		else
		{
			if (TYPE_POINT == m_vecStatementData[index].type)
			{
				ptr = static_cast<const T*>(m_vecStatementData[index].data.ptr);
			}
			else
			{
				OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
			}
		}

		return ptr;
	}

	oBOOL isEmpty() const;
	void clear();
	void printContext() const;

	// 返回类型，调试用
	PreparedStatementValueType getType( const oUINT8 index ) const;
	oUINT8 getPreparedStatementDataCount() const;

private:
	static void errorInvalidIndex(const std::string &strFuncName, const oUINT8 index);
	static void errorTypeMismatch(const std::string &strFuncName, const oUINT8 index, PreparedStatementValueType eType);
protected:
	std::vector<PreparedStatementData> m_vecStatementData;    //- Buffer of parameters, not tied to MySQL in any way yet
};


#endif//_PreparedStatement_h__
