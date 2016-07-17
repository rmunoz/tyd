#include <managers/tasks_manager.hpp>

#include <iostream>
#include <sstream>
#include <queue>
#include <functional>
#include <algorithm>
#include <map>

//#include "backward.hpp"
#include "rang/include/rang.hpp"

using namespace tyd;

using namespace std::chrono_literals;

using arguments = std::queue<std::string>;
using callback = std::function<bool(arguments&)>;

static bool keep_running { true };
std::experimental::optional<tyd::models::task_record> current_task_record {};

auto help = [](arguments& args) -> bool
{
  #pragma unused(args)

  std::cout << rang::style::bold << rang::style::underline
      << "Available commands\n" << rang::style::reset
      << rang::style::bold << "\thelp:"
      << rang::style::reset << " print this help\n"
      << rang::style::bold << "\tquit:"
      << rang::style::reset <<" quits\n"
      << rang::style::bold << rang::style::underline
      << "Tasks management\n" << rang::style::reset
      << rang::style::bold << "\tadd"
      << rang::style::dim << rang::fg::yellow << " name estimate size"
      << rang::style::reset << ": adds a task.\n"
      << rang::style::bold << "\t  name: "
      << rang::style::reset << "a string without spaces.\n"
      << rang::style::bold << "\t  estimate: "
      << rang::style::reset << "the initial estimate in hours.\n"
      << rang::style::bold << "\t  size: "
      << rang::style::reset << "the task size: XS, S, M, L, XL or XXL.\n"
      << rang::style::bold << "\tprint"
      << rang::style::dim << rang::fg::yellow << " id"
      << rang::style::reset << ": prints information of a task by its id.\n"
      << rang::style::bold << rang::style::underline
      << "Task recording\n" << rang::style::reset
      << rang::style::bold << "\tstart"
      << rang::style::dim << rang::fg::yellow << " id [timestamp]"
      << rang::style::reset << ": starts a new task record. It will close the "
                                "current task record if it is already open\n"
      << rang::style::bold << "\t  id:"
      << rang::style::reset << " the id of the associated task.\n"
      << rang::style::bold << "\t  timestamp:"
      << rang::style::reset << " YYYY-MM-DDTHH:MM:SS to set as start, if it is "
                                "not defined, current timestamp will be used.\n"
      << rang::style::bold << "\tend"
      << rang::style::dim << rang::fg::yellow << " restimate [timestamp]"
      << rang::style::reset << ": ends the current opened task record.\n"
      << rang::style::bold << "\t  id:"
      << rang::style::reset << " the id of the associated task.\n"
      << rang::style::bold << "\t  restimate:"
      << rang::style::reset <<" the remaining estimate in hours.\n"
      << rang::style::bold << "\t  timestamp:"
      << rang::style::reset << " YYYY-MM-DDTHH:MM:SS to set as end, if it is "
                                "not defined, current timestamp will be used.";
  std::cout << "\n\n";

  return true;
};

auto quit = [](arguments& args) -> bool
{
  #pragma unused(args)

  std::cout << rang::fg::blue << "Bye...\n" << rang::style::reset;
  keep_running = false;

  return true;
};

auto add = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto name = args.front();
    args.pop();

    auto estimate = std::chrono::hours(std::atoi(args.front().c_str()));
    args.pop();

    auto size = tyd::models::to_task_size(args.front());
    args.pop();

    tyd::managers::add_task(name, estimate, size);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cerr << ex.what();
  }

  return retvalue;
};

auto print_task = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));

    auto invested = tyd::models::get_invested_effort(task);
    auto remaining = task.estimate() - invested;
    auto deviation = invested - task.estimate();

    using namespace std::chrono;

    std::cout << "id: " << task.id() << "\n";
    std::cout << "name: " << task.name() << "\n";
    std::cout << "size: " << tyd::models::to_string(task.size()) << "\n";
    std::cout << "original estimate: "
      << duration_cast<hours>(task.estimate()).count() << " hours\n";
    std::cout << "invested effort: "
      << duration_cast<hours>(invested).count() << " hours\n";
    std::cout << "remaining effort: "
      << duration_cast<hours>(remaining).count() << " hours\n";
    std::cout << "deviation: "
      << duration_cast<hours>(deviation).count() << " hours\n";

    std::cout << "\ntask records: " << task.records().size() << "\n";

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << rang::fg::green << args.front()
      << rang::style::reset << " does not exist.\n";
  }

  return retvalue;
};

auto start_record = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));
    args.pop();

    auto time = std::chrono::system_clock::now();
    if(not args.empty())
    {
      std::cout << rang::fg::yellow << "Use of timestamp not implemented yet,"
        << " using now timestamp.\n" << rang::style::reset;
    }

    task.open_record(time);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << rang::fg::green << args.front()
      << rang::style::reset << " does not exist.\n";
  }

  return retvalue;
};

auto end_record = [](arguments& args) -> bool
{
  bool retvalue = false;

  try
  {
    auto& task = tyd::managers::find_task(std::atoi(args.front().c_str()));
    args.pop();

    auto reestimate = std::chrono::hours(std::atoi(args.front().c_str()));
    args.pop();

    auto time = std::chrono::system_clock::now();
    if(not args.empty())
    {
      std::cout << rang::fg::yellow << "Use of timestamp not implemented yet,"
        << " using now timestamp.\n" << rang::style::reset;
    }

    task.close_record(time, reestimate);

    retvalue = true;
  }
  catch(const std::exception& ex)
  {
    std::cout << "The task with id: " << rang::fg::green << args.front()
      << rang::style::reset << " does not exist.\n";
  }

  return retvalue;
};

static std::map<std::string, callback> dispatcher = {
  { std::string("help"), help },
  { std::string("quit"), quit },
  { std::string("add"), add },
  { std::string("print"), print_task},
  { std::string("start"), start_record },
  { std::string("end"), end_record },
};

bool dispatch_command(
    const std::string& command,
    std::queue<std::string>& args
  )
{
  bool retvalue = true;

  try
  {
    retvalue = dispatcher.at(command)(args);
  }
  catch(const std::exception& ex)
  {
    std::cout << rang::fg::red << "Unknown command\n" << rang::style::reset;
  }

  return retvalue;
};

int main()
{
  while(keep_running)
  {
    std::cout << rang::fg::blue << "[tyd]" << rang::style::reset << "# ";

    std::string input;
    std::getline(std::cin, input);

    std::istringstream tokens(input);
    std::queue<std::string> args;

    for(std::string each; std::getline(tokens, each, ' '); args.push(each));

    auto command = args.front();
    args.pop();

    if(not dispatch_command(command, args))
    {
      std::cout << rang::fg::red << "Invalid arguments\n" << rang::style::reset;
    }
  }
}
