#ifndef PROPERTYSTORE_HPP
#define PROPERTYSTORE_HPP

#include "property.hpp"
class BaseStore
{
    protected:
        BaseStore(size_t size): size(size) {};
    public:
        const size_t size;
        virtual BaseProperty* get_property(uint32_t i) = 0;

        struct Iterator
        {
            public:
                using dtype = BaseProperty*;
                using Pointer = dtype*;
                using Reference = dtype&;
                Iterator(Pointer ptr) : m_ptr(ptr) {}    
            
                Reference operator*() const { return *m_ptr; }
                Pointer operator->() { return m_ptr; }

                // Prefix increment
                Iterator& operator++() { m_ptr++; return *this; }  

                // Postfix increment
                Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

                friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
                friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

            private:
                Pointer m_ptr;
        };
        virtual Iterator begin() = 0;
        virtual Iterator end() = 0;
        
};

template<size_t SIZE>
class PropertyStore: public BaseStore
{
    private:
        BaseProperty* properties[SIZE];
        
    public:
        PropertyStore():
            BaseStore(SIZE)
        {}

        PropertyStore(BaseProperty* const (&props)[SIZE]):
            BaseStore(SIZE)
        {
            assign_properties(props);
        }

        Iterator begin()
        {
            return Iterator(&properties[0]);
        }

        Iterator end()
        {
            return Iterator(&properties[SIZE]);
        }
        
        PropertyStore<SIZE> operator=(const PropertyStore<SIZE>& rhs)
        {
            assign_properties(rhs.properties);
            return *this;
        }

        void assign_properties(BaseProperty* const (&props)[SIZE])
        {
            for(uint32_t i = 0; i < SIZE; i++)
            {
                properties[i] = props[i];
            }
        }

        BaseProperty* get_property(uint32_t i)
        {
            if(i >= SIZE) return nullptr;
            else return properties[i];
        }
};

#endif