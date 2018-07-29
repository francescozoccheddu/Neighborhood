#include <Game/Scene/Loader.hpp>
#include <Game/Scene/Mesh.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <map>
#include <string>
#include <RapidJSON/document.h>

namespace Scene
{

	template<typename T>
	void CopyJSONArray (const rapidjson::Value& _jArr, T*& _pArrOut, int& _cArrOut)
	{
		GAME_ASSERT_MSG (_jArr.IsArray (), "Not a JSON array");
		rapidjson::SizeType cArr{ _jArr.Size () };
		T * pBuf{ new T[cArr] };
		for (rapidjson::SizeType iArr{ 0 }; iArr < cArr; iArr++)
		{
			pBuf[iArr] = _jArr[iArr].Get<T> ();
		}
		_pArrOut = pBuf;
		_cArrOut = static_cast<int>(cArr);
	}

	inline Mesh LoadFromJSONObject (const rapidjson::Value& _jObj)
	{
		GAME_ASSERT_MSG (_jObj.IsObject (), "Not a JSON object");
		float * pVerts;
		unsigned int * pInds;
		int cVerts, cInds;
		CopyJSONArray (_jObj["vertices"], pVerts, cVerts);
		CopyJSONArray (_jObj["indices"], pInds, cInds);
		return Mesh{ pVerts,cVerts,pInds,cInds };
	}

	std::map<std::string, Mesh> LoadFromJSON (const std::string& _json)
	{
		rapidjson::Document jDoc;
		jDoc.Parse (_json.c_str ());
		GAME_ASSERT_MSG (jDoc.IsObject (), "Not a JSON object");
		std::map<std::string, Mesh> map;
		for (auto& jObj : jDoc.GetObject ())
		{
			std::string name{ jObj.name.GetString () };
			Mesh mesh{ LoadFromJSONObject (jObj.value) };
			map.emplace( name,mesh );
		}
		return map;
	}

}