#pragma once

#include <Game/DirectXMath.hpp>
#include <string>
#include <vector>

struct Scene
{

private:

	struct Light
	{

		virtual ~Light () = default;

		DirectX::XMFLOAT3 color;
		bool bCastShadows;

	protected:

		Light () = default;

	};

public:

	class Transform
	{

	public:

		DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 rotation { 1.0f, 0.0f, 0.0f, 0.0f };

		void Update ();

		DirectX::XMMATRIX Get () const;

	private:

		DirectX::XMFLOAT4X4 m_Transform;

	};

	struct PointLight : public Light
	{
		DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
		float radius { 0.0f };
	};

	struct DirectionalLight : public Light
	{
		DirectX::XMFLOAT3 direction { 0.0f, -1.0f, 0.0f };
	};

	struct ConeLight : public Light
	{
		DirectX::XMFLOAT3 position { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 target { 0.0f, 0.0f, 0.0f };
		float angle { 0.0f };
	};

	struct Material
	{
		std::string name;
	};

	struct Drawable
	{
		std::string mesh;
		Transform transform;
		Material material;
	};

	class AbstractView
	{

	public:

		DirectX::XMFLOAT3 unrotatedUp { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };

		DirectX::XMMATRIX Get () const;

		virtual void Update () = 0;

		virtual ~AbstractView () = default;

	protected:

		DirectX::XMFLOAT4X4 m_mTransform;

	};

	class ViewWithTarget : public AbstractView
	{

	public:

		DirectX::XMFLOAT3 target { 0.0f, 0.0f, 1.0f };

		ViewWithTarget ();

		void Update () override;

	};

	class View : public AbstractView
	{

	public:

		DirectX::XMFLOAT3 unrotatedForward { 0.0f, 0.0f, 1.0f };
		float turn { 0.0f };
		float lookUp { 0.0f };
		float tilt { 0.0f };

		View ();


		void Update () override;

		DirectX::XMVECTOR GetUp () const;

		DirectX::XMVECTOR GetForward () const;

		DirectX::XMVECTOR GetRight () const;

		void Move (float right, float up, float forward, float speed = 1.0f);

		void ClampLookUp (float angle);

	private:

		DirectX::XMFLOAT3 m_Up { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 m_Forward { 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 m_Right { 1.0f, 0.0f, 0.0f };

	};

	class Projection
	{

	public:

		float vFov { 1.5708f };
		float nearZ { 0.01f };
		float farZ { 100.0f };
		float aspectRatio { 1.0f };

		static float CalcAspectRatio (int width, int height);

		static float CalcVFov (float hFov, float aspectRatio);

		Projection ();

		void Update ();

		DirectX::XMMATRIX Get () const;

		virtual ~Projection () = default;

	private:

		DirectX::XMFLOAT4X4 m_mTransform;

	};

	std::vector<Drawable> drawables {};
	std::vector<PointLight> pointLights {};
	std::vector<DirectionalLight> directionalLights {};
	std::vector<ConeLight> coneLight {};
	AbstractView * pView { nullptr };
	Projection projection {};

};

