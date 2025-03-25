/**
 * @file duplicate_remover.hpp
 * @brief Класс для удаления дубликатов из контейнеров
 */

#ifndef DUPLICATE_REMOVER_HPP
#define DUPLICATE_REMOVER_HPP

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace containers {

/**
 * @class DuplicateRemover
 * @brief Класс для удаления дубликатов из контейнеров с сохранением порядка элементов
 * 
 * @tparam T Тип элементов контейнера
 */
class DuplicateRemover {
public:
    /**
     * @brief Удаляет дубликаты из контейнера, сохраняя порядок элементов
     * 
     * @tparam Container Тип контейнера
     * @param container Входной контейнер
     * @return Container Контейнер без дубликатов
     */
    template <typename Container>
    static Container removeDuplicates(const Container& container) {
        using T = typename Container::value_type;
        
        if (container.empty()) {
            return container;
        }
        
        Container result;
        // Резервируем память для результата, если контейнер поддерживает reserve
        if constexpr (has_reserve_v<Container>) {
            result.reserve(container.size());
        }
        
        std::unordered_set<T> seen;
        
        for (const auto& item : container) {
            // Если элемент еще не встречался, добавляем его в результат
            if (seen.find(item) == seen.end()) {
                seen.insert(item);
                result.push_back(item);
            }
        }
        
        return result;
    }
    
    /**
     * @brief Удаляет дубликаты из диапазона итераторов, сохраняя порядок элементов
     * 
     * @tparam InputIt Тип входного итератора
     * @tparam OutputIt Тип выходного итератора
     * @param first Итератор на начало входного диапазона
     * @param last Итератор на конец входного диапазона
     * @param d_first Итератор на начало выходного диапазона
     * @return OutputIt Итератор на конец результирующего диапазона
     */
    template <typename InputIt, typename OutputIt>
    static OutputIt removeDuplicates(InputIt first, InputIt last, OutputIt d_first) {
        using T = typename std::iterator_traits<InputIt>::value_type;
        
        std::unordered_set<T> seen;
        
        for (; first != last; ++first) {
            if (seen.insert(*first).second) {
                *d_first++ = *first;
            }
        }
        
        return d_first;
    }

private:
    // SFINAE для проверки наличия метода reserve у контейнера
    template <typename, typename = void>
    struct has_reserve : std::false_type {};
    
    template <typename T>
    struct has_reserve<T, std::void_t<decltype(std::declval<T>().reserve(std::declval<typename T::size_type>()))>> : std::true_type {};
    
    template <typename T>
    static constexpr bool has_reserve_v = has_reserve<T>::value;
};

} // namespace containers

#endif // DUPLICATE_REMOVER_HPP
