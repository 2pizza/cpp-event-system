namespace detail
{
	inline std::size_t hash_combine(std::size_t seed, std::size_t value) noexcept
	{
		// где-то взято на просторах
		return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
	}

	template<typename... Args>
	inline std::size_t GenerateID(void (*func)(Args...))
	{
		return std::hash<void*>()(reinterpret_cast<void*>(func));
	}

	template<typename Obj, typename Meth>
	inline std::size_t GenerateID(Obj* obj, Meth method)
	{
		constexpr std::size_t N = sizeof(Meth);
		constexpr std::size_t W = sizeof(std::size_t);
		constexpr std::size_t CHUNKS = (N + W - 1) / W;

		std::size_t pieces[CHUNKS]{};
		std::memcpy(pieces, &method, N);

		std::size_t h = std::hash<std::type_index>{}(typeid(Obj));
		for (std::size_t v : pieces)
			h = hash_combine(h, std::hash<std::size_t>{}(v));

		h = hash_combine(h, std::hash<void*>{}(static_cast<void*>(obj)));
		return h;
	}
}
