﻿#ifndef __SHADERS_H__
#define __SHADERS_H__
#include <gmcommon.h>
#include <gamemachine.h>
BEGIN_NS

struct GMGLShaderInfo
{
	GMuint type;
	GMString source;
	GMString filename;

	static GMuint toGLShaderType(GMShaderType type);
};

typedef AlignedVector<GMuint> GMGLShaderIDList;
typedef AlignedVector<GMGLShaderInfo> GMGLShaderInfos;

GM_PRIVATE_OBJECT(GMGLShaderProgram)
{
	GMGLShaderInfos shaderInfos;
	GMGLShaderIDList shaders;
	GMuint shaderProgram = 0;
	static GMuint lastUsedProgram;
	Map<GMString, GMString> aliasMap;
};

class GMGLShaderProgram : public GMObject, public IShaderProgram
{
	DECLARE_PRIVATE(GMGLShaderProgram);

public:
	GMGLShaderProgram() = default;
	~GMGLShaderProgram();

	void load();
	GMuint getProgram() { D(d); return d->shaderProgram; }
	void attachShader(const GMGLShaderInfo& shaderCfgs);

public:
	virtual void useProgram();
	virtual void setMatrix4(const char* name, const GMMat4& value);
	virtual void setVec4(const char* name, const GMFloat4& value);
	virtual void setVec3(const char* name, const GMfloat value[3]);
	virtual void setInt(const char* name, GMint value);
	virtual void setFloat(const char* name, GMfloat value);
	virtual void setBool(const char* name, bool value);
	virtual bool setInterfaceInstance(const char* interfaceName, const char* instanceName, GMShaderType type) override;
	virtual const GMShaderVariablesDesc& getDesc();

	void setMatrix4(const GMString& name, const GMMat4& value);
	void setVec4(const GMString& name, const GMFloat4& value);
	void setVec3(const GMString& name, const GMfloat value[3]);
	void setInt(const GMString& name, GMint value);
	void setFloat(const GMString& name, GMfloat value);
	void setBool(const GMString& name, bool value);

private:
	void setProgram(GMuint program) { D(d); d->shaderProgram = program; }
	void removeShaders();
	void expandSource(REF GMGLShaderInfo& shaderInfo);
	GMString expandSource(const GMString& workingDir, const GMString& source);
	bool matchMacro(const GMString& source, const GMString& macro, REF GMString& result);
	void expandInclude(const GMString& workingDir, const GMString& fn, IN OUT GMString& source);
	void expandAlias(const GMString& alias, IN OUT GMString& source);
	GMString& replaceLine(IN OUT GMString& line);
	bool setSubrotinue(const char* funcName, const char*  implement, GMuint shaderType);
	bool verify();

public:
	virtual bool getInterface(GameMachineInterfaceID id, void** out) override;
	virtual bool setInterface(GameMachineInterfaceID id, void* in) override;

public:
	static constexpr const char* techniqueName()
	{
		return "GM_techniqueEntrance";
	}
};

END_NS
#endif