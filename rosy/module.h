#pragma once

#include <type_traits>
#include <functional>

#include <iostream>

template <typename T>
class module {
public:
    using type = T;
    class storage {
        bool m_constructed = false;
        std::aligned_storage_t<sizeof(T), alignof(T)> m_data;

    public:
        storage() = default;
        storage(const storage&) = delete;
        storage& operator=(const storage&) = delete;

        storage(storage&&) = delete;
        storage& operator=(storage&&) = delete;

        ~storage() {
            if (m_constructed) {
                default_destructor(reinterpret_cast<T*>(&m_data));
                m_constructed = false;
            }
        }

        inline T* operator ->() noexcept {
            if (!m_constructed) {
                m_constructed = true;

                default_constructor(reinterpret_cast<T*>(&m_data));
            }
            return reinterpret_cast<T*>(&m_data);
        }
    };

    inline T* operator ->() const noexcept {
        return m_storage.operator->();
    }
private:
    template <typename... Args>
    inline static void default_constructor(T* p, Args&&... args) {
        new (p) T(std::forward<Args>(args)...);
    }
    inline static void default_destructor(T* ptr) {
        ptr->~T();
    }

    inline static storage m_storage;
};