#ifndef VIEWERPANECONTROLLER_H
#define VIEWERPANECONTROLLER_H

#include <MFListener.h>
#include <atomic>

class ViewerPaneController : public MF::MFListener
{
public:
  ViewerPaneController(bool useMultipleWorkers);
  virtual void Notify() override;
  virtual std::string const & GetName() const override{return mName;}
  void SetName(std::string const & inName) override{mName=inName;}

  void StartProcessing();
private:
  void ProcessQueueEvents();
private:
  std::atomic<bool> mLoopFlag;
  std::string mName;
  std::atomic<bool> mDataReady;
  bool mUseMultipleWorkers;
};

#endif // VIEWERPANECONTROLLER_H
