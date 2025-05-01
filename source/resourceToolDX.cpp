// =======================================================
// spriteToolDX.cpp
// 
// DirectX環境のリソース管理
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/08/06
// =======================================================
#include "resourceToolDX.h"
#include "rendererDX.h"
#include "audioToolDX.h"

namespace MG {
	// =======================================================
	// DirectX環境のテクスチャリソース
	// =======================================================
	TextureDX::TextureDX(ID3D11ShaderResourceView* resourceView) : resourceView(resourceView) {}


	// =======================================================
	// DirectX環境のオーディオリソース
	// =======================================================
	AudioDX::AudioDX(BYTE* soundData, WAVEFORMATEX waveFormatEX, int length, int playLength) :
		soundData(soundData), waveFormatEX(waveFormatEX), length(length), playLength(playLength) {}


	// =======================================================
	// DirectX環境のリソース管理クラス
	// =======================================================
	ResourceToolDX::ResourceToolDX(RendererDX* renderer) : renderer(renderer) {}


	// =======================================================
	// リソース解放
	// =======================================================
	void ResourceToolDX::__ReleaseResource(const string& key)
	{
		string type = __resources[key].resource->GetType();
		if (type == "Texture") {
			TextureDX* texture = (TextureDX*)__resources[key].resource;
			texture->resourceView->Release();
			texture->resourceView = nullptr;
		}
		else if (type == "Audio") {
			AudioDX* audio = (AudioDX*)__resources[key].resource;
			delete[] audio->soundData;
			audio->soundData = nullptr;
		}

		delete __resources[key].resource;
		__resources[key].resource = nullptr;
	}


	// =======================================================
	// テクスチャリソース取得
	// =======================================================
	Texture* ResourceToolDX::LoadTexture(const string& path, const string& scope)
	{
		if (!__resources[path].resource) {
			ID3D11ShaderResourceView* resourceView;
			TexMetadata metadata;
			ScratchImage image;

			std::vector<wchar_t> buffer(path.size() + 1);
			std::mbstowcs(buffer.data(), path.c_str(), path.size() + 1);
			LoadFromWICFile(buffer.data(), WIC_FLAGS_NONE, &metadata, image);

			// 読み込んだ画像データをDirectXへわたしてテクスチャとして管理させる
			CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &resourceView);
			// なんか失敗した場合に警告を出す
			assert(resourceView);
			__resources[path].resource = new TextureDX(resourceView);
		}
		if (__resources[path].resource && __resources[path].resource->GetType() == "Texture") {
			__AddScope(path, scope);
			return (Texture*)__resources[path].resource;
		}
		return nullptr;
	}


	// =======================================================
	// テクスチャリソース取得、リソースID指定バージョン
	// =======================================================
	Texture* ResourceToolDX::LoadTexture(unsigned int resourceId, const string& scope)
	{
		string key = "tex_" + std::to_string(resourceId);
		if (!__resources[key].resource) {
			ID3D11ShaderResourceView* resourceView;
			TexMetadata metadata;
			ScratchImage image;

			HRSRC hrs = FindResourceW(nullptr, MAKEINTRESOURCEW(resourceId), L"TEXTURE");
			if (!hrs) { return nullptr; }
			HANDLE ptr = LoadResource(nullptr, hrs);
			if (!ptr) { return nullptr; }

			DWORD size = SizeofResource(nullptr, hrs);
			LoadFromWICMemory(ptr, size, WIC_FLAGS_NONE, &metadata, image);

			// 読み込んだ画像データをDirectXへわたしてテクスチャとして管理させる
			CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &resourceView);
			// なんか失敗した場合に警告を出す
			assert(resourceView);
			__resources[key].resource = new TextureDX(resourceView);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == "Texture") {
			__AddScope(key, scope);
			return (Texture*)__resources[key].resource;
		}
		return nullptr;
	}


	// =======================================================
	// オーディオデータ読み込み
	// =======================================================
	AudioDX* __LoadAudio(HMMIO hmmio)
	{
		WAVEFORMATEX wfx = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		BYTE* soundData;
		int length;
		int playLength;

		buflen = datachunkinfo.cksize;
		soundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)soundData, buflen);

		length = readlen;
		playLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);

		return new AudioDX(soundData, wfx, length, playLength);
	}


	// =======================================================
	// オーディオリソース取得
	// =======================================================
	Audio* ResourceToolDX::LoadAudio(const std::string& path, const std::string& scope)
	{
		if (!__resources[path].resource) {
			HMMIO hmmio = NULL;
			MMIOINFO mmioinfo = { 0 };
			hmmio = mmioOpen((LPSTR)path.data(), &mmioinfo, MMIO_READ);
			assert(hmmio);
			__resources[path].resource = __LoadAudio(hmmio);
		}
		if (__resources[path].resource && __resources[path].resource->GetType() == "Audio") {
			__AddScope(path, scope);
			return (Audio*)__resources[path].resource;
		}
		return nullptr;
	}


	// =======================================================
	// オーディオリソース取得
	// =======================================================
	Audio* ResourceToolDX::LoadAudio(unsigned int resourceId, const std::string& scope)
	{
		string key = "aud_" + std::to_string(resourceId);
		if (!__resources[key].resource) {
			// リソースを見つける
			HRSRC hrs = FindResourceW(nullptr, MAKEINTRESOURCEW(resourceId), L"AUDIO");
			if (!hrs) { return nullptr; }

			// リソースをロード
			HGLOBAL ptr = LoadResource(nullptr, hrs);
			if (!ptr) { return nullptr; }

			// リソースをロックしてメモリアドレスを取得
			LPVOID pLockedResource = LockResource(ptr);
			DWORD resourceSize = SizeofResource(nullptr, hrs);
			if (!pLockedResource || resourceSize == 0) {
				return nullptr;
			}

			// メモリを MMIO ハンドルとして扱う
			MMIOINFO mmioInfo = {};
			mmioInfo.pchBuffer = static_cast<HPSTR>(pLockedResource);
			mmioInfo.fccIOProc = FOURCC_MEM;
			mmioInfo.cchBuffer = resourceSize;

			HMMIO hmmio = mmioOpen(nullptr, &mmioInfo, MMIO_READ);
			assert(hmmio);

			__resources[key].resource = __LoadAudio(hmmio);
		}
		if (__resources[key].resource && __resources[key].resource->GetType() == "Audio") {
			__AddScope(key, scope);
			return (Audio*)__resources[key].resource;
		}
		return nullptr;
	}
} // namespace MG