/**
 * @file linked_list.hpp
 * @brief Определение класса LinkedList для работы со связным списком
 */

#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>

namespace data_structures {

/**
 * @brief Определение структуры узла связного списка
 * 
 * @tparam T Тип данных, хранимых в узле
 */
template <typename T>
struct Node {
    T data;                      ///< Данные, хранящиеся в узле
    std::shared_ptr<Node<T>> next; ///< Указатель на следующий узел
    
    /**
     * @brief Конструктор узла
     * 
     * @param value Значение, которое будет храниться в узле
     */
    explicit Node(const T& value) : data(value), next(nullptr) {}
};

/**
 * @class LinkedList
 * @brief Класс для работы со связным списком
 * 
 * @tparam T Тип данных, хранимых в списке
 */
template <typename T>
class LinkedList {
private:
    std::shared_ptr<Node<T>> head; ///< Указатель на начало списка
    size_t size_;                ///< Размер списка
    
public:
    /**
     * @brief Конструктор по умолчанию
     */
    LinkedList() : head(nullptr), size_(0) {}
    
    /**
     * @brief Конструктор из вектора значений
     * 
     * @param values Вектор значений для инициализации списка
     */
    explicit LinkedList(const std::vector<T>& values);
    
    /**
     * @brief Деструктор
     */
    ~LinkedList() = default;
    
    /**
     * @brief Добавляет элемент в конец списка
     * 
     * @param value Значение, которое будет добавлено
     */
    void append(const T& value);
    
    /**
     * @brief Добавляет элемент в начало списка
     * 
     * @param value Значение, которое будет добавлено
     */
    void prepend(const T& value);
    
    /**
     * @brief Разворачивает список рекурсивно
     */
    void reverseRecursively();
    
    /**
     * @brief Возвращает размер списка
     * 
     * @return size_t Количество элементов в списке
     */
    size_t size() const { return size_; }
    
    /**
     * @brief Проверяет, пуст ли список
     * 
     * @return bool true, если список пуст, иначе false
     */
    bool isEmpty() const { return head == nullptr; }
    
    /**
     * @brief Преобразует список в вектор
     * 
     * @return std::vector<T> Вектор, содержащий элементы списка
     */
    std::vector<T> toVector() const;
    
    /**
     * @brief Создает список из вектора
     * 
     * @param values Вектор значений
     * @return LinkedList<T> Новый связный список
     */
    static LinkedList<T> fromVector(const std::vector<T>& values);
    
    /**
     * @brief Очищает список
     */
    void clear();
    
    /**
     * @brief Возвращает указатель на начало списка
     * 
     * @return std::shared_ptr<Node<T>> Указатель на первый узел
     */
    std::shared_ptr<Node<T>> getHead() const { return head; }
    
private:
    /**
     * @brief Рекурсивная функция для разворота списка
     * 
     * @param node Указатель на текущий узел
     * @return std::shared_ptr<Node<T>> Указатель на новое начало списка
     */
    std::shared_ptr<Node<T>> reverseRecursively(std::shared_ptr<Node<T>> node);
};

// Включаем реализацию шаблонных методов
#include "../../src/linked_list/linked_list.cpp"

} // namespace data_structures

#endif // LINKED_LIST_HPP
