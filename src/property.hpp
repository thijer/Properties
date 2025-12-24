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

template<typename T>
class Property: public BaseProperty
{
    public:
        Property(const char* name, T default_value): 
            BaseProperty(name),
            value(default_value)
        {}
        Property(const char* name): 
            BaseProperty(name)
        {}
        
        void set_from_string(String& input){}

        void set_from_bytes(uint8_t* arr)
        {
            T v;
            memcpy(&v, arr, size);
            value = v;
            // updated = true;
        }

        void set(T v)
        {
            value = v;
            updated = true;
        }
        
        void save_to_bytes(uint8_t* arr)
        {
            memcpy(arr, &value, size);    
        }

        void print_to(Print& sink)
        {
            sink.print(name);
            sink.print(":");
            sink.println(value);
        }
        
        T get()                   { return value; }
        bool operator<(T rhs)     { return value < rhs; }
        bool operator>(T rhs)     { return value > rhs; }
        bool operator<=(T rhs)    { return value <= rhs; }
        bool operator>=(T rhs)    { return value >= rhs; }
        bool operator==(T rhs)    { return value == rhs; }
        bool operator!=(T rhs)    { return value != rhs; }
        bool operator||(T rhs)    { return value || rhs; }
        bool operator&&(T rhs)    { return value && rhs; }
        
        static const size_t size = sizeof(T);
    protected:
        T value;
};

// Specializations to use the correct String conversion function.
template<>
void Property<float>::set_from_string(String& input)
{
    value = input.toFloat();
    updated = true;
}

template<>
void Property<double>::set_from_string(String& input)
{
    value = double(input.toFloat());
    updated = true;
}

template<>
void Property<int32_t>::set_from_string(String& input)
{
    value = int32_t(input.toInt());
    updated = true;
}

template<>
void Property<int64_t>::set_from_string(String& input)
{
    value = int64_t(input.toInt());
    updated = true;
}

template<>
void Property<bool>::set_from_string(String& input)
{
    value = bool(constrain(input.toInt(), 0, 1));;
    updated = true;
}

// Temporary fix to enable printing wide IntegerProperties to a Printer.
template<>
void Property<int64_t>::print_to(Print& sink)
{
    sink.print(name);
    sink.print(":");
    // Temporary workaround to allow printing of int64_t.
    sink.println(int32_t(value));
}

#ifdef PROPERTY_WIDE
typedef Property<int64_t> IntegerProperty;
typedef Property<double>  RealProperty;
#else
typedef Property<int32_t> IntegerProperty;
typedef Property<float>   RealProperty;
#endif
typedef Property<bool>    BooleanProperty;

template<size_t SIZE>
class CategoricalProperty: public IntegerProperty
{
    public:
        CategoricalProperty(const char* name, const char* const (&cats)[SIZE]): 
            IntegerProperty(name)
        {
            for(uint32_t i = 0; i < SIZE; i++)
            {
                categories[i] = cats[i];
            }
        }
        
        void set_from_string(String& input)
        {
            const char* cat = input.c_str();
            for(uint32_t i = 0; i < SIZE; i++)
            {
                if(strcmp(cat, categories[i]) == 0)
                {
                    IntegerProperty::set(i);
                }
            }
        }

        void print_to(Print& sink)
        {
            sink.print(name);
            sink.print(":");
            sink.println(categories[value]);
        }
        
        const char* get()                { return categories[value]; }
        const char* get(size_t i)
        { 
            if(i < SIZE)
            {
                return categories[i];
            }
            else return nullptr;
        }
        
        bool operator==(const char* rhs) { return strcmp(rhs, categories[value]) == 0; }
        bool operator!=(const char* rhs) { return strcmp(rhs, categories[value]) != 0; }
        
    private:
        const char* categories[SIZE];
};

#endif