#include "commonVariable.h"

static std::map<std::string, float> _float_vars;
static std::map<std::string, int> _int_vars;

void SetCommonFloat(std::string key, float value)
{
	_float_vars[key] = value;
}

float GetCommonFloat(std::string key)
{
	return _float_vars[key];
}

void SetCommonInt(std::string key, int value)
{
	_int_vars[key] = value;
}

float GetCommonInt(std::string key)
{
	return _int_vars[key];
}