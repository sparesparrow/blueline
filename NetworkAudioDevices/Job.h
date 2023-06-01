#ifndef JOB_H
#define JOB_H

class IJob
{
public:
    virtual ~IJob() = default;
    virtual void abort() = 0;
    virtual void execute() = 0;
    virtual bool isComplete() = 0;
    virtual void setComplete(const JobStatusEnum outcome) = 0;
    virtual void setBlocked(bool blocked) = 0;
    virtual bool isBlocked() const = 0;
};

class IJobWorker
{
public:

    virtual ~IJobWorker() = default;
    virtual void addJob(std::shared_ptr<IJob> job) = 0;
};
class Job : public IJob
{
public:
    virtual ~Job() = default;
    bool isComplete() override { return complete; }
    const JobStatus& getStatus() override { return status; }
    void abort() override { abortRequested = true; }
    void setComplete(const JobStatusEnum outcome) override
    {
        status.status.value = outcome;
        status.progress = 1;
        complete = true;
    };
    std::string detail() override;
    void setBlocked(bool blocked) override { executionBlocked = blocked; }
    bool isBlocked() const override { return executionBlocked; }
    void execute() override
    {
        switch (status.status.value)
        {
        case JobStatusEnum::notStarted:
        {
            if (abortRequested)
            {
                setComplete(JobStatusEnum::aborted);
                MsgLogInfo::log(MsgLogId::swInfo, "Job::execute(): Job aborted from notStarted state");
            }
            return;
        }
        case JobStatusEnum::inProgress:
        {
            if (abortRequested)
            {
                setComplete(JobStatusEnum::aborted);
                MsgLogInfo::log(MsgLogId::swInfo, "Job::execute(): Job aborted from inProgress state");
            }
            return;
        }
        }
    }
protected:
    bool complete = false;
    bool abortRequested = false;
    bool executionBlocked = false;
    JobStatus status;
    JobType type = JobType::genericJob;
};

class JobWorker : public IJobWorker
{
public:
    JobWorker()
    {
        initialized = true;
    }
    ~JobWorker()
    {
        for (auto& job : execJobs)
        {
            job.get()->abort();
            job.get()->execute();
        }
        newJobs.empty();
    }

    AddResult addJob(std::shared_ptr<IJob> job) override
    {
        newJobs.push_back(job);
        return AddResult::ok;
    }
    bool execute()
    {
        while (!newJobs.empty())
        {
            execJobs.push_back(newJobs.front());
            newJobs.pop_front();
        }
        for (auto job : execJobs)
        {
            job->execute();
        }
        execJobs.remove_if([](const std::shared_ptr<IJob>& job) { return job->isComplete(); });
        return true;
    }

protected:
    LockFreeQueue<std::shared_ptr<IJob>> newJobs;
    std::list<std::shared_ptr<IJob>> execJobs;
}
class JobMsgQueProc
{
public:
    JobMsgQueProc(IRequestReader& reqRdr);
    ~JobMsgQueProc() override
    {
        for (auto& job : jobs)
        {
            if (!job->isComplete())
            {
                job->setComplete(JobStatusEnum::aborted);
            }

        }
    }
    bool execute() override;
    MsgQue& getQue();
protected:
    virtual std::shared_ptr<MsgJob> processMsg(std::shared_ptr<IMsg> msg) = 0;
private:
    std::list<std::shared_ptr<MsgJob>> jobs;
    IRequestReader& reqRdr;
    MsgQue msgs;

};

JobMsgQueProc::JobMsgQueProc(IRequestReader& reqRdr)
    : reqRdr(reqRdr)

{
    initialized = true;
}

bool JobMsgQueProc::execute()
{
    while (!msgs.empty())
    {
        std::shared_ptr<IMsg> msg = msgs.get();
        if (msg == nullptr)
        {
            continue;
        }
        if (!reqRdr.read(msg->data))
        {
            MsgLogInfo::log(MsgLogId::swMessageMalformed, "JobMsgQueProcessor::reqRdr.read(msg->data)");
            msg->resp->send(reqRdr.writeMalformedMessage());
            continue;
        }
        auto newJob = processMsg(msg);
        if (newJob != nullptr && !newJob->hasResponded())
        {
            jobs.push_back(newJob);
            newJob->sendInitial();
        }
    }
    for (auto job : jobs)
    {
        job->processMsg();
        if (job->isComplete())
        {
            job->sendResponse();
        }
        else if (job->getSession().terminated)
        {
            job->abort();
            job->execute();
            job->sendResponse();
        }
    }
    jobs.remove_if([](const std::shared_ptr<MsgJob>& job) { return job->hasResponded(); });
    return true;
}

MsgQue& JobMsgQueProc::getQue()
{
    return msgs;
}


#endif // JOB_H
