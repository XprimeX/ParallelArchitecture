#include "ViewerPaneController.h"
#include "MFQueueManager.h"
#include "MFQueue.hpp"

#include "CloseWindowEvent.h"
#include "RenderImageEvent.h"
#include "RenderImageDone.h"

#include <QImage>

#include <thread>
#include <functional>

ViewerPaneController::ViewerPaneController(bool useMultipleWorkers)
:
  mLoopFlag(true)
  , mName("ViewerPaneController")
  , mDataReady(false)
  , mUseMultipleWorkers(useMultipleWorkers)
{
  MF::MFQueueManager::GetInstance().GetQueue("Controller").Register(this);
}

void ViewerPaneController::Notify()
{
  mDataReady.store(true);
}

void ViewerPaneController::ProcessQueueEvents()
{
  auto eventEntity = MF::MFQueueManager::GetInstance().GetQueue("Controller").remove();
  auto* event = eventEntity.second;

  // process the event type:
  if(RenderImageEvent* renderEvent = dynamic_cast<RenderImageEvent*>(event))
  {
    QImage* image = new QImage(renderEvent->GetImagePath());
    MF::MFQueueManager::GetInstance().GetQueue("GUI").addToQueue(this,new RenderImageDone("ImagePane",renderEvent->GetImagePath(),image));
  }
  else if(CloseWindowEvent* closeEvent = dynamic_cast<CloseWindowEvent*>(event))
  {
    mLoopFlag.store(false);
  }
  delete event;
}

void ViewerPaneController::StartProcessing()
{
  while(mLoopFlag.load())
  {
    if(mDataReady.load())
    {
      auto queueSize = MF::MFQueueManager::GetInstance().GetQueue("Controller").GetSize();
      if(queueSize != 0)
      {
        if(mUseMultipleWorkers)
        {
          std::vector<std::thread> threads;
          for(int i=0; i<queueSize; ++i)
          {
            threads.push_back(std::thread(std::bind(&ViewerPaneController::ProcessQueueEvents, this)));
            threads.back().detach();
          }
        }
        else
        {
          ProcessQueueEvents();
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}
