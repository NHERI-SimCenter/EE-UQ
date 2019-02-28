#include <iostream>
#include <stdexcept>
#include <vector>

/**< Add value to key */
template <typename Tparam>
bool utilities::JsonObject::add_value(const std::string& key,
                                      const Tparam& value) {
  bool status = true;
    
  try {
    if (json_object_.find(key) != json_object_.end()) {
      status = false;      
      throw std::runtime_error(
          "\nWARNING: In utilities::JsonObject::add_value: Input already "
          "exists, so no value was added!\n");
    } else {
      json_object_.emplace(key, value);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what();
    status = false;
    throw;
  }

  return status;
};

/**< Template specialization for case when input value in JsonObject */
template <> inline
bool utilities::JsonObject::add_value(const std::string& key,
                                      const utilities::JsonObject& value) {
  bool status = true;
  
  try {
    add_value<nlohmann::json>(key, value.get_library_json());
  } catch (const std::exception& e) {
    std::cerr << e.what();
    status = false;
    throw;    
  }

  return status;
}

/**< Template specialization for case when input value is vector of JsonObjects */
template<> inline
bool utilities::JsonObject::add_value(const std::string& key, const std::vector<utilities::JsonObject>& value) {
  bool status = true;

  try {
    // Create vector nlohmann::json type
    std::vector<nlohmann::json> value_vector(value.size());   
    for (unsigned int i = 0; i < value.size(); ++i) {
      value_vector[i] = value[i].get_library_json();
    }    
    add_value<std::vector<nlohmann::json>>(key, value_vector);
  } catch (const std::exception& e) {
    std::cerr << e.what();
    status = false;
    throw;    
  }

  return status;  
}

/**< Get value at input key */
template <typename Tparam>
Tparam utilities::JsonObject::get_value(const std::string& key) const {

  auto key_location = json_object_.find(key);
  if (key_location != json_object_.end()) {
      return key_location.value();
  } else {
    throw std::runtime_error(
        "\nWARNING: In utilities::JsonObject::get_value: Input key not found, "
        "so make sure that exception is properly handled!\n");
  } 
};

/**< Template specialization for case when return value is JsonObject */
template <> inline
utilities::JsonObject utilities::JsonObject::get_value(
    const std::string& key) const {

  auto key_location = json_object_.find(key);
  if (json_object_.find(key) != json_object_.end()) {
    return JsonObject(key_location.value());
  } else {
    throw std::runtime_error(
        "\nWARNING: In utilities::JsonObject::get_value: Input key not found, "
        "so make sure that exception is properly handled!\n");
  } 
}
