#include "Event.h"
class SomeClass
{
public:
	Event<> someEvent;
	Event<int, SomeClass*> otherEvent;
public:
	void someMethodWhoDispatchEvent()
	{
		someEvent();
	}

    void otherDispatchMethod()
    {
        otherEvent(5, this);
    }
};

class SomeOtherClass
{
public:
    ~SomeOtherClass()
    {
       m_someClass->otherEvent.RemoveHandler(*this, &SomeOtherClass::onSomeOtherEvent);
    }
	void onSomeClassCreated(SomeClass* someClass)
	{
		m_someClass = someClass;
		m_someClass->someEvent.AddHandler(*this, &SomeOtherClass::onSomeEvent);
		m_someClass->otherEvent.AddHandler(*this, &SomeOtherClass::onSomeOtherEvent);
	}
	void onSomeEvent()
	{
		printf("onSomeEvent\n");
		m_someClass->someEvent.RemoveHandler(*this, &SomeOtherClass::onSomeEvent);

	}

	void onSomeOtherEvent(int val, SomeClass* obj)
	{
	    printf("onSomeOtherEvent\n");
	}
private:
	SomeClass* m_someClass;
};

int main()
{
    SomeClass val;
    {
        SomeOtherClass cl;
        cl.onSomeClassCreated(&val);
        val.someMethodWhoDispatchEvent();
        val.someMethodWhoDispatchEvent();
        val.otherDispatchMethod();
    }
    val.someMethodWhoDispatchEvent();
    val.otherDispatchMethod();

    return 0;
}
