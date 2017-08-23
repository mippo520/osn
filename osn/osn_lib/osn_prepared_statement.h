#ifndef _PreparedStatement_h__
#define _PreparedStatement_h__
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include "osn_type.h"
#include "osn_singleton.h"

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
    TYPE_NULL
};

class OsnPreparedStatement;
typedef std::function<OsnPreparedStatement (const OsnPreparedStatement &)> STMT_FUNC;
typedef std::function<void (const OsnPreparedStatement &)> VOID_STMT_FUNC;

class PreparedStatementData
{
public:
    PreparedStatementData();
    
    void setBool(const oBOOL value);
    void setInt32(const oINT32 value);
    void setString(const char *sz);
    void setNull();
    
    oBOOL getBool() const;
    oINT32 getInt32() const;
    std::string getString() const;
    const char* getCharPtr() const;
    oBOOL isNull() const;
    
    PreparedStatementValueType getType() const;
private:
    friend class OsnPreparedStatement;
    PreparedStatementDataUnion data;
    PreparedStatementValueType type;
    mutable std::string str;
    VOID_STMT_FUNC func;
};

class OsnPreparedStatement
{
public:
    OsnPreparedStatement();
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
	void setBuffer(const oUINT8 index, const oINT8 *pBuffer, oUINT32 sz);

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
    const char* getCharPtr(const oUINT8 index)const;
    const oINT8* getBuffer(const oUINT8 index, oUINT32 &sz)const;

    oINT32 popBackInt32() const;
    oUINT32 popBackUInt32() const;
    oUINT64 popBackUInt64() const;
    void pushBackInt32(oINT32 nValue) const;
    void pushBackUInt32(oUINT32 unValue) const;
    void pushBackUInt64(oUINT64 unValue) const;
    
	oBOOL isEmpty() const;
	void clear();
	void printContext() const;

	// 返回类型，调试用
	PreparedStatementValueType getType( const oUINT8 index ) const;
	oUINT8 getPreparedStatementDataCount() const;
private:
    friend class OsnService;
    void setFunction(const oUINT8 index, const VOID_STMT_FUNC &func);
    VOID_STMT_FUNC getFunction(const oUINT8 index)const;
    
    static void errorInvalidIndex(const std::string &strFuncName, const oUINT8 index);
	static void errorTypeMismatch(const std::string &strFuncName, const oUINT8 index, PreparedStatementValueType eType);
public:
    static std::shared_ptr<OsnPreparedStatement> s_SharedPtr;
protected:
    typedef std::vector<PreparedStatementData> VEC_DATA;
    mutable VEC_DATA m_vecStatementData;
};

#define STMT_NONE OsnSingleton<OsnPreparedStatement>::instance()
#define SHARED_PTR_STMT_NONE OsnPreparedStatement::s_SharedPtr

typedef std::function<void (ID_SERVICE, ID_SESSION, const OsnPreparedStatement &)> DISPATCH_FUNC;
typedef std::map<oINT32, DISPATCH_FUNC> MAP_DISPATCH_FUNC;
typedef MAP_DISPATCH_FUNC::iterator MAP_DISPATCH_FUNC_ITR;
typedef std::shared_ptr<OsnPreparedStatement> SHARED_PTR_STMT;

#endif//_PreparedStatement_h__
