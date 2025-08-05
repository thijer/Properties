#ifndef PROPERTY_HPP
#define PROPERTY_HPP
#include "Print.h"

class BaseProperty
{
    public:
        // Used by external sources such as through serial.
        virtual void set_from_string(String& input) = 0;

        // Used to set the value loaded from non-volatile memory.
        virtual void set_from_bytes(uint8_t* ptr) = 0;

        // Used to save the current value to non-volatile memory.
        virtual void save_to_bytes(uint8_t* ptr) = 0;

        // Print the value to the designated Print interface (such as Serial).
        virtual void print_to(Print& sink) = 0;

        // Return the name of this property.
        const char* get_name() { return name; }

        // Indicates wether the property is updated and needs to be saved to memory.
        bool is_updated() { return updated; }

        // Disables the updated flag.
        void saved() { updated = false; }
        
    protected:
        BaseProperty(const char* name): 
            name(name)
        {}
        bool updated = false;
        const char* name;
    private:
};

class IntProperty: public BaseProperty
{
    public:
        IntProperty(const char* name, int32_t default_value = 0): 
            BaseProperty(name),
            value(default_value)
        {}
        
        void set_from_string(String& input)
        {
            int32_t val = input.toInt();
            // TODO checks
            value = val;
            updated = true;
        }

        void set_from_bytes(uint8_t* arr)
        {
            int32_t v;
            memcpy(&v, arr, IntProperty::size);
            value = v;
            // updated = true;
        }

        void set(int32_t v)
        {
            value = v;
            updated = true;
        }
        
        void save_to_bytes(uint8_t* arr)
        {
            memcpy(arr, &value, IntProperty::size);    
        }

        void print_to(Print& sink)
        {
            sink.print(name);
            sink.print(":");
            sink.println(value);
        }
        
        int32_t get()                   { return value; }
        void print(Print& sink)         { sink.print(name); sink.print(":"); sink.println(value); }
        bool operator<(int32_t rhs)     { return value < rhs; }
        bool operator>(int32_t rhs)     { return value > rhs; }
        bool operator<=(int32_t rhs)    { return value <= rhs; }
        bool operator>=(int32_t rhs)    { return value >= rhs; }
        bool operator==(int32_t rhs)    { return value == rhs; }
        bool operator!=(int32_t rhs)    { return value != rhs; }
        
        static const size_t size;
    private:
        int32_t value;
        // std::vector<IntSubscriber*> mSubscribers;

};

class BoolProperty: public BaseProperty
{
    public:
        BoolProperty(const char* name, bool default_value = false): 
            BaseProperty(name),
            value(default_value)
        {}

        void set_from_string(String& input)
        {
            int32_t val = input.toInt();
            // checks
            value = bool(constrain(val, 0, 1));
            updated = true;
        }
        void set_from_bytes(uint8_t* arr)
        {
            bool v;
            memcpy(&v, arr, BoolProperty::size);
            value = v;
            updated = true;
        }
        void set(bool v)
        {
            value = v;
            updated = true;
        }

        void save_to_bytes(uint8_t* arr)
        {
            memcpy(arr, &value, BoolProperty::size);    
        }

        void print_to(Print& sink)
        {
            sink.print(name);
            sink.print(":");
            sink.println(value);
        }
        
        bool get() { return value; }
        void print(Print& sink) { sink.print(name); sink.print(":"); sink.println(value); }

        bool operator==(bool rhs) { return value == rhs; }
        bool operator!=(bool rhs) { return value != rhs; }
        explicit operator bool() const { return value; }
        
        static const size_t size;
    private:
        bool value;

};

class FloatProperty: public BaseProperty
{
    public:
        FloatProperty(const char* name, double default_value = false): 
            BaseProperty(name),
            value(default_value)
        {}

        void set_from_string(String& input)
        {
            float val = input.toFloat();
            // checks
            value = double(val);
            // PRINT("[Properties] ", name, " updated with ", input, " to ", value);
            updated = true;
        }
        void set_from_bytes(uint8_t* arr)
        {
            double v;
            memcpy(&v, arr, FloatProperty::size);
            value = v;
            updated = true;
        }
        void set(double v)
        {
            value = v;
            updated = true;
        }
        void save_to_bytes(uint8_t* arr)
        {
            memcpy(arr, &value, FloatProperty::size);    
        }
        void print_to(Print& sink)
        {
            sink.print(name);
            sink.print(":");
            sink.println(value);
        }
        
        double get() { return value; }
        bool operator<(double rhs) { return value < rhs; }
        bool operator>(double rhs) { return value > rhs; }
        bool operator<=(double rhs) { return value <= rhs; }
        bool operator>=(double rhs) { return value >= rhs; }
        bool operator==(double rhs) { return value == rhs; }
        bool operator!=(double rhs) { return value != rhs; }
        
        static const size_t size;
    
    private:
        double value;

};

const size_t IntProperty::size = sizeof(int32_t);
const size_t BoolProperty::size = sizeof(bool);
const size_t FloatProperty::size = sizeof(double);


#endif