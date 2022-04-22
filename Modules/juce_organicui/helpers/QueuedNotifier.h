/*
  ==============================================================================

    QueuedNotifier.h
    Created: 11 May 2016 4:29:20pm
    Author:  Martin Hermant

  ==============================================================================
*/
#pragma once

#define ENUM_LIST(...) {__VA_ARGS__}
;
#define DECLARE_ASYNC_EVENT(BaseClass, EventPrefix, NotifierPrefix, Types) \
class  EventPrefix ## Event \
{ \
public: \
	enum Type Types; \
	 EventPrefix ## Event(Type t, BaseClass * item) : type(t), item(item) {} \
	Type type; \
	BaseClass* item; \
}; \
QueuedNotifier<EventPrefix ## Event> NotifierPrefix ## Notifier; \
typedef QueuedNotifier<EventPrefix ## Event>::Listener AsyncListener; \
void addAsync ## EventPrefix ## Listener(AsyncListener* newListener) { NotifierPrefix ## Notifier.addListener(newListener); } \
void addAsyncCoalesced ## EventPrefix ## Listener(AsyncListener* newListener) { NotifierPrefix ## Notifier.addAsyncCoalescedListener(newListener); } \
void removeAsync ## EventPrefix ## Listener(AsyncListener* listener) { NotifierPrefix ## Notifier.removeListener(listener); }
;


template<typename MessageClass,class CriticalSectionToUse = CriticalSection>
class QueuedNotifier:
	public  AsyncUpdater
{
public:

    QueuedNotifier(int _maxSize, bool _dropMessageOnOverflow = true) :
    dropMessageOnOverflow(_dropMessageOnOverflow),
    fifo(_maxSize)
    {
        maxSize = _maxSize;

    }

	bool dropMessageOnOverflow;

    virtual ~QueuedNotifier() {
		listeners.clear();
		lastListeners.clear();
		messageQueue.clear();
		cancelPendingUpdate();
	}



    class Listener{
    public:
        virtual ~Listener(){}
        virtual void newMessage(const MessageClass&) = 0;
    };

    void addMessage( MessageClass * msg,bool forceSendNow = false){
        if(listeners.size()==0 && lastListeners.size()==0){
            delete msg;
            return;
        }
        forceSendNow |= MessageManager::getInstance()->isThisTheMessageThread();
        if(forceSendNow){
            listeners.call(&Listener::newMessage,*msg);
            lastListeners.call(&Listener::newMessage,*msg);
            delete msg;
            return;
        }
        else{

            // add if we are in a decent array size
            
			int start1,size1,start2,size2;
			fifo.prepareToWrite(1, start1, size1, start2, size2);
			
			/*
			if(size2>0){
				if(messageQueue.size()<maxSize){messageQueue.add(msg);}
				else{messageQueue.set(start2,msg);}
			}
			*/
			if (size1 == 0)
			{
				if (dropMessageOnOverflow)
				{
					delete msg;
				}
				else
				{
					while (size1 == 0)
					{
						fifo.prepareToWrite(1, start1, size1, start2, size2);
						Thread::sleep(10);
					}
				}
			}else {
				if (messageQueue.size()<maxSize) { messageQueue.add(msg); }
				else { messageQueue.set(start1, msg); }
			}

			//jassert(size2 == 0);


			fifo.finishedWrite (size1 + size2);
            triggerAsyncUpdate();
        }

    }

    // allow to stack all values or get oly last updated value
    void addListener(Listener* newListener) { listeners.add(newListener); }
    void addAsyncCoalescedListener(Listener* newListener) { lastListeners.add(newListener); }
    void removeListener(Listener* listener) { listeners.remove(listener);lastListeners.remove(listener); }

private:

    void handleAsyncUpdate() override
    {
            int start1,size1,start2,size2;
            fifo.prepareToRead(fifo.getNumReady(), start1, size1, start2, size2);

                for(int i = start1 ;i <start1+ size1 ; ++i){
                    listeners.call(&Listener::newMessage,*messageQueue.getUnchecked(i));
                }

            for(int i = start2 ;i <start2+ size2 ; ++i){
                listeners.call(&Listener::newMessage,*messageQueue.getUnchecked(i));
            }

            if(size2>0)
                lastListeners.call(&Listener::newMessage,*messageQueue.getUnchecked(start2+size2-1));
            else if(size1>0)
                lastListeners.call(&Listener::newMessage,*messageQueue.getUnchecked(start1+size1-1));

            fifo.finishedRead(size1 + size2);
    }



    AbstractFifo fifo;
    int maxSize;
    OwnedArray<MessageClass,CriticalSectionToUse> messageQueue;

    ListenerList<Listener > listeners;
    ListenerList<Listener > lastListeners;
};