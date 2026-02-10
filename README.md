# Properties
This Arduino library provides a structured means to create properties that can be used in programs. These property objects represent key-value pairs that can describe parameters, settings or variables of a program. 

- `RealProperty` stores a value from the set of real numbers, and is stored as a `float` internally.
- `IntegerProperty` stores values from the set of integer numbers, and is stored as `int32_t` internally.
- `BooleanProperty` stores values from the set $\{0, 1\}$ as a `bool`.
- `CategoricalProperty` stores a label from a set of given labels. All labels are c string literals.
 
Properties can be added to a container class called `PropertyStore`. This class inherits from the `BaseStore` class and can be used by other classes or in the main sketch.
A `PropertyStore` can then be used in the `PropertyMemory` and `PropertyTextInterface` classes. `PropertyMemory` facilitates the saving and loading of `Property`s from and to non-volatile memory. `PropertyTextInterface` provides a simple means to set specific properties by supplying a key-value pair as `String`s, and allows the contents of the `PropertyStore` to be printed to a `Print` interface.