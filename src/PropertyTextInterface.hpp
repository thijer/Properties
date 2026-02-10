#ifndef PROPERTYTEXTINTERFACE_HPP
#define PROPERTYTEXTINTERFACE_HPP
#include "propertystore.hpp"
#include "utility/debug.hpp"

class PropertyTextInterface
{
    public:
        PropertyTextInterface(
            BaseStore& store
        ):
            store(store)
        {}
        ~PropertyTextInterface(){}
        void apply_setting(String key, String value);
        void print_to(Print& sink);
        
    protected:
    
    private:
        BaseStore& store;
};

void PropertyTextInterface::apply_setting(String key, String value)
{
    #ifdef DEBUG_PROPERTY
    PRINT("[Properties]: Applying ", value.c_str(), " to ", key.c_str());
    #endif
    bool found = false;
    for(BaseProperty* p : store)
    {
        if(strcmp(p->get_name(), key.c_str()) == 0)
        {
            found = true;
            p->set_from_string(value);
            #ifdef DEBUG_PROPERTY
            PRINT("[Properties]: ", p->get_name(), " updated.");
            #endif
        }
    }
    if(!found)
    {
        #ifdef DEBUG_PROPERTY
        PRINT("[Properties] ", key.c_str(), " not found.");
        #endif
    }
}

void PropertyTextInterface::print_to(Print& sink)
{
    sink.println("[Properties]: printing config.");
    for(BaseProperty* p : store) p->print_to(sink);
}
#endif