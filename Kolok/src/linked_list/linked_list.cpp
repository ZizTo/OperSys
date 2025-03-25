/**
 * @file linked_list.cpp
 * @brief Реализация класса LinkedList
 */

namespace data_structures {

template <typename T>
LinkedList<T>::LinkedList(const std::vector<T>& values) : head(nullptr), size_(0) {
    for (const auto& value : values) {
        append(value);
    }
}

template <typename T>
void LinkedList<T>::append(const T& value) {
    auto newNode = std::make_shared<Node<T>>(value);
    
    if (!head) {
        head = newNode;
        size_++;
        return;
    }
    
    auto current = head;
    while (current->next) {
        current = current->next;
    }
    
    current->next = newNode;
    size_++;
}

template <typename T>
void LinkedList<T>::prepend(const T& value) {
    auto newNode = std::make_shared<Node<T>>(value);
    newNode->next = head;
    head = newNode;
    size_++;
}

template <typename T>
void LinkedList<T>::reverseRecursively() {
    if (!head || !head->next) {
        return; // Пустой список или список из одного элемента
    }
    
    head = reverseRecursively(head);
}

template <typename T>
std::shared_ptr<Node<T>> LinkedList<T>::reverseRecursively(std::shared_ptr<Node<T>> node) {
    // Базовый случай: последний или предпоследний узел
    if (!node || !node->next) {
        return node;
    }
    
    // Рекурсивно разворачиваем оставшуюся часть списка
    auto newHead = reverseRecursively(node->next);
    
    // Меняем указатели
    node->next->next = node;
    node->next = nullptr;
    
    // Возвращаем новое начало списка
    return newHead;
}

template <typename T>
std::vector<T> LinkedList<T>::toVector() const {
    std::vector<T> result;
    result.reserve(size_); // Оптимизация: резервируем память
    
    auto current = head;
    while (current) {
        result.push_back(current->data);
        current = current->next;
    }
    
    return result;
}

template <typename T>
LinkedList<T> LinkedList<T>::fromVector(const std::vector<T>& values) {
    return LinkedList<T>(values);
}

template <typename T>
void LinkedList<T>::clear() {
    head = nullptr;
    size_ = 0;
}

} // namespace data_structures
