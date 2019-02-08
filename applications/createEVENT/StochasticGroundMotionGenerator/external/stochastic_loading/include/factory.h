/* THIS LICENSE ONLY APPLIES TO THIS FILE, FACTORY.H. THE REMAINDER OF THE FILES */
/* IN THIS PROJECT ARE RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE VERSION 3 */

/* # MIT License */

/* Copyright (c) 2018 NHERI SimCenter */
/* Copyright (c) 2017 CB-Geo MPM */

/* Permission is hereby granted, free of charge, to any person obtaining a copy */
/* of this software and associated documentation files (the "Software"), to deal */
/* in the Software without restriction, including without limitation the rights */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the Software is */
/* furnished to do so, subject to the following conditions: */

/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE */
/* SOFTWARE. */


#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * Singleton factory implementation
 * @tparam Tbaseclass Base class
 * @tparam Targs variadic template arguments
 */
template <typename Tbaseclass, typename... Targs>
class Factory {
 public:
  /**
   * Get the single instance of the factory
   */
  static Factory* instance() {
    static Factory factory;
    return &factory;
  }

  /**
   * Register a factory function to create an instance of classname
   * @param[in] key Register key
   * @tparam Tderivedclass Derived class
   */
  template <typename Tderivedclass>
  void register_factory(const std::string& key) {
    registry[key].reset(new Creator<Tderivedclass>);
  }

  /**
   * Create an instance of a registered class
   * @param[in] key Key to item in registry
   * @param[in] args Variadic template arguments
   * @return shared_ptr<Tbaseclass> Shared pointer to a base class
   */
  std::shared_ptr<Tbaseclass> create(const std::string& key, Targs&&... args) {
    if (!this->check(key))
      throw std::runtime_error("Invalid key: " + key +
                               ", not found in the factory register!");
    return registry.at(key)->create(std::forward<Targs>(args)...);
  }

  /**
   * Check if an element is registered
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
   * List registered elements
   * @return factory_items Return list of items in the registry
   */
  std::vector<std::string> list() const {
    std::vector<std::string> factory_items;
    for (const auto& keyvalue : registry)
      factory_items.push_back(keyvalue.first);
    return factory_items;
  }

 private:
  /**
   * Private constructor
   */
  Factory() = default;

  /**
   * A base class creator struct
   */
  struct CreatorBase {
    // A virtual create function
    virtual std::shared_ptr<Tbaseclass> create(Targs&&...) = 0;
  };

  /**
   * Creator class
   * @tparam Tderivedclass Derived class
   */
  template <typename Tderivedclass>
  struct Creator : public CreatorBase {
    /**
     * Create instance of object
     */
    std::shared_ptr<Tbaseclass> create(Targs&&... args) override {
      return std::make_shared<Tderivedclass>(std::forward<Targs>(args)...);
    }
  };

  std::map<std::string, std::shared_ptr<CreatorBase>> registry; /**< Register of
                                                                   factory
                                                                   functions */
};

/**
 * A helper class to register a factory function
 * @tparam Tbaseclass Base class
 * @tparam Tderivedclass Derived class
 * @tparam Targs Variadic template arguments
 */
template <typename Tbaseclass, typename Tderivedclass, typename... Targs>
class Register {
 public:
  /**
   * Register with a given key
   * @param[in] key Key to item in registry
   */
  explicit Register(const std::string& key) {
    // register the class factory function
    Factory<Tbaseclass, Targs...>::instance()
        ->template register_factory<Tderivedclass>(key);
  }
};

#endif  // _FACTORY_H_
