#include <functional>
#include <vector>
#include <exception>
#include <mutex>
#include <thread>

class TaskGraph
{
public:
    using Id = std::size_t;

    Id Add(std::function<void()> body)
    {
        if(!body) 
        {
            throw std::invalid_argument("empty task");
        }
        nodes_.push_back({std::move(body),{},0});
        return nodes_.size() - 1;
    }

    void Precede(Id from, Id to)
    {
        if(from >= nodes_.size() || to >= nodes_.size())
        {
            throw std::out_of_range("invalid node id");
        }
        auto& edges = nodes_[from].next;
        if(std::find(edges.begin(), edges.end(), to)!= edges.end())
        {
            throw std::invalid_argument("duplicate edge");
        }
        edges.push_back(to);
        ++nodes_[to].indegree;
    }

    void Run(std::size_t workers) const
    {
        if(workers == 0)
        {
            throw std::invalid_argument("workers must be > 0");
        }
        ValidateAcyclic();
        if(nodes_.empty())
        {
            return;
        }
        std::vector<std::size_t> pending;
        pending.reserve(nodes_.size());
        std::vector<Id> ready;
        ready.reserve(nodes_.size());
        for(Id i = 0; i < nodes_.size(); ++i)
        {
            pending.push_back(nodes_[i].indegree);
            if(pending.back() == 0)
            {
                ready.push_back(i);
            }
        }

        std::size_t remaining = nodes_.size();
        std::mutex mutex;
        std::condition_variable cv;
        bool started = false;
        bool stopped = false;
        std::exception_ptr error;

        auto worker = [&]
        {
            while(true)
            {
                Id id;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cv.wait(lock, [&]{
                        return stopped || (started && 
                            (remaining == 0 || !ready.empty()));
                    });
                    if(stopped || remaining == 0)
                    {
                        return ;
                    }
                    id = ready.back();
                    ready.pop_back();
                }
                try
                {
                    nodes_[id].body();
                }
                catch(...)
                {
                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        if(!error)
                        {
                            error = std::current_exception();
                            stopped = true;
                        }
                    }
                    cv.notify_all();
                    return;
                }
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if(stopped)
                    {
                        return;
                    }
                    for(Id next : nodes_[id].next)
                    {
                        if(--pending[next] == 0)
                        {
                            ready.push_back(next);
                        }
                    }
                    --remaining;
                }
                cv.notify_all();//通知多个后续任务唤醒，通知所有现成没有任务
            };
        };

        std::vector<std::thread> threads;
        threads.reserve(workers);
        try
        {
            for(std::size_t i = 0; i< workers; ++i)
            {
                threads.emplace_back(worker);
            }
        }
        catch(...)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                stopped = true;
            }
            cv.notify_all();
            for(auto& t: threads)
            {
                t.join();
            }
            throw;
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            started = true;
        }
        cv.notify_all();
        for(auto& t : threads)
        {
            t.join();
        }
        if(error)
        {
            std::rethrow_exception(error);
        }     
    }

private:
    struct Node
    {
        std::function<void()> body;
        std::vector<Id> next;
        std::size_t indegree;//入度，前置任务的数量
    };
    std::vector<Node> nodes_;

    void ValidateAcyclic() const
    {
        std::vector<std::size_t> degree;
        std::vector<Id> ready;
        for( Id i = 0; i < nodes_.size() ; ++i)
        {
            degree.push_back(nodes_[i].indegree);
            if(nodes_[i].indegree == 0)   
            {
                ready.push_back(i);   
            }
        }
        std::size_t visited = 0;
        while(!ready.empty())
        {
            const Id id = ready.back();
            ready.pop_back();
            ++visited;
            for(Id next : nodes_[id].next)
            {
                if(--degree[next] == 0)
                {
                    ready.push_back(next);
                }
            }
        }
        if(visited != nodes_.size())
        {
            throw std::logic_error("dependency cycle");
        }
    }
};