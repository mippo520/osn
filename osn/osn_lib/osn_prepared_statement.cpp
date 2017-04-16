#include "osn_prepared_statement.h"

OsnPreparedStatement::OsnPreparedStatement( const OsnPreparedStatement& right )
{
	*this = right;
}

OsnPreparedStatement& OsnPreparedStatement::operator=( const OsnPreparedStatement& right )
{
	if ( this == &right )
	{
		return *this;
	}

	m_vecStatementData.assign( right.m_vecStatementData.begin(), right.m_vecStatementData.end() );
	return *this;
}

OsnPreparedStatement::OsnPreparedStatement(...)
{
    
}

OsnPreparedStatement::~OsnPreparedStatement()
{

}

//- Bind to buffer
void OsnPreparedStatement::setBool(const oUINT8 index, const oBOOL value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.boolean = value;
	m_vecStatementData[index].type = TYPE_BOOL;
}

void OsnPreparedStatement::setUInt8(const oUINT8 index, const oUINT8 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.ui8 = value;
	m_vecStatementData[index].type = TYPE_UINT8;
}

void OsnPreparedStatement::setUInt16(const oUINT8 index, const oUINT16 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.ui16 = value;
	m_vecStatementData[index].type = TYPE_UINT16;
}

void OsnPreparedStatement::setUInt32(const oUINT8 index, const oUINT32 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.ui32 = value;
	m_vecStatementData[index].type = TYPE_UINT32;
}

void OsnPreparedStatement::setUInt64(const oUINT8 index, const oUINT64 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.ui64 = value;
	m_vecStatementData[index].type = TYPE_UINT64;
}

void OsnPreparedStatement::setInt8(const oUINT8 index, const oINT8 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.i8 = value;
	m_vecStatementData[index].type = TYPE_INT8;
}

void OsnPreparedStatement::setInt16(const oUINT8 index, const oINT16 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.i16 = value;
	m_vecStatementData[index].type = TYPE_INT16;
}

void OsnPreparedStatement::setInt32(const oUINT8 index, const oINT32 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.i32 = value;
	m_vecStatementData[index].type = TYPE_INT32;
}

void OsnPreparedStatement::setInt64(const oUINT8 index, const oINT64 value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.i64 = value;
	m_vecStatementData[index].type = TYPE_INT64;
}

void OsnPreparedStatement::setFloat32(const oUINT8 index, const float value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.f = value;
	m_vecStatementData[index].type = TYPE_FLOAT32;
}

void OsnPreparedStatement::setFloat64(const oUINT8 index, const double value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].data.d = value;
	m_vecStatementData[index].type = TYPE_FLOAT64;
}

void OsnPreparedStatement::setString(const oUINT8 index, const std::string& value)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].str = value;
	m_vecStatementData[index].type = TYPE_STRING;
}

void OsnPreparedStatement::setNull(const oUINT8 index)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index+1);

	m_vecStatementData[index].type = TYPE_NULL;
}

void OsnPreparedStatement::setFunction(const oUINT8 index, const STMT_FUNC &func)
{
    if (index >= m_vecStatementData.size())
        m_vecStatementData.resize(index+1);
    
    m_vecStatementData[index].func = func;
    m_vecStatementData[index].type = TYPE_FUNCTION;
}

void OsnPreparedStatement::setPoint(const oUINT8 index, const void *ptr)
{
	if (index >= m_vecStatementData.size())
		m_vecStatementData.resize(index + 1);

	m_vecStatementData[index].data.ptr = ptr;
	m_vecStatementData[index].type = TYPE_POINT;
}

