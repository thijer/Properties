#ifndef PROPERTYSTORE_HPP
#define PROPERTYSTORE_HPP
#include "property.hpp"


#if defined(ARDUINO_ARCH_ESP32)
#include "Print.h"
#include "Preferences.h"
#include <vector>
#include <array>


const size_t mem_segment_size = std::max({IntProperty::size, BoolProperty::size, FloatProperty::size});

class PropertyStore
{
    private:
        std::vector<BaseProperty*> properties;
        Preferences memory;
        const char* memory_namespace = "properties";
        bool memory_opened;

        void save_to_memory(BaseProperty* p)
        {
            log_d("[Properties]: Saving %s to memory.", p->get_name());
            if(!memory_opened)
            {
                log_i("[Properties]: Opening memory");
                memory_opened = memory.begin(memory_namespace);
                if(!memory_opened)
                {    
                    log_e("[Properties]: Memory failed to open");
                    return;
                }
            }
            uint8_t mem[mem_segment_size] = {0};
            p->save_to_bytes(mem);
            memory.putBytes(p->get_name(), mem, mem_segment_size);
            p->saved();
        }
        
    public:
        // PropertyStore(std::initializer_list<BaseProperty*> props):
        //     properties(props)
        // {}
        PropertyStore(std::initializer_list<BaseProperty*> props):
            memory_opened(false),
            properties(props)
        {}

        bool begin()
        {
            log_i("[Properties]: Opening memory");
            memory_opened = memory.begin(memory_namespace);
            
            if(!memory_opened) 
            {
                log_e("[Properties]: Memory failed to open");
                return false;
            }

            // Loop through all registered properties and see if they already have a 
            // value stored in memory. If not, save its current (default) value.
            for(BaseProperty* p : properties)
            {
                if(memory.isKey(p->get_name()))
                {
                    // Load existing value from memory
                    log_i("[Properties]: Loading %s from memory.", p->get_name());
                    uint8_t mem[mem_segment_size] = {0};
                    size_t len = memory.getBytes(p->get_name(), mem, mem_segment_size);
                    if(len != mem_segment_size)
                    {
                        log_e("[Properties]: Read %i instead of %i bytes.", len, mem_segment_size);
                        return false;
                    }
                    p->set_from_bytes(mem);
                    p->saved();        // Disable save flag when loading from memory.
                }
                else save_to_memory(p);
            }
            log_i("[Properties]: Closing memory");
            memory.end();
            memory_opened = false;
            return true;
        }

        void apply_setting(String key, String value)
        {
            log_i("[Properties]: Applying %s to %s", value.c_str(), key.c_str());
            bool found = false;
            for(BaseProperty* p : properties)
            {
                if(strcmp(p->get_name(), key.c_str()) == 0)
                {
                    found = true;
                    log_i("[Properties]: found.");
                    p->set_from_string(value);
                    log_i("[Properties]: %s updated.", p->get_name());
                }
            }
            if(!found)
            {
                log_i("[Properties] %s not found.", key.c_str());
            }
        }

        void print_to(Print& sink)
        {
            sink.println("[Properties]: printing config.");
            for(BaseProperty* p : properties) p->print_to(sink);
        }

        void save()
        {
            // Loopt through all registered properties and check if they need saving.
            for(BaseProperty* p : properties)
            {
                if(p->is_updated()) save_to_memory(p);
            }
            // Close memory if opened.
            if(memory_opened)
            {
                log_i("[Properties]: Closing memory");
                memory.end();
                memory_opened = false;
            }
        }
};

/* 
class TelemetryStore
{
    private:
        std::vector<BaseProperty*> variables;
    public:
        // PropertyStore(std::initializer_list<BaseProperty*> props):
        //     properties(props)
        // {}
        TelemetryStore(std::initializer_list<BaseProperty*> vars):
            variables(vars)
        {}

        void print_to(Print& sink)
        {
            // PRINT("[Variables]: printing config.");
            for(BaseProperty* v : variables) v->print_to(sink);
        }
};
*/

#elif defined(ARDUINO_ARCH_AVR)
#include <avr/eeprom.h>

const size_t mem_segment_size = 1 + max(max(IntProperty::size, BoolProperty::size), FloatProperty::size);


class MemoryPropertyWrapper
{
    public:
        MemoryPropertyWrapper(BaseProperty* p, size_t mem_address):
            property(p),
            address(mem_address)
        {}

        MemoryPropertyWrapper() {}

        bool begin()
        {
            // Check if there is already a value for this property saved in memory
            uint8_t mem[mem_segment_size] = {0};
            #ifdef WIPE_MEMORY
            eeprom_write_block(mem, (uint8_t*)address, mem_segment_size);
            #else
            eeprom_read_block(mem, (uint8_t*)address, mem_segment_size);
            #endif
            // First byte indicates if memory is already written to before, meaning that 
            // the property's value can be read from there.
            if(mem[0] > 0)
            {
                property->set_from_bytes(&mem[1]);
            }
            return true;
        }

        void save_to_memory()
        {
            uint8_t mem[mem_segment_size] = {0};
            mem[0] = 1;
            property->save_to_bytes(&mem[1]);
            eeprom_write_block(mem, (uint8_t*)address, mem_segment_size);
            property->saved();
        }

        void set_from_string(String& input) { property->set_from_string(input); }
        void print_to(Print& sink)          { property->print_to(sink); }
        bool is_updated()                   { return property->is_updated(); }
        const char* get_name()              { return property->get_name(); }
    private:
        // bool mUpdated;
        BaseProperty* property;
        size_t address;
};  

template<size_t SIZE>
class PropertyStore
{
    private:
        MemoryPropertyWrapper properties[SIZE];
        size_t last_check;
        const size_t save_interval;
        const size_t memory_start;
        bool memory_opened;

    public:
        PropertyStore(BaseProperty* const (&props)[SIZE], size_t memory_start = 0):
            memory_start(memory_start)
        {
            for(size_t i = 0; i < SIZE; i++)
            {
                properties[i] = MemoryPropertyWrapper(props[i], i * mem_segment_size);
            }
        }

        bool begin()
        {
            if(memory_start + SIZE * mem_segment_size >= E2END) return false;
            if(!eeprom_is_ready()) return false;

            for(size_t i = 0; i < SIZE; i++)
            {
                properties[i].begin();
            }
        }

        void apply_setting(String key, String value)
        {
            // PRINT("[Properties]: Applying ", value, " to ", key);
            for(MemoryPropertyWrapper& p : properties)
            {
                if(strcmp(p.get_name(), key.c_str()) == 0)
                {
                    // PRINT("[Properties]: found ", key);
                    p.set_from_string(value);
                    // PRINT("[Properties]: ", p.get_name(), " updated.");
                }
            }
        }

        void print_to(Print& sink)
        {
            sink.println("[Properties]: printing config.");
            for(MemoryPropertyWrapper& p : properties) p.print_to(sink);
        }

        void save()
        {
            for(MemoryPropertyWrapper& p : properties)
            {
                if(p.is_updated()) p.save_to_memory();
            }
        }
};

#else
#error "This architecture is not supported."
#endif



#endif