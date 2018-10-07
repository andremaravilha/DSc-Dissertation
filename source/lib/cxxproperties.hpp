/*

MIT License

Copyright (c) 2017 André L. Maravilha

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef CXX_PROPERTIES_HPP
#define CXX_PROPERTIES_HPP

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>


namespace cxxproperties {

    /**
     * An easy to use properties class that allows serialization and
     * deserialization of times. If the type serialized to a property
     * is not a primitive type, it has to implement the flow operators
     * to work properly.
     */
    class Properties {

    public:

        /**
         * Default constructor.
         */
        Properties() = default;

        /**
         * Copy constructor.
         *
         * @param   other
         *          The object to be copied.
         */
        Properties(const Properties &other) = default;

        /**
         * Transfer constructor.
         *
         * @param   other
         *          The object to be transfered.
         */
        Properties(Properties &&other) = default;

        /**
         * Destructor.
         */
        virtual ~Properties() = default;

        /**
         * Assignment operator by copy.
         *
         * @param   other
         *          The object to be copied.
         *
         * @return  A reference to this object.
         */
        Properties &operator=(const Properties &other) = default;

        /**
         * Assignment operator by transfer.
         *
         * @param   other
         *          The object to be transferred.
         *
         * @return  A reference to this object.
         */
        Properties &operator=(Properties &&other) = default;

        /**
         * Get a property by key.
         *
         * @param   key
         *          The key.
         *
         * @return  The value.
         */
        template<class T = std::string>
        T get(const std::string &key) const;

        /**
         * Get a property by its key.
         *
         * @param   key
         *          The key.
         * @param   default_value
         *          The value to be returned if this class does not contains any
         *          property with the key specified.
         *
         * @return  The value of the property or the default value if this class
         *          does not contains the key specified.
         */
        template<class T>
        T get(const std::string &key, const T &default_value) const;

        /**
         * Add a new pair key-value.
         *
         * @param   key
         *          The key.
         * @param   value
         *          The value.
         */
        template<class T>
        void add(const std::string &key, const T &value);

        /**
         * Remove a property by the key.
         *
         * @param   key
         *          The key.
         */
        void remove(const std::string &key);

        /**
         * Return a set with keys of the properties in this object.
         *
         * @return  A set with keys of properties.
         */
        std::set<std::string> get_keys() const;

        /**
         * Check if this object contains a property with the given key.
         *
         * @param   key
         *          A key.
         *
         * @return  True if this object contains a property of the given key, false
         *          otherwise.
         */
        bool contains(std::string key) const;

        /**
         * The number of pairs key-value in this object.
         *
         * @return  The number of pairs key-value in this object.
         */
        std::size_t size() const;

    private:

        std::map<std::string, std::string> properties_;
    };


    template<class T>
    T Properties::get(const std::string &key) const {

        // Load the value into a stream
        std::stringstream stream;
        stream << properties_.at(key);

        // Get the value
        T value;
        stream >> value;

        return value;
    }

    template<>
    inline std::string Properties::get<std::string>(const std::string &key) const {
        return properties_.at(key);
    }

    template<class T>
    T Properties::get(const std::string &key, const T &default_value) const {

        // Check whether there is a property with the key
        if (contains(key)) {
            return get<T>(key);
        }

        // Return the default value
        return default_value;
    }

    template<class T>
    void Properties::add(const std::string &key, const T &value) {

        // Load the value into a stream
        std::stringstream stream;
        stream << value;

        // Store the value
        properties_[key] = stream.str();
    }

    inline void Properties::remove(const std::string &key) {
        properties_.erase(key);
    }

    inline std::set<std::string> Properties::get_keys() const {
        std::set<std::string> keys;
        for (auto iter = properties_.cbegin(); iter != properties_.cend(); ++iter) {
            keys.insert(iter->first);
        }

        return keys;
    }

    inline bool Properties::contains(std::string key) const {
        return (properties_.find(key) != properties_.cend());
    }

    inline std::size_t Properties::size() const {
        return properties_.size();
    }

}

#endif
