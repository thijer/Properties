#ifndef PROPERTYSTORE_HPP
#define PROPERTYSTORE_HPP
#define DEBUG Serial
#include "utility/debug.hpp"

#include "property.hpp"


#if defined(ARDUINO_ARCH_AVR) && defined(E2END)
#include <avr/eeprom.h>
#include <avr/io.h>

// Keep KEY_NAME_MAX_SIZE equal to NVS_KEY_NAME_MAX_SIZE from nvs.h esp32
#define KEY_NAME_MAX_SIZE 16        // Includes null-terminator.
const size_t mem_segment_size = max(max(IntProperty::size, BoolProperty::size), FloatProperty::size);
const size_t mem_entry_size   = KEY_NAME_MAX_SIZE + mem_segment_size;
const size_t mem_max_entries  = round((E2END + 1) / mem_entry_size);

class Preferences
{
    public:
        Preferences();
        bool begin(const char* mem_namespace = nullptr);
        void end();
        bool isKey(const char* key);
        size_t getBytes(const char* key, void* value, size_t len);
        size_t putBytes(const char* key, const void* value, size_t len);
        bool remove(const char* key);
        // bool wipeMemory();
    private:
        size_t n_entries;
        size_t getEntry(const char* key, uint8_t* buf = nullptr);
};

Preferences::Preferences()
{

}

bool Preferences::begin(const char* mem_namespace)
{
    if(!eeprom_is_ready()) return false;

    // Count entries in memory
    uint8_t block[mem_entry_size] = {0};
    while(n_entries < mem_max_entries)
    {
        #ifdef WIPE_MEMORY
        eeprom_write_block(block, (uint8_t*)(n_entries * mem_entry_size), mem_entry_size);
        #else
        eeprom_read_block(block, (uint8_t*)(n_entries * mem_entry_size), mem_entry_size);
        if(block[0] == 0) break;
        #endif
        n_entries++;
    }
    #ifdef WIPE_MEMORY
    n_entries = 0;
    #endif
    return true;
}

void Preferences::end()
{

}

/* Find the block of memory in eeprom associated with the provided key, and store it in buf. */
size_t Preferences::getEntry(const char* key, uint8_t* buf)
{
    size_t i = 0;
    uint8_t block[mem_entry_size] = {0};
    if(buf == nullptr) buf = block;

    while(i < n_entries)
    {
        eeprom_read_block(buf, (void*)(i * mem_entry_size), mem_entry_size);
        if(strncmp(key, (const char*)buf, strlen(key)) == 0) return i;
        i++;
    }
    return i;
}

bool Preferences::isKey(const char* key)
{
    return getEntry(key) < n_entries;
}

size_t Preferences::putBytes(const char* key, const void* value, size_t len)
{
    if(len != mem_segment_size)
    {
        return 0;
    }
    uint8_t block[mem_entry_size] = {0};
    size_t i = getEntry(key, block);
    if(i < n_entries)
    {
        // Update existing entry
        memcpy(block + KEY_NAME_MAX_SIZE, value, len);
        eeprom_update_block(block + KEY_NAME_MAX_SIZE, (void*)(i * mem_entry_size + KEY_NAME_MAX_SIZE), mem_segment_size);
        return len;
    }
    else if(n_entries < mem_max_entries)
    {
        // Create new entry
        size_t keylength = strlen(key);
        if(keylength >= KEY_NAME_MAX_SIZE) return 0;
        memcpy(block, key, keylength);
        memcpy(block + KEY_NAME_MAX_SIZE, value, len);
        eeprom_write_block(block, (void*)(i * mem_entry_size), mem_entry_size);
        n_entries++;
        return len;
    }
    else
    {
        return 0;
    }
    
}

size_t Preferences::getBytes(const char* key, void* value, size_t len)
{
    if(len != mem_segment_size) return 0;

    uint8_t block[mem_entry_size] = {0};
    size_t i = getEntry(key, block);
    if(i < n_entries)
    {
        memcpy(value, block + KEY_NAME_MAX_SIZE, len);
        return len;
    }
    return 0;
}

