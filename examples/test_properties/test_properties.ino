// Uncomment to wipe the non-volatile memory and initialize the properties with their default values.
// #define WIPE_MEMORY

// Uncomment to enable printing debug information to a Print interface.
// #define DEBUG_PROPERTY Serial

#include "property.hpp"
#include "propertystore.hpp"
#include "PropertyMemory.hpp"
#include "PropertyTextInterface.hpp"

// Instantiate properties with their name and default/initial value.
RealProperty           test_real("test_real", 20.1);
IntegerProperty        test_int("test_int", 1200);
BooleanProperty        test_bool("test_bool", true);
CategoricalProperty<4> test_cats_0("test_cats_0", {"cat0", "cat1", "cat2", "cat3"});
CategoricalProperty<4> test_cats_1("test_cats_1", {"cat0", "cat1", "cat2", "cat3"});

// Add the properties to the PropertyStore. The PropertyStore presents a single interface 
// from which all properties can be accessed, and facilitates the saving and loading of 
// properties to non-volatile memory. 
PropertyStore<4> properties({&test_real, &test_int, &test_bool, &test_cats_0});
PropertyStore<4> tel({&test_real, &test_int, &test_bool, &test_cats_0});

// Allow the properties to be saved and read from non-volatile memory
PropertyMemory memory(properties);
// Allow to print properties to and set properties from serial.
PropertyTextInterface interface(properties);

String buffer;

void setup()
{
    buffer.reserve(51);
    Serial.begin(115200);
    delay(5000);
    
    bool success = memory.begin();
    Serial.print("Initializing properties: ");
    Serial.println(success ? "successful" : "failed");
}

void loop()
{
    serial_input();
}

void serial_input()
{
    while(Serial.available() > 0)
    {
        char c = Serial.read();
        if(c == '\r' || c == '\n')
        {
            Serial.println("[Serial]: processing line.");
            int i = buffer.indexOf('=');
            if(i != -1)
            {
                // `key=value`
                // Set the property with the name `key` to the value of `value`.
                String key = buffer.substring(0, i);
                String value = buffer.substring(i + 1);
                interface.apply_setting(key, value);
            }
            else if(buffer == "print")
            {
                // Print properties to serial.
                interface.print_to(Serial);
            }
            else if(buffer == "save")
            {
                // Save properties to memory.
                memory.save();
            }
            buffer = "";
        }
        else
        {
            buffer += c;
        }
    }
}

