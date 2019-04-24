#ifndef QUEUE_H
#define QUEUE_H

#include "MFListener.h"

#include <mutex>
#include <queue>
#include <string>
#include <tuple>

namespace MF {

  template<typename T>
  class MFQueue
  {
  public:
    using TaskEntity = std::pair<MFListener*, T*>;
    MFQueue(std::string const & inTag)
    :
      mTag(inTag)
      ,mTasks()
    {
    }
    std::string const & GetTag() const {return mTag;}

    void addToQueue(MFListener* listener, T* task)
    {
      std::lock_guard<std::mutex> lock(mMutex);
      mTasks.push(std::make_pair(listener, task));
      for(auto * listener : mListeners)
      {
        if(listener->GetName() == task->GetRecepientName())
          listener->Notify();
      }
    }

    std::size_t GetSize()
    {
      std::lock_guard<std::mutex> lock(mMutex);
      return mTasks.size();
    }

    TaskEntity remove()
    {
      std::lock_guard<std::mutex> lock(mMutex);
      auto entity = mTasks.front();
      mTasks.pop();
      return entity;
    }

    // ToDo: Use a weak_ptr for the listener objects.
    void Register(MF::MFListener * listener)
    {
      mListeners.push_back(listener);
    }

    void Clear()
    {
      std::lock_guard<std::mutex> lock(mMutex);
      while(mTasks.size()!=0)
        mTasks.pop();
    }

  private:
    std::mutex mMutex;
    std::string mTag;
    std::queue<TaskEntity> mTasks;
    std::vector<MF::MFListener*> mListeners;
  };
}


#endif // QUEUE_H