bool Preferences::remove(const char* key)
{
    size_t i = getEntry(key);
    if(i < n_entries)
    {
        uint8_t block[mem_entry_size] = {0};
        // Update existing entry
        eeprom_write_block(block, (void*)(i * mem_entry_size), mem_entry_size);
        return true;
    }
    else return false;
}

/* bool Preferences::wipeMemory()
{
    uint8_t block[16] = {0};
    for(size_t i = 0; i < (E2END + 1); i += 16)
    {
        eeprom_write_block(block, (void*)i, 16);
    }
    return true;
} */

#elif defined(ARDUINO_ARCH_ESP32)
#include "Print.h"
#include "Preferences.h"
const size_t mem_segment_size = std::max({IntProperty::size, BoolProperty::size, FloatProperty::size});
#endif

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_AVR)
template<size_t SIZE>
class PropertyStore
{
    private:
        BaseProperty* properties[SIZE];
        Preferences memory;
        const char* memory_namespace = "properties";
        bool memory_opened;

        void save_to_memory(BaseProperty* p)
        {
            PRINT("[Properties]: Saving ", p->get_name(), " to memory.");
            if(!memory_opened)
            {
                PRINT("[Properties]: Opening memory");
                memory_opened = memory.begin(memory_namespace);
                if(!memory_opened)
                {    
                    PRINT("[Properties]: Memory failed to open");
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
        PropertyStore(BaseProperty* const (&props)[SIZE]):
            memory_opened(false)
        {
            for(uint32_t i = 0; i < SIZE; i++)
            {
                properties[i] = props[i];
            }
        }

        bool begin()
        {
            PRINT("[Properties]: Opening memory");
            memory_opened = memory.begin(memory_namespace);
            
            if(!memory_opened) 
            {
                PRINT("[Properties]: Memory failed to open");
                return false;
            }

            // Loop through all registered properties and see if they already have a 
            // value stored in memory. If not, save its current (default) value.
            for(BaseProperty* p : properties)
            {
                if(memory.isKey(p->get_name()))
                {
                    // Load existing value from memory
                    PRINT("[Properties]: Loading ", p->get_name(), " from memory.");
                    uint8_t mem[mem_segment_size] = {0};
                    size_t len = memory.getBytes(p->get_name(), mem, mem_segment_size);
                    if(len != mem_segment_size)
                    {
                        PRINT("[Properties]: Read ", len," instead of ", mem_segment_size, " bytes.");
                        return false;
                    }
                    p->set_from_bytes(mem);
                    p->saved();        // Disable save flag when loading from memory.
                }
                else save_to_memory(p);
            }
            PRINT("[Properties]: Closing memory");
            memory.end();
            memory_opened = false;
            return true;
        }

        void apply_setting(String key, String value)
        {
            PRINT("[Properties]: Applying ", value.c_str(), " to ", key.c_str());
            bool found = false;
            for(BaseProperty* p : properties)
            {
                if(strcmp(p->get_name(), key.c_str()) == 0)
                {
                    found = true;
                    p->set_from_string(value);
                    PRINT("[Properties]: ", p->get_name(), " updated.");
                }
            }
            if(!found)
            {
                PRINT("[Properties] ", key.c_str(), " not found.");
            }
        }

        void print_to(Print& sink)
        {
            sink.println("[Properties]: printing config.");
            for(BaseProperty* p : properties) p->print_to(sink);
        }

        void save()
        {
            // Loop through all registered properties and check if they need saving.
            for(BaseProperty* p : properties)
            {
                if(p->is_updated()) save_to_memory(p);
            }
            // Close memory if opened.
            if(memory_opened)
            {
                PRINT("[Properties]: Closing memory");
                memory.end();
                memory_opened = false;
            }
        }
};

template<size_t SIZE>
class TelemetryStore
{
    private:
        BaseProperty* variables[SIZE];
    public:
        TelemetryStore(BaseProperty* const (&vars)[SIZE])
        {
            for(uint32_t i = 0; i < SIZE; i++)
            {
                variables[i] = vars[i];
            }
        }

        void print_to(Print& sink)
        {
            PRINT("[Variables]: printing config.");
            for(BaseProperty* v : variables) v->print_to(sink);
        }
};

#else
#error "This architecture is not supported."
#endif

#endif