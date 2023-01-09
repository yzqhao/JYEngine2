#pragma once

#include "../../Core.h"

NS_JYE_BEGIN

class CORE_API IEncoder
{
public:
	IEncoder() {}
	virtual ~IEncoder() {}
public:
	virtual IEncoder* GetChild() = 0;
	virtual void SetField(const char* name, uint nlen) = 0;//null变量
	virtual void SetField(const char* name, uint nlen, bool v) = 0;
	virtual void SetField(const char* name, uint nlen, short v) = 0;
	virtual void SetField(const char* name, uint nlen, uint16 v) = 0;
	virtual void SetField(const char* name, uint nlen, int v) = 0;
	virtual void SetField(const char* name, uint nlen, uint v) = 0;
	virtual void SetField(const char* name, uint nlen, float v) = 0;
	virtual void SetField(const char* name, uint nlen, int64 v) = 0;
	virtual void SetField(const char* name, uint nlen, uint64 v) = 0;
	virtual void SetField(const char* name, uint nlen, double v) = 0;
	virtual void SetField(const char* name, uint nlen, const char* v, uint len) = 0;
	virtual void SetField(const char* name, uint nlen, const IEncoder* v) = 0;
	virtual void SetField(int index) = 0;//null变量
	virtual void SetField(int index, bool v) = 0;
	virtual void SetField(int index, short v) = 0;
	virtual void SetField(int index, uint16 v) = 0;
	virtual void SetField(int index, int v) = 0;
	virtual void SetField(int index, uint v) = 0;
	virtual void SetField(int index, float v) = 0;
	virtual void SetField(int index, int64 v) = 0;
	virtual void SetField(int index, uint64 v) = 0;
	virtual void SetField(int index, double v) = 0;
	virtual void SetField(int index, const char* v, uint len) = 0;
	virtual void SetField(int index, const IEncoder* v) = 0;
	virtual void SetField(uint64 index) = 0;//null变量
	virtual void SetField(uint64 index, bool v) = 0;
	virtual void SetField(uint64 index, short v) = 0;
	virtual void SetField(uint64 index, uint16 v) = 0;
	virtual void SetField(uint64 index, int v) = 0;
	virtual void SetField(uint64 index, uint v) = 0;
	virtual void SetField(uint64 index, float v) = 0;
	virtual void SetField(uint64 index, int64 v) = 0;
	virtual void SetField(uint64 index, uint64 v) = 0;
	virtual void SetField(uint64 index, double v) = 0;
	virtual void SetField(uint64 index, const char* v, uint len) = 0;
	virtual void SetField(uint64 index, const IEncoder* v) = 0;
};

NS_JYE_END