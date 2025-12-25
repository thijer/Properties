# Changelog
## [2.2.0] - 25-12-2025
### Added
- Boolean OR and AND operators (`||`, `&&`) to `Property`.

## [2.1.0] - 3-9-2025
### Added
- Access to all categories of `CategoricalProperty` by providing an index to `get()`. 

## [2.0.0] - 1-09-2025
### Added
- `CategoricalProperty`, a property that can be set to a category from a set of categories.
- A general `Property` template class, from which all specific properties are derived from.
- By defining `PROPERTY_WIDE`, `IntegerProperty` and `RealProperty` use `int64_t` and `double` instead of `int32_t` and `float`.

### Changed
- `IntProperty`, `FloatProperty`, and `BoolProperty` are renamed to `IntegerProperty`, `RealProperty`, and `BooleanProperty`.
- All properties are now based on a general `Property` template class. 

## [1.1.0] - 30-08-2025
### Added
- Debug output can now be enabled/disabled by setting `DEBUG_PROPERTY` to a `Print` instance of your choice, for example `Serial`.
- Added a `Preferences` class for AVR that enables the storage of key/value pairs in EEPROM. The `Preferences` class mimics the `Preferences` class from the ESP32 package, but only the methods used by `PropertyStore` are implemented.

### Removed
- `MemoryPropertyWrapper`, which functionality is replaced with the `Preferences` class.
- The `PropertyStore` implementation for AVR. AVR and ESP32 now use the same `PropertyStore`.

## [1.0.0] - 11-05-2025

### Added
- IntProperty, BoolProperty, and FloatProperty (src/property.hpp)
- PropertyStore for AVR and ESP32 architectures (src/propertystore.hpp)

## [0.0.1] - 10-05-2025
### Added
- This changelog
- README
- gitignore