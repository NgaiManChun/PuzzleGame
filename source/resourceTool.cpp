// =======================================================
// resourceTool.cpp
// 
// リソース管理クラス
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "resourceTool.h"

namespace MG {

	std::string Resource::GetType() { return "Resource"; }
	std::string Texture::GetType() { return "Texture"; }
	std::string Audio::GetType() { return "Audio"; }


	// =======================================================
	// 指定したリソースをスコープを追加
	// =======================================================
	void ResourceTool::__AddScope(const string& key, const string& scope)
	{
		if (std::find(__resources[key].scope.begin(), __resources[key].scope.end(), scope) == __resources[key].scope.end()) {
			__resources[key].scope.push_back(scope);
		}
	}


	// =======================================================
	// 指定したテクスチャをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseTexture(const std::string& path, const std::string& scope)
	{
		if (__resources[path].resource && __resources[path].resource->GetType() == "Texture") {
			ReleaseResource(path, scope);
		}
	}


	// =======================================================
	// 指定したテクスチャをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseTexture(unsigned int resourceId, const std::string& scope)
	{
		ReleaseTexture("tex_" + std::to_string(resourceId), scope);
	}


	// =======================================================
	// 指定したスコープからすべてのテクスチャを解放
	// =======================================================
	void ResourceTool::ReleaseTexture(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			ReleaseTexture(pair.first, scope);
		}
	}


	// =======================================================
	// 指定したオーディオをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseAudio(const std::string& path, const std::string& scope)
	{
		if (__resources[path].resource && __resources[path].resource->GetType() == "Audio") {
			ReleaseResource(path, scope);
		}
	}


	// =======================================================
	// 指定したオーディオをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseAudio(unsigned int resourceId, const std::string& scope)
	{
		ReleaseAudio("aud_" + std::to_string(resourceId), scope);
	}


	// =======================================================
	// 指定したスコープからすべてのオーディオを解放
	// =======================================================
	void ResourceTool::ReleaseAudio(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			ReleaseAudio(pair.first, scope);
		}
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(const std::string& path, const std::string& scope)
	{
		if (__resources[path].resource) {
			auto it = std::find(__resources[path].scope.begin(), __resources[path].scope.end(), scope);
			if (it != __resources[path].scope.end()) {
				__resources[path].scope.erase(it);
			}

			if (__resources[path].scope.size() == 0) {
				__ReleaseResource(path);
			}
		}
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(unsigned int resourceId, const std::string& scope)
	{
		ReleaseResource(std::to_string(resourceId), scope);
	}


	// =======================================================
	// 指定したリソースをスコープから解放
	// =======================================================
	void ResourceTool::ReleaseResource(Resource* resource, const std::string& scope)
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource == resource && pair.second.resource != nullptr) {
				ReleaseResource(pair.first, scope);
			}
		}
	}


	// =======================================================
	// 指定したスコープからすべてのリソースを解放
	// =======================================================
	void ResourceTool::ReleaseResource(const std::string& scope)
	{
		for (const auto& pair : __resources) {
			ReleaseResource(pair.first, scope);
		}
	}


	// =======================================================
	// すべてのリソースを解放
	// =======================================================
	void ResourceTool::ReleaseAllResource()
	{
		for (const auto& pair : __resources) {
			if (pair.second.resource) {
				__ReleaseResource(pair.first);
			}
		}
		__resources.clear();
	}
}