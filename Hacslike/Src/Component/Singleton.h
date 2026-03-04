#pragma once
template <typename T>
class Singleton {
public:

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	inline static T& GetInstance() {
		static T pInstance;
		return pInstance;
	}

	// 複製代入の削除
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;
	~Singleton() = default;
};

