#ifndef _FUNCTION_DISPATCHER_H_
#define _FUNCTION_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

/**
 * Singleton function dispatcher
 * @tparam Treturntype Function return type
 * @tparam Targs Function arguments
 */
template <typename Treturntype, typename... Targs>
class Dispatcher {
 public:
  /**
   * Get the single instance of the dispatcher
   */
  static Dispatcher* instance() {
    static Dispatcher dispatcher;
    return &dispatcher;
  }

  /**
   * Register a function with the dispatcher
   * @param[in] key Register key
   * @param[in] new_function New function to register with the dispatcher
   */
  void register_function(const std::string& key, std::function<Treturntype(Targs...)> new_function) {
    if (!this->check(key)) {    
      registry[key] = std::move(new_function);
    } else {
      throw std::runtime_error("Duplicate key: " + key +
                               ", already registered. Verify key choice to "
                               "ensure function has not already been added");
    }
  }

  /**
   * Dispatch function with input arguments
   * @param key Key to function in dispatcher
   * @param args Inputs to function
   * @return Return result of evaluating function with input args
   */
  Treturntype dispatch(const std::string& key, Targs... args) {
    if (!this->check(key)) {
      throw std::runtime_error("Invalid key: " + key + ", not found in the function dispatcher");
    } else {
      return registry.at(key)(std::forward<Targs>(args)...);
    }
  }

  /**
   * Check if a function is registered
   * @param[in] key Key to be checked in registry
   * @return status Return if key is in registry or not
   */
  bool check(const std::string& key) const {
    bool status = false;
    for (const auto& keyvalue : registry)
      if (keyvalue.first == key) status = true;
    return status;
  }
  
  /**
   * List registered functions
   * @return dispather_items Return list of items in the registry
   */
  std::vector<std::string> list() const {
    std::vector<std::string> dispatcher_items;
    for (const auto& keyvalue : registry)
      dispatcher_items.push_back(keyvalue.first);
    return dispatcher_items;
  }

 private:
  /**
   * Private constructor
   */
  Dispatcher() = default;

  std::map<std::string, std::function<Treturntype(Targs...)>> registry; /**<
                                                                   Register of
                                                                   functions */
};

/**
 * A helper class to register a function with the dispatcher
 * @tparam Treturntype Function return type
 * @tparam Targs Function arguments
 */
template <typename Treturntype, typename... Targs>
class DispatchRegister {
 public:
  /**
   * Register with a given key
   * @param[in] key Key to item in registry
   */
  explicit DispatchRegister(
      const std::string& key,
      std::function<Treturntype(Targs...)> new_function) {
    // register the class factory function
    Dispatcher<Treturntype, Targs...>::instance()->register_function(
        key, std::move(new_function));
  }
};

#endif  // _FUNCTION_DISPATCHER_H_
