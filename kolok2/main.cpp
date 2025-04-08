#include <iostream>
#include <mutex>
#include <atomic>
#include <functional>
#include <stdexcept>

// Политики создания
template <typename T>
struct CreateUsingNew {
    static T* create() {
        return new T();
    }
    static void destroy(T* p) {
        delete p;
    }
};

template <typename T>
struct CreateStatic {
    static T* create() {
        static T instance;
        return &instance;
    }
    static void destroy(T* /*p*/) {}
};

// Политики времени жизни
template <typename T>
class DefaultLifetime {
public:
    static void scheduleDestruction(T* /*instance*/, void (*destructor)()) {
        std::atexit(destructor);
    }
    static void onDeadReference() {
        throw std::runtime_error("Dead Reference detected");
    }
};

template <typename T>
class PhoenixLifetime {
public:
    static void scheduleDestruction(T* instance, void (*destructor)()) {
        std::atexit(destructor);
    }
    static void onDeadReference() {
        // Разрешить воссоздание объекта
    }
};

// Политики синхронизации
template <typename T>
class SingleThreaded {
protected:
    using Mutex = int; // Фиктивный тип

    class Lock {
    public:
        Lock(Mutex&) {} // Пустой конструктор
    };

    static void init() {}
    static void destroy() {}
};

template <typename T>
class MultiThreaded {
protected:
    using Mutex = std::mutex;
    static Mutex& mutex() {
        static Mutex mtx;
        return mtx;
    }

    class Lock {
    public:
        Lock() : lock_(mutex()) {}
    private:
        std::unique_lock<Mutex> lock_;
    };

    static std::once_flag once_flag_;
    static void init() {
        std::call_once(once_flag_, []{});
    }
    static void destroy() {}
};

template <typename T>
std::once_flag MultiThreaded<T>::once_flag_;

// Базовый класс SingletonHolder
template <
    typename T,
    template <typename> class CreationPolicy,
    template <typename> class LifetimePolicy,
    template <typename> class ThreadingModel
>
class SingletonHolder : private ThreadingModel<T> {
private:
    static T* instance_;
    static bool destroyed_;
    using ThreadingModel<T>::mutex;
    using typename ThreadingModel<T>::Lock;

    static void destroySingleton() {
        CreationPolicy<T>::destroy(instance_);
        instance_ = nullptr;
        destroyed_ = true;
    }

public:
    static T& instance() {
        if (destroyed_) {
            Lock lock;
            LifetimePolicy<T>::onDeadReference();
            destroyed_ = false;
        }

        static std::once_flag init_flag;
        std::call_once(init_flag, [] {
            Lock lock;
            instance_ = CreationPolicy<T>::create();
            LifetimePolicy<T>::scheduleDestruction(instance_, &destroySingleton);
        });

        return *instance_;
    }

protected:
    SingletonHolder() = default;
    ~SingletonHolder() = default;
};

// Инициализация статических членов
template <typename T, template <typename> class CP, template <typename> class LP, template <typename> class TM>
T* SingletonHolder<T, CP, LP, TM>::instance_ = nullptr;

template <typename T, template <typename> class CP, template <typename> class LP, template <typename> class TM>
bool SingletonHolder<T, CP, LP, TM>::destroyed_ = false;

// Пример использования
class DatabaseConnection {
public:
    void connect() {
        std::cout << "Connected to database\n";
    }
};

int main() {
    using Singleton = SingletonHolder<
        DatabaseConnection,
        CreateUsingNew,
        DefaultLifetime,
        MultiThreaded
    >;

    Singleton::instance().connect();
    return 0;
}
