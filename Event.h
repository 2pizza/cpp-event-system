#include <functional> 
#include <utility>
#include <cstring>
#include <memory>
#include <algorithm>

#include "details.inl"

template<typename ...Args>
class EventHandler
{
public:
	virtual void call(Args&&... args) = 0;

	const size_t& GetHandlerID() const { return m_handlerID; }

	void Invalidate() { m_valid = false; }
	bool IsValid() const { return m_valid; }

protected:
	size_t m_handlerID;
private:
	bool m_valid = true;
};

template<typename Object, typename ...Args>
class MethodEventHandler final : public EventHandler<Args...>
{
public:
	using MethodType = void(Object::*)(Args...);
public:
	MethodEventHandler(Object& object, MethodType method) : m_object(object), m_method(method)
	{
		this->m_handlerID = detail::GenerateID(&object, method);
	}

	void call(Args&&... args) override { (m_object.*m_method)(std::forward<Args>(args)...); }

private:
	Object& m_object;
	MethodType m_method;
};

template<typename ...Args>
class FunctionEventHandler : public EventHandler<Args...>
{
public:
	using FunctionType = void(*)(Args...);
public:
	FunctionEventHandler(FunctionType function) : m_function(function)
	{
		this->m_handlerID = detail::GenerateID(function);
	}

	void call(Args&&... args) override { (*m_function)(std::forward<Args>(args)...); }

private:
	FunctionType m_function;
};

template<typename ...Args>
class Event
{
	using HandlerType = EventHandler<Args...>;

public:
	//Защитимся от копирования и переноса - это недопустимая операция(во всяком случае пока)
	Event() = default;
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;
	Event(Event&&) = delete;
	Event& operator=(Event&&) = delete;

	template<typename Object>
	void AddHandler(Object& object, MethodEventHandler<Object, Args...>::MethodType method)
	{
		if (HasId(detail::GenerateID(&object, method)))
			return;

		(m_dispatching ? m_added_handlers : m_handlers).emplace_back(std::make_unique<MethodEventHandler<Object, Args...>>(object, method));
	}

	void AddHandler(FunctionEventHandler<Args...>::FunctionType function)
	{
		if (HasId(detail::GenerateID(function)))
			return;

		(m_dispatching ? m_added_handlers : m_handlers).emplace_back(std::make_unique<FunctionEventHandler<Args...>>(function));
	}

	template<class F>
	void AddHandler(F) = delete; //Запред для функторов и лямбд

	template<typename Object>
	void RemoveHandler(Object& object, MethodEventHandler<Object, Args...>::MethodType method)
	{
		RemoveById(detail::GenerateID(&object, method));
	}

	void RemoveHandler(FunctionEventHandler<Args...>::FunctionType function)
	{
		RemoveById(detail::GenerateID(function));
	}
	
	template<class F>
	void RemoveHandler(F) = delete; //Симметрия

	void operator()(Args... args)
	{
		m_dispatching = true;
		for (auto& handler : m_handlers)
			if (handler->IsValid())
				handler->call(std::forward<Args>(args)...);
		m_dispatching = false;

		m_handlers.erase(std::remove_if(m_handlers.begin(), m_handlers.end(), [](const auto& handler) { return !handler->IsValid(); }), m_handlers.end());

		for (auto& handler : m_added_handlers)
			m_handlers.push_back(std::move(handler));
		m_added_handlers.clear();
	}

private:
	std::vector<std::unique_ptr<HandlerType>> m_handlers;
	std::vector<std::unique_ptr<HandlerType>> m_added_handlers;

	bool m_dispatching = false;

	bool HasId(std::size_t id) const
	{
		auto pred = [&](const auto& handler) { return handler->GetHandlerID() == id; };
		return std::any_of(m_handlers.begin(), m_handlers.end(), pred) || std::any_of(m_added_handlers.begin(), m_added_handlers.end(), pred);
	}

	void RemoveById(std::size_t id)
	{
		auto pred = [&](const auto& handler) { return handler->GetHandlerID() == id; };

		if (m_dispatching)
		{
			if (auto it = std::find_if(m_handlers.begin(), m_handlers.end(), pred); it != m_handlers.end())
				(*it)->Invalidate();
		}
		else
		{
			m_handlers.erase(std::remove_if(m_handlers.begin(), m_handlers.end(), pred), m_handlers.end());
		}

		m_added_handlers.erase(std::remove_if(m_added_handlers.begin(), m_added_handlers.end(), pred), m_added_handlers.end());
	}
};
