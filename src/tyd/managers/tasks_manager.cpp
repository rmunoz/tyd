#include "managers/tasks_manager.hpp"

#include <list>
#include <mutex>
#include <algorithm>
#include <stdexcept>

namespace {
  std::list<tyd::models::task> _tasks;

  std::mutex _task_mutex;
}

namespace tyd::managers {

  models::task& add_task(
      const std::string& name,
      const models::effort& estimate,
      const models::task_size& size
      )
  {
    std::lock_guard<std::mutex> lock(_task_mutex);

    models::task_id id = _tasks.size();
    _tasks.emplace_front(++id, name, estimate, size);

    return _tasks.front();
  }

  models::task& find_task(const models::task_id& id)
  {
    std::lock_guard<std::mutex> lock(_task_mutex);

    auto task = std::find_if(
        std::begin(_tasks),
        std::end(_tasks),
        [id](const models::task& elem)
        {
          return elem.id() == id;
        }
      );

    if(task == std::end(_tasks)) throw std::runtime_error("unknown task");

    return *task;
  }

} // namespace tyd::managers
