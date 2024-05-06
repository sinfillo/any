#pragma once

#include <memory>
#include <typeinfo>

class Any {
public:
    Any() {
    }

    template <class T>
    Any(const T& value) {
        ptr_ = new Inner<T>(value);
    }

    template <class T>
    Any& operator=(const T& value) {
        delete ptr_;
        ptr_ = new Inner<T>(value);
        return *this;
    }

    Any(const Any& rhs) {
        delete ptr_;
        if (rhs.ptr_) {
            ptr_ = rhs.ptr_->Clone();
        } else {
            ptr_ = nullptr;
        }
    }

    Any& operator=(const Any& rhs) {
        if (this == &rhs) {
            return *this;
        }
        delete ptr_;
        ptr_ = nullptr;
        Any cur(rhs);
        Swap(cur);
        return *this;
    }

    ~Any() {
        delete ptr_;
        ptr_ = nullptr;
    }

    bool Empty() const {
        if (ptr_) {
            return false;
        }
        return true;
    }

    void Clear() {
        delete ptr_;
        ptr_ = nullptr;
    }
    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        if (typeid(T) != ptr_->Type()) {
            throw std::bad_cast();
        }
        return *(static_cast<const Inner<T>&>(*ptr_));
    }

private:
    struct InnerBase {
        virtual ~InnerBase() {
        }
        virtual InnerBase* Clone() const = 0;
        virtual const std::type_info& Type() const = 0;
    };

    template <class T>
    struct Inner : public InnerBase {
        Inner(const T& data) : data_(data){};
        virtual InnerBase* Clone() const override {
            return new Inner<T>(data_);
        }
        T& operator*() {
            return data_;
        }
        const T& operator*() const {
            return data_;
        }
        virtual const std::type_info& Type() const override {
            return typeid(data_);
        }

    private:
        T data_;
    };

    InnerBase* ptr_ = nullptr;
};
