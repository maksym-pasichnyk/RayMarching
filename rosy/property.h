#pragma once

#include <functional>

template <typename...>
struct property;

template <>
struct property<> {
    using get = struct {};
    using set = struct {};
};

template <typename T>
struct property<T> {
    using value_type = std::decay_t<T>;

    property(const property&) = delete;
    property(property&&) = delete;

    property& operator=(const property&) = delete;
    property& operator=(property&&) = delete;

    property(value_type& value) : m_value(value) {}
    property(value_type&& value) : m_value(std::forward<T>(value)) {}

//    operator value_type&() {
//        return m_value;
//    }
//
//    operator const value_type&() const {
//        return m_value;
//    }

    property& operator =(value_type& v) {
        m_value = v;
        return *this;
    }

    property& operator =(value_type&& v) {
        m_value = std::forward<T>(v);
        return *this;
    }
private:
    T m_value;
};

template <typename T>
struct property<T, property<>::get> {
    property(const property&) = delete;
    property(property&&) = delete;

    property& operator=(const property&) = delete;
    property& operator=(property&&) = delete;

    template <typename U>
    property(U&& get) : m_get(get) {}

//    inline operator T() noexcept {
//        return m_get();
//    }
//
//    inline operator const T() const noexcept {
//        return m_get();
//    }

//private:
    const std::function<T()> m_get;
};

template <typename T>
struct property<T, property<>::set> {
    using value_type = std::decay_t<T>;

    property(const property&) = delete;
    property(property&&) = delete;

    property& operator=(const property&) = delete;
    property& operator=(property&&) = delete;

    template <typename U>
    property(U&& get) : m_set(get) {}

    property& operator =(value_type& v) {
        m_set(v);
        return *this;
    }

    property& operator =(value_type&& v) {
        m_set(std::forward<T>(v));
        return *this;
    }

//private:
    const std::function<void(const T&)> m_set;
};

template <typename T>
struct property<T, property<>::get, property<>::set> {
    using value_type = std::decay_t<T>;

    property(const property&) = delete;
    property(property&&) = delete;

    property& operator=(const property&) = delete;
    property& operator=(property&&) = delete;

    const std::function<T()> get;
    const std::function<void(const T&)> set;

//    inline operator T() noexcept {
//        return get();
//    }
//
//    inline operator const T() const noexcept {
//        return get();
//    }

    property& operator =(value_type& v) {
        set(v);
        return *this;
    }

    property& operator =(value_type&& v) {
        set(std::forward<T>(v));
        return *this;
    }
};

//template <typename T>
//property(T&) -> property<T>;
//
//template <typename T>
//property(getter<T>&& v) -> property<T, property_mode::read>;
//
//template <typename T>
//property(getter<T>&& v) -> property<T, property_mode::write>;
//
//template <typename T>
//property(getter<T>&& get, setter<T>&& set) -> property<T, property_mode::read_write>;
//
//template <typename T>
//property(setter<T>&& set, getter<T>&& get) -> property<T, property_mode::read_write>;
