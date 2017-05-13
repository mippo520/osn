//
//  osn_macro.h
//  osn
//
//  Created by zenghui on 17/4/5.
//  Copyright © 2017年 zenghui. All rights reserved.
//

#ifndef osn_macro_h
#define osn_macro_h

#define SAFE_DELETE(x)  if(NULL != x) { delete x; x = NULL; }
#define SAFE_FREE(x)    if(NULL != x) { free(x); x = NULL; }

#define MEMBER_VALUE(type, name)    \
    private:    \
        type m_##name;  \
    public: \
        const type& get##name() const    \
        { return m_##name; }    \
        void set##name(const type &value)   \
        { m_##name = value; }

#define ATOM_INC(ptr) __sync_add_and_fetch(ptr, 1)
#define ATOM_FINC(ptr) __sync_fetch_and_add(ptr, 1)
#define ATOM_DEC(ptr) __sync_sub_and_fetch(ptr, 1)
#define ATOM_FDEC(ptr) __sync_fetch_and_sub(ptr, 1)
#define ATOM_ADD(ptr,n) __sync_add_and_fetch(ptr, n)
#define ATOM_SUB(ptr,n) __sync_sub_and_fetch(ptr, n)
#define ATOM_AND(ptr,n) __sync_and_and_fetch(ptr, n)
#define ATOM_CAS(ptr, oval, nval) __sync_bool_compare_and_swap(ptr, oval, nval)
#define ATOM_SET(ptr,n) __sync_lock_test_and_set(ptr, n)

#define AddService_Declare(SERVICE_NAME)	\
class SERVICE_NAME##Factory : public IServiceFactory	\
{	\
public:	\
	virtual OsnService* create();	\
};	\
class IOsn;	\
class IOsnService;	\
class IOsnCoroutine;	\
class IOsnSocket;	\
class IServiceFactory;	\
class OsnServiceFactory;	\
extern "C"	\
{	\
	oBOOL init##SERVICE_NAME(const IOsn *pOsn, const IOsnService *pService, const IOsnCoroutine *pCoroutine, const IOsnSocket *pSocket);	\
	IServiceFactory* get##SERVICE_NAME##Factory();	\
}

#define AddService_Instance(SERVICE_NAME)	\
OsnService* SERVICE_NAME##Factory::create()	\
{	\
	return new SERVICE_NAME();	\
}	\
const IOsn *g_Osn = NULL;	\
const IOsnService *g_Service = NULL;	\
const IOsnCoroutine *g_Coroutine = NULL;	\
const IOsnSocket *g_Socket = NULL;	\
extern "C"	\
{	\
	oBOOL init##SERVICE_NAME(const IOsn *pOsn	\
		, const IOsnService *pService	\
		, const IOsnCoroutine *pCoroutine	\
		, const IOsnSocket *pSocket)	\
	{	\
		oBOOL bRet = false;	\
		printf("%s init!\n", #SERVICE_NAME);	\
		if (NULL != pOsn && NULL != pService && NULL != pCoroutine && NULL != pSocket)	\
		{	\
			g_Osn = pOsn;	\
			g_Service = pService;	\
			g_Coroutine = pCoroutine;	\
			g_Socket = pSocket;	\
			bRet = true;	\
		}	\
		else	\
		{	\
			printf("dylib init error! Global value is NULL!\n");	\
		}	\
		return bRet;	\
	}	\
	IServiceFactory* get##SERVICE_NAME##Factory()	\
	{	\
		printf("%s getfactory!\n", #SERVICE_NAME);	\
		return new SERVICE_NAME##Factory();	\
	}	\
}


#endif /* osn_macro_h */
