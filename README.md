Простая система событий

Требует С++17.

Как подлкючить:
1. Скачать Event.h и detail.inl
2. Использовать #include "<папка_куда_положили_файлы_из_п1>/Event.h"

Пример:
```cpp
#include "Event.h"
Event<> evt; // объявляем эвент.

void onEvt() //Функция хендлер
{
}

class EventListener
{
public:
  EventListener()
  {
      evt.AddHandler(*this, &EventListener::OnEvent); //В конструкторе подписались
  }
  ~EventListener()
  {
      evt.RemoveHandler(*this, &EventListener::OnEvent); //В деструкторе отписались
  }

  Event<EventListener&> onGlobalEventHandled;
private:
  void OnEvent() //Функция-член хендлер
  {
      onGlobalEventHandled(*this); //диспатчим событие
  }
};

void OnEventFromListener(EventListener& object)
{
    object.onGlobalEventHandled.RemoveHandler(&OnEventFromListener); //Нам нужно было только разок получить событие и отписываемся
}

int main()
{
  EventListener listenerObj; //Объект подписывается на событие
  listenerObj.onGlobalEventHandled.AddHandler(&OnEventFromListener); //Подписываемся на событие объекта
  evt.AddHandler(&onEvt); // Подписываем свободную функцию
  evt.AddHandler(&onEvt); // Повторная подписка будет проигнорирована

  //evt.AddHandler([]() {}); //Ошибка компиляции - лямбду нельзя использовать

  evt(); //Отправим событие
  
  evt.RemoveHandler(&onEvt); //Отписываем свободную функци
  listenerObj.onGlobalEventHandled.RemoveHandler(&OnEventFromListener); //Отписываемся в конце использования - двойная отписка не страшна
  
  return 0;
}
```
Посмотрите example.cpp