oBOOL OsnPreparedStatement::getBool( const oUINT8 index ) const
{
	oBOOL value = false;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_BOOL == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.boolean;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oUINT8 OsnPreparedStatement::getUInt8( const oUINT8 index ) const
{
	oUINT8 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT8 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.ui8;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oUINT16 OsnPreparedStatement::getUInt16( const oUINT8 index ) const
{
	oUINT16 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT16 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.ui16;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oUINT32 OsnPreparedStatement::getUInt32( const oUINT8 index ) const
{
	oUINT32 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT32 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.ui32;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oUINT64 OsnPreparedStatement::getUInt64( const oUINT8 index ) const
{
	oUINT64 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT64 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.ui64;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oINT8 OsnPreparedStatement::getInt8( const oUINT8 index ) const
{
	oINT8 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT8 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.i8;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oINT16 OsnPreparedStatement::getInt16( const oUINT8 index ) const
{
	oINT16 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT16 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.i16;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oINT32 OsnPreparedStatement::getInt32( const oUINT8 index ) const
{
	oINT32 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT32 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.i32;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

oINT64 OsnPreparedStatement::getInt64( const oUINT8 index ) const
{
	oINT64 value = 0;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT64 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.i64;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

float OsnPreparedStatement::getFloat32( const oUINT8 index ) const
{
	float value = 0.0f;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_FLOAT32 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.f;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

double OsnPreparedStatement::getFloat64( const oUINT8 index ) const
{
	double value = 0.0f;
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_FLOAT64 == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].data.d;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

std::string OsnPreparedStatement::getString( const oUINT8 index ) const
{
	std::string value = "";
	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_STRING == m_vecStatementData[index].type)
		{	
			value = m_vecStatementData[index].str;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
		}
	}

	return value;
}

OsnPreparedStatement::STMT_FUNC OsnPreparedStatement::getFunction(const oUINT8 index)const
{
    STMT_FUNC func;
    if ( index < 0 || index >= m_vecStatementData.size() )
    {
        OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
    }
    else
    {
        if (TYPE_FUNCTION == m_vecStatementData[index].type)
        {
            func = m_vecStatementData[index].func;
        }
        else
        {
            OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, m_vecStatementData[index].type);
        }
    }
    return func;
}

oBOOL OsnPreparedStatement::isEmpty() const
{
	return 0 == m_vecStatementData.size();
}

void OsnPreparedStatement::clear()
{
	m_vecStatementData.clear();
}

void OsnPreparedStatement::printContext() const
{
	std::string strPrintInfo = "";
	oUINT8 unCount = getPreparedStatementDataCount();
	for (oUINT8 i = 0; i < unCount; ++i)
	{
		char sz[256];
		PreparedStatementValueType type = getType(i);
		switch (type)
		{
		case TYPE_BOOL:
			sprintf(sz, "BOOL = %s, ", getBool(i) ? "true" : "false");
			strPrintInfo += sz;
			break;
		case TYPE_INT8:
			sprintf(sz, "INT8 = %d, ", getInt8(i));
			strPrintInfo += sz;
			break;
		case TYPE_UINT8:
			sprintf(sz, "UINT8 = %d, ", getUInt8(i));
			strPrintInfo += sz;
			break;
		case TYPE_INT16:
			sprintf(sz, "INT16 = %d, ", getInt16(i));
			strPrintInfo += sz;
			break;
		case TYPE_UINT16:
			sprintf(sz, "UINT16 = %d, ", getUInt16(i));
			strPrintInfo += sz;
			break;
		case TYPE_INT32:
			sprintf(sz, "INT32 = %d, ", getInt32(i));
			strPrintInfo += sz;
			break;
		case TYPE_UINT32:
			sprintf(sz, "UINT32 = %u, ", getUInt32(i));
			strPrintInfo += sz;
			break;
		case TYPE_INT64:
			sprintf(sz, "INT64 = %ll, ", getInt64(i));
			strPrintInfo += sz;
			break;
		case TYPE_UINT64:
			sprintf(sz, "UINT64 = %llu, ", getUInt64(i));
			strPrintInfo += sz;
			break;
		case TYPE_FLOAT32:
			sprintf(sz, "FLOAT32 = %f, ", getFloat32(i));
			strPrintInfo += sz;
			break;
		case TYPE_FLOAT64:
			sprintf(sz, "FLOAT64 = %lf, ", getFloat64(i));
			strPrintInfo += sz;
			break;
		case TYPE_STRING:
			sprintf(sz, "STRING = %s, ", getString(i).c_str());
			strPrintInfo += sz;
			break;
		case TYPE_POINT:
			sprintf(sz, "POINT = %llx, ", getPoint<void>(i));
			strPrintInfo += sz;
			break;
		case TYPE_FUNCTION:
			sprintf(sz, "FUNCTION, ");
			strPrintInfo += sz;
			break;
		case TYPE_NULL:
			sprintf(sz, "NULL, ");
			strPrintInfo += sz;
			break;
		default:
			break;
		}
	}
	strPrintInfo += "\n";
	printf("OsnPreparedStatement = %s", strPrintInfo.c_str());
}

PreparedStatementValueType OsnPreparedStatement::getType( const oUINT8 index ) const
{
	PreparedStatementValueType eType = TYPE_NULL;

	if ( index < 0 || index >= m_vecStatementData.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		eType = m_vecStatementData[index].type;
	}

	return eType;
}

oUINT8 OsnPreparedStatement::getPreparedStatementDataCount() const
{
	return static_cast<oUINT8>(m_vecStatementData.size());
}

void OsnPreparedStatement::errorInvalidIndex(const std::string &strFuncName, const oUINT8 index)
{
//	ccDebugLog("##error %s => invalid index:%d", pszFuncName, index);
}

void OsnPreparedStatement::errorTypeMismatch(const std::string &strFuncName, const oUINT8 index, PreparedStatementValueType eType)
{
//	ccDebugLog("##error %s => type mismatch, index:%d, type:%d", pszFuncName, index, eType);
}


