Простая система событий

Event<> evt; - объявляем эвент.

void onEvt()
{
}

class EventListener
{
  EventListener()
  {
      evt.AddHandler(*this, &EventListener::OnEvent);
  }
  ~EventListener()
  {
      evt.RemoveHandler(*this, &EventListener::OnEvent);
  }
  void OnEvent()
  {
  }
};

int main()
{
  EventListener listenerObj; //Объект подписывается на событие
  evt.AddHandler(&onEvent); // Подписываем свободную функцию

  //evt.AddHandler([]() {}); //Ошибка компиляции - лямбду нельзя использовать

  evt(); //Отправим событие
  
  evt.RemoveHandler(&onEvent); //Отписываем свободную функци
}
