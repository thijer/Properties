# Changelog

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