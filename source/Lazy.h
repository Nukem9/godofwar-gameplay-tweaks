#pragma once

#include <functional>

template<typename T>
class Lazy
{
private:
	std::function<T()> m_Initializer;
	std::function<void(T&)> m_Deinitializer;

public:
	Lazy(std::function<T()> Initializer, std::function<void(T&)> Deinitializer = nullptr) : m_Initializer(Initializer), m_Deinitializer(Deinitializer)
	{
	}

	Lazy(const Lazy&) = delete;

	~Lazy()
	{
		if (m_Deinitializer)
			m_Deinitializer(*get());
	}

	Lazy& operator=(const Lazy&) = delete;

	inline T *get() const
	{
		return InternalGet();
	}

	inline T& operator*() const { return *get(); }
	inline T *operator->() const { return get(); }
	inline T *operator&() const = delete;
	inline operator T&() const { return *get(); }

	inline auto& operator[](size_t i) const
	{
		// Need to investigate the overhead on this. Static constructors could be a better alternative.
		return (*get())[i];
	}

private:
	__declspec(noinline) auto InternalGet() const
	{
		static auto value { m_Initializer() };
		return std::addressof(value);
	}
};