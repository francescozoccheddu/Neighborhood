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
		rapidjson::SizeType cArr { _jArr.Size () };
		T * pBuf { new T[cArr] };
		for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
		{
			pBuf[iArr] = _jArr[iArr].Get<T> ();
		}
		_pArrOut = pBuf;
		_cArrOut = static_cast<int>(cArr);
	}

	inline Mesh LoadFromJSONObject (const rapidjson::Value& _jObj)
	{
		GAME_ASSERT_MSG (_jObj.IsObject (), "Not a JSON object");
		Mesh::Vertex * pVerts;
		unsigned int * pInds;
		int cVerts, cInds;
		{
			const rapidjson::Value & jVerts { _jObj["vertices"] };
			GAME_ASSERT_MSG (jVerts.IsArray (), "Not a JSON array");
			rapidjson::SizeType cArr { jVerts.Size () };
			cVerts = static_cast<int>(cArr);
			pVerts = new Mesh::Vertex[cVerts];
			for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
			{
				Mesh::Vertex & vert { pVerts[iArr] };
				const rapidjson::Value & jVert { jVerts[iArr] };
				GAME_ASSERT_MSG (_jObj.IsObject (), "Not a JSON object");
				{
					const rapidjson::Value & jPos { jVert["position"] };
					GAME_ASSERT_MSG (_jObj.IsObject (), "Not a JSON object");
					vert.position.x = jPos["x"].GetFloat ();
					vert.position.y = jPos["y"].GetFloat ();
					vert.position.z = jPos["z"].GetFloat ();
				}
				{
					const rapidjson::Value & jNorm { jVert["normal"] };
					GAME_ASSERT_MSG (_jObj.IsObject (), "Not a JSON object");
					vert.normal.x = jNorm["x"].GetFloat ();
					vert.normal.y = jNorm["y"].GetFloat ();
					vert.normal.z = jNorm["z"].GetFloat ();
				}
			}
		}
		{
			const rapidjson::Value & jInds { _jObj["indices"] };
			GAME_ASSERT_MSG (jInds.IsArray (), "Not a JSON array");
			rapidjson::SizeType cArr { jInds.Size () };
			cInds = static_cast<int>(cArr);
			pInds = new unsigned int[cInds];
			for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
			{
				pInds[iArr] = jInds[iArr].GetUint ();
			}
		}
		return Mesh { pVerts, cVerts, pInds, cInds };
	}

	std::map<std::string, Mesh> LoadFromJSON (const std::string& _json)
	{
		rapidjson::Document jDoc;
		jDoc.Parse (_json.c_str ());
		GAME_ASSERT_MSG (jDoc.IsObject (), "Not a JSON object");
		std::map<std::string, Mesh> map;
		for (auto& jObj : jDoc.GetObject ())
		{
			std::string name { jObj.name.GetString () };
			Mesh mesh { LoadFromJSONObject (jObj.value) };
			map.emplace (name, mesh);
		}
		return map;
	}

}