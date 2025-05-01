// =======================================================
// resourceTool.h
// 
// リソース管理クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#ifndef _RESOURCE_TOOL_H
#define _RESOURCE_TOOL_H

#include "MGCommon.h"

namespace MG {
	constexpr const char* RESOURCE_SCOPE_GOBAL = "gobal";

	struct RESOURCE_NOTE {
		Resource* resource = 0;
		std::list<string> scope;
	};

	class Resource {
	public:
		virtual std::string GetType();
	};

	class Texture : public Resource {
	public:
		std::string GetType() override;
	};

	class Audio : public Resource {
	public:
		std::string GetType() override;
	};

	class ResourceTool {
	protected:
		map<string, RESOURCE_NOTE> __resources;
		virtual void __ReleaseResource(const string& key) = 0;
		void __AddScope(const string& key, const string& scope);
	public:
		virtual Texture* LoadTexture(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		virtual Texture* LoadTexture(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseTexture(const std::string& path, const std::string& scope);
		void ReleaseTexture(unsigned int resourceId, const std::string& scope);
		void ReleaseTexture(const std::string& scope);
		virtual Audio* LoadAudio(const std::string& path, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		virtual Audio* LoadAudio(unsigned int resourceId, const std::string& scope = RESOURCE_SCOPE_GOBAL) = 0;
		void ReleaseAudio(const std::string& path, const std::string& scope);
		void ReleaseAudio(unsigned int resourceId, const std::string& scope);
		void ReleaseAudio(const std::string& scope);
		void ReleaseResource(const std::string& path, const std::string& scope);
		void ReleaseResource(unsigned int resourceId, const std::string& scope);
		void ReleaseResource(Resource* resource, const std::string& scope);
		void ReleaseResource(const std::string& scope);
		void ReleaseAllResource();
	};

} // namespace MG

#endif