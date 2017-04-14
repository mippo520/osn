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

	statement_data.assign( right.statement_data.begin(), right.statement_data.end() );
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
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.boolean = value;
	statement_data[index].type = TYPE_BOOL;
}

void OsnPreparedStatement::setUInt8(const oUINT8 index, const oUINT8 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.ui8 = value;
	statement_data[index].type = TYPE_UINT8;
}

void OsnPreparedStatement::setUInt16(const oUINT8 index, const oUINT16 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.ui16 = value;
	statement_data[index].type = TYPE_UINT16;
}

void OsnPreparedStatement::setUInt32(const oUINT8 index, const oUINT32 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.ui32 = value;
	statement_data[index].type = TYPE_UINT32;
}

void OsnPreparedStatement::setUInt64(const oUINT8 index, const oUINT64 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.ui64 = value;
	statement_data[index].type = TYPE_UINT64;
}

void OsnPreparedStatement::setInt8(const oUINT8 index, const oINT8 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.i8 = value;
	statement_data[index].type = TYPE_INT8;
}

void OsnPreparedStatement::setInt16(const oUINT8 index, const oINT16 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.i16 = value;
	statement_data[index].type = TYPE_INT16;
}

void OsnPreparedStatement::setInt32(const oUINT8 index, const oINT32 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.i32 = value;
	statement_data[index].type = TYPE_INT32;
}

void OsnPreparedStatement::setInt64(const oUINT8 index, const oINT64 value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.i64 = value;
	statement_data[index].type = TYPE_INT64;
}

void OsnPreparedStatement::setFloat(const oUINT8 index, const float value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.f = value;
	statement_data[index].type = TYPE_FLOAT32;
}

void OsnPreparedStatement::setDouble(const oUINT8 index, const double value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].data.d = value;
	statement_data[index].type = TYPE_FLOAT64;
}

void OsnPreparedStatement::setString(const oUINT8 index, const std::string& value)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].str = value;
	statement_data[index].type = TYPE_STRING;
}

void OsnPreparedStatement::setNull(const oUINT8 index)
{
	if (index >= statement_data.size())
		statement_data.resize(index+1);

	statement_data[index].type = TYPE_NULL;
}

void OsnPreparedStatement::setFunction(const oUINT8 index, const STMT_FUNC &func)
{
    if (index >= statement_data.size())
        statement_data.resize(index+1);
    
    statement_data[index].func = func;
    statement_data[index].type = TYPE_FUNCTION;
}

oBOOL OsnPreparedStatement::getBool( const oUINT8 index ) const
{
	oBOOL value = false;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_BOOL == statement_data[index].type)
		{	
			value = statement_data[index].data.boolean;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oUINT8 OsnPreparedStatement::getUInt8( const oUINT8 index ) const
{
	oUINT8 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT8 == statement_data[index].type)
		{	
			value = statement_data[index].data.ui8;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oUINT16 OsnPreparedStatement::getUInt16( const oUINT8 index ) const
{
	oUINT16 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT16 == statement_data[index].type)
		{	
			value = statement_data[index].data.ui16;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oUINT32 OsnPreparedStatement::getUInt32( const oUINT8 index ) const
{
	oUINT32 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT32 == statement_data[index].type)
		{	
			value = statement_data[index].data.ui32;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oUINT64 OsnPreparedStatement::getUInt64( const oUINT8 index ) const
{
	oUINT64 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_UINT64 == statement_data[index].type)
		{	
			value = statement_data[index].data.ui64;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oINT8 OsnPreparedStatement::getInt8( const oUINT8 index ) const
{
	oINT8 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT8 == statement_data[index].type)
		{	
			value = statement_data[index].data.i8;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oINT16 OsnPreparedStatement::getInt16( const oUINT8 index ) const
{
	oINT16 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT16 == statement_data[index].type)
		{	
			value = statement_data[index].data.i16;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oINT32 OsnPreparedStatement::getInt32( const oUINT8 index ) const
{
	oINT32 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT32 == statement_data[index].type)
		{	
			value = statement_data[index].data.i32;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

oINT64 OsnPreparedStatement::getInt64( const oUINT8 index ) const
{
	oINT64 value = 0;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_INT64 == statement_data[index].type)
		{	
			value = statement_data[index].data.i64;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

float OsnPreparedStatement::getFloat( const oUINT8 index ) const
{
	float value = 0.0f;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_FLOAT32 == statement_data[index].type)
		{	
			value = statement_data[index].data.f;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

double OsnPreparedStatement::getDouble( const oUINT8 index ) const
{
	double value = 0.0f;
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_FLOAT64 == statement_data[index].type)
		{	
			value = statement_data[index].data.d;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

std::string OsnPreparedStatement::getString( const oUINT8 index ) const
{
	std::string value = "";
	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		if (TYPE_STRING == statement_data[index].type)
		{	
			value = statement_data[index].str;
		}
		else
		{
			OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
		}
	}

	return value;
}

OsnPreparedStatement::STMT_FUNC OsnPreparedStatement::getFunction(const oUINT8 index)const
{
    STMT_FUNC func;
    if ( index < 0 || index >= statement_data.size() )
    {
        OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
    }
    else
    {
        if (TYPE_FUNCTION == statement_data[index].type)
        {
            func = statement_data[index].func;
        }
        else
        {
            OsnPreparedStatement::errorTypeMismatch(__FUNCTION__, index, statement_data[index].type);
        }
    }
    return func;
}

PreparedStatementValueType OsnPreparedStatement::getType( const oUINT8 index ) const
{
	PreparedStatementValueType eType = TYPE_NULL;

	if ( index < 0 || index >= statement_data.size() )
	{
		OsnPreparedStatement::errorInvalidIndex(__FUNCTION__, index);
	}
	else
	{
		eType = statement_data[index].type;
	}

	return eType;
}

size_t OsnPreparedStatement::getPreparedStatementDataCount() const
{
	return statement_data.size();
}

void OsnPreparedStatement::errorInvalidIndex(const std::string &strFuncName, const oUINT8 index)
{
//	ccDebugLog("##error %s => invalid index:%d", pszFuncName, index);
}

void OsnPreparedStatement::errorTypeMismatch(const std::string &strFuncName, const oUINT8 index, PreparedStatementValueType eType)
{
//	ccDebugLog("##error %s => type mismatch, index:%d, type:%d", pszFuncName, index, eType);
}


