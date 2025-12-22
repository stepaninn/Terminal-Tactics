#ifndef INC_3_MATRIX_H
#define INC_3_MATRIX_H

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

template<std::default_initializable T>
class Matrix;

template<typename T, bool is_const>
class MatrixIterator {

private:
    typedef std::conditional_t<is_const, const T, T>* element_ptr_t;

    element_ptr_t ptr_ = nullptr;

    explicit MatrixIterator(element_ptr_t ptr, size_t rows, size_t cols) noexcept : ptr_(ptr), rows_(rows), cols_(cols) {}

    friend Matrix<T>;
    friend MatrixIterator<T, !is_const>;

    size_t rows_ = 0;
    size_t cols_ = 0;

public:

    /*!
     * @brief Тип, обозначающий дистанцию между двумя итераторами,
     *        т.е. результат std::distance(it1, it2)
     */
    typedef ptrdiff_t difference_type;

    /*!
     * @brief Адресуемый итератором тип
     */
    typedef T value_type;

    /*!
     * @brief Тип указателя на хранимое значение,
     *        для const_iterator - const T*, для обычного - T*
     */
    typedef std::conditional_t<is_const, const T, T>* pointer;

    /*!
     * @brief Тип ссылки на хранимое значение,
     *        для const_iterator - const T&, для обычного - T&
     */
    typedef std::conditional_t<is_const, const T, T>& reference;

    /*!
     * @brief Категория итератора
     */
    typedef std::random_access_iterator_tag iterator_category;

    /*!
     * @brief Тип, используемый для преставления размера контейнера
     */
    typedef size_t size_type;

    /*!
     * @brief Пустой конструктор, разыменование созданного итератора приведёт к UB
     * @note Реализует default_initializable из regular
     */
    MatrixIterator() noexcept = default;

    /*!
     * @brief Копирующий конструктор
     * @tparam other_const Константность копируемого итератора
     * @note Допускает копирование итератора иной константности
     * @note Реализует std::move_constructible и std::copy_constructible
     */
    template<bool other_const>
    MatrixIterator(const MatrixIterator<T, other_const>& o) noexcept
      // нельзя из неконстантного в константный
      requires (is_const >= other_const) : ptr_(o.ptr_), rows_(o.rows_), cols_(o.cols_) {}

    /*!
     * @brief Копирующий конструктор с явным заданием размеров
     * @tparam other_const Константность копируемого итератора
     * @note Допускает копирование итератора иной константности
     * @note Реализует std::move_constructible и std::copy_constructible
     */
    template<bool other_const>
    explicit MatrixIterator(const MatrixIterator<T, other_const>& o, size_type rows, size_type cols) noexcept
      // нельзя из неконстантного в константный
      requires (is_const >= other_const) : ptr_(o.ptr_), rows_(rows), cols_(cols) {}

    /*!
     * @brief Копирующее перемещениеo,
     * @returns Ссылка на переданный итератор
     * @tparam other_const Константность копируемого итератора
     * @note Допускает копирование итератора иной константности
     * @note Реализует std::assignable_from<T&, T>, std::copyable и std::swappable
     */
    template<bool other_const>
    MatrixIterator& operator = (const MatrixIterator<T, other_const>& o) noexcept
      // нельзя из неконстантного в константный
      requires (is_const >= other_const) {
        ptr_ = o.ptr_;
        rows_ = o.rows_;
        cols_ = o.cols_;
        return *this;
    }

    /*!
     * @brief Разыменование
     * @returns Ссылка на адресуемый элемент списка
     * @note Реализует `{ *i } -> / *can-reference* /` из input_or_output_iterator и indirectly_readable
     */
    reference operator * () const noexcept { return *ptr_; }

    /*!
     * @brief Обращение к полям элемента
     * @returns Указатель на адресуемый элемент списка
     */
    pointer operator -> () const noexcept { return ptr_; }

    /*!
     * @brief Сравнение итераторов
     * @tparam other_const Константность копируемого итератора
     * @returns true если итераторы ссылаются на один и тот же элемент,
     *          иначе false
     * @note Реализует equality_comparable из regular
     * @note Оператор != получаем автоматически
     */
    template<bool other_const>
    bool operator == (const MatrixIterator<T, other_const>& o) const noexcept {
        return ptr_ == o.ptr_;
    }

    /*!
     * @brief Передвигает итератор на следующий элемент списка
     * @returns Ссылка на переданный итератор
     * @note Реализует `{ ++i } -> std::same_as<I&>` из weakly_incrementable
     */
    MatrixIterator& operator ++ () noexcept {
        ++ptr_;
        return *this;
    }

    /*!
     * @brief Передвигает итератор на следующий элемент списка
     * @returns Состояние итератора до модификации
     */
    MatrixIterator operator ++ (int) noexcept {
        auto tmp = *this;
        ++ptr_;
        return tmp;
    }

    /*!
     * @brief Передвигает итератор на предыдущий элемент списка
     * @returns Ссылка на переданный итератор
     */
    MatrixIterator& operator -- () noexcept {
        --ptr_;
        return *this;
    }

    /*!
     * @brief Передвигает итератор на предыдущий элемент списка
     * @returns Состояние итератора до модификации
     */
    MatrixIterator operator -- (int) noexcept {
        auto tmp = *this;
        --ptr_;
        return tmp;
    }

    MatrixIterator& move_north(difference_type n) noexcept {
        ptr_ -= cols_ * n;
        return *this;
    }

    MatrixIterator& move_south(difference_type n) noexcept {
        ptr_ += cols_ * n;
        return *this;
    }

    MatrixIterator& move_east(difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    MatrixIterator& move_west(difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    MatrixIterator& operator += (difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    MatrixIterator& operator -= (difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    reference operator [] (difference_type n) const noexcept { return ptr_[n]; }

    [[nodiscard]] pointer base() const noexcept { return ptr_; }

    friend MatrixIterator operator + (MatrixIterator it, difference_type n) noexcept {
        it += n;
        return it;
    }

    friend MatrixIterator operator + (difference_type n, MatrixIterator it) noexcept {
        it += n;
        return it;
    }

    friend MatrixIterator operator - (MatrixIterator it, difference_type n) noexcept {
        it -= n;
        return it;
    }

    friend difference_type operator - (const MatrixIterator& a, const MatrixIterator& b) noexcept {
        return a.ptr_ - b.ptr_;
    }

    friend bool operator < (const MatrixIterator& a, const MatrixIterator& b) noexcept {
        return a.ptr_ < b.ptr_;
    }

    friend bool operator > (const MatrixIterator& a, const MatrixIterator& b) noexcept {
        return b < a;
    }

    friend bool operator <= (const MatrixIterator& a, const MatrixIterator& b) noexcept {
        return !(b < a);
    }

    friend bool operator >= (const MatrixIterator& a, const MatrixIterator& b) noexcept {
        return !(a < b);
    }
};

static_assert(std::random_access_iterator<MatrixIterator<int, false>>);
static_assert(std::random_access_iterator<MatrixIterator<int, true>>);

/*!
 * @brief Матрица
 * @tparam T Тип хранимых значений
 */
template <std::default_initializable T>
class Matrix {

private:
    size_t rows_ = 0;
    size_t cols_ = 0;

    std::unique_ptr<T[]> data_;

public:

     /*!
     * @brief Тип хранимых значений
     */
    typedef T value_type;

    /*!
     * @brief Ссылка на тип хранимых значений
     */
    typedef T& reference;

    /*!
     * @brief Константная ссылка на тип хранимых значений
     */
    typedef const T& const_reference;

    /*!
     * @brief Тип итератора
     */
    typedef MatrixIterator<T, false> iterator;

    /*!
     * @brief Тип константного итератора
     */
    typedef MatrixIterator<T, true> const_iterator;

    /*!
     * @brief Тип, обозначающий дистанцию между двумя итераторами,
     *        т.е. результат std::distance(it1, it2)
     */
    typedef ptrdiff_t difference_type;

    /*!
     * @brief Тип, используемый для преставления размера контейнера
     */
    typedef size_t size_type;

    /*!
     * @brief Пустой конструктор, создаёт пустой список
     */
    Matrix()
      // noexcept если T() это noexcept
      noexcept(std::is_nothrow_default_constructible_v<T>) = default;


    /*!
     * @brief Конструктор, принимающий размеры матрицы
     * и заполняющий ее значениями по умолчанию
     * @param rows Количество строк
     * @param cols Количество столбцов
     */
    Matrix(size_type rows, size_type cols) : rows_(rows), cols_(cols),
        data_( (rows == 0 || cols == 0) ? nullptr : std::make_unique<T[]>(rows * cols) ) {}

    /*!
     * @brief Конструктор, заполняющий матрицу rows*cols копиями value
     * @param rows Количество строк
     * @param cols Количество столбцов
     * @param value Значение элемента, которым будет заполнена матрица
     */
    Matrix(size_type rows, size_type cols, const T& value) requires std::copy_constructible<T>
        : Matrix(rows, cols)
    {
        if (data_) std::fill_n(data_.get(), rows_ * cols_, value);
    }

    /*!
     * @brief Копирующий конструктор
     * @param other Копируемая матрица
     */
    Matrix(const Matrix& other) requires std::copy_constructible<T> : rows_(other.rows_), cols_(other.cols_),
        data_( (other.size() == 0) ? nullptr : std::make_unique<T[]>(other.size()) )
    {
        std::copy_n(other.data_.get(), size(), data_.get());
    }

    /*!
     * @brief Перемещающий конструктор
     * @param o Перемещаемая матрица
     */
    Matrix(Matrix&& o) noexcept(std::is_nothrow_default_constructible_v<T>)
        : rows_(o.rows_), cols_(o.cols_), data_(std::move(o.data_))
    {
        o.rows_ = o.cols_ = 0;
    }

    /*!
     * @brief Копирующий оператор присваивания
     * @param other Копируемая матрица
     */
    Matrix& operator = (const Matrix& other) requires std::copy_constructible<T> {
        if (this == &other) return *this;
        if (other.size() == 0) {
            data_.reset();
            rows_ = cols_ = 0;
            return *this;
        }

        if (!data_ || size() != other.size()) {
            data_ = std::make_unique<T[]>(other.size());
        }
        rows_ = other.rows_;
        cols_ = other.cols_;
        std::copy_n(other.data_.get(), size(), data_.get());
        return *this;
    }

    /*!
     * @brief Перемещающий оператор присваивания
     * @param other Перемещаемая матрица
     */
    Matrix& operator = (Matrix&& other) noexcept {
        if (this == &other) return *this;
        data_ = std::move(other.data_);
        rows_ = other.rows_;
        cols_ = other.cols_;
        other.rows_ = other.cols_ = 0;
        return *this;
    }

    /// @brief Получение количества строк матрицы
    [[nodiscard]] size_type rows() const noexcept { return rows_; }
    /// @brief Получение количества столбцов матрицы
    [[nodiscard]] size_type cols() const noexcept { return cols_; }
    /// @brief Получение размера матрицы
    [[nodiscard]] size_type size() const noexcept { return rows_ * cols_; }
    /// @brief Проверка на пустоту
    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    /// @brief Получение указателя на данные матрицы
    T* data() noexcept { return data_.get(); }
    /// @brief Получение указателя на константные данные матрицы
    const T* data() const noexcept { return data_.get(); }

    /*!
     * @brief Получения итератора на начало матрицы
     * @returns Итератор, адресующий начало матрицы
     */
    iterator begin() noexcept {
        return iterator(data_ ? data_.get() : nullptr, rows_, cols_);
    }

    /*!
     * @brief Получения итератора на конец матрицы
     * @returns Итератор, адресующий конец матрицы
     */
    iterator end() noexcept {
        return data_ ? iterator(data_.get() + size(), rows_, cols_)
                     : iterator(nullptr, rows_, cols_);
    }

    /*!
     * @brief Получения константного итератора на начало матрицы
     * @returns Итератор, адресующий начало матрицы
     */
    const_iterator begin() const noexcept {
        return const_iterator(data_ ? data_.get() : nullptr, rows_, cols_);
    }

    /*!
     * @brief Получения константного итератора на конец матрицы
     * @returns Итератор, адресующий конец матрицы
     */
    const_iterator end() const noexcept {
        return data_ ? const_iterator(data_.get() + size(), rows_, cols_)
                     : const_iterator(nullptr, rows_, cols_);
    }

    /*!
     * @brief Получения константного итератора на начало матрицы
     * @returns Итератор, адресующий начало матрицы
     */
    const_iterator cbegin() const noexcept {
        return const_iterator(data_ ? data_.get() : nullptr, rows_, cols_);
    }

    /*!
     * @brief Получения константного итератора на конец матрицы
     * @returns Итератор, адресующий конец матрицы
     */
    const_iterator cend() const noexcept {
        return data_ ? const_iterator(data_.get() + size(), rows_, cols_)
                     : const_iterator(nullptr, rows_, cols_);
    }

    T& operator()(size_type r, size_type c) noexcept {
        return data_.get()[r * cols_ + c];
    }
    const T& operator()(size_type r, size_type c) const noexcept {
        return data_.get()[r * cols_ + c];
    }

    T* operator [] (size_type r) noexcept {
        return data_.get() + r * cols_;
    }
    const T* operator [] (size_type r) const noexcept {
        return data_.get() + r * cols_;
    }

    reference at(size_type r, size_type c) {
        if (r >= rows_ || c >= cols_) throw std::out_of_range("index out of range");
        return operator()(r,c);
    }
    const_reference at(size_type r, size_type c) const {
        if (r >= rows_ || c >= cols_) throw std::out_of_range("index out of range");
        return operator()(r,c);
    }

    void resize(size_type new_rows, size_type new_cols)
      requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) {
        if (new_rows == rows_ && new_cols == cols_) return;
        size_type new_size = new_rows * new_cols;
        std::unique_ptr<T[]> new_data = (new_size == 0) ? nullptr : std::make_unique<T[]>(new_size);

        if (new_data) {
            size_type min_rows = std::min(rows_, new_rows);
            size_type min_cols = std::min(cols_, new_cols);
            for (size_type r = 0; r < min_rows; ++r) {
                T* dest = new_data.get() + r * new_cols;
                T* src  = data_.get()    + r * cols_;

                for (size_type c = 0; c < min_cols; ++c) {
                    dest[c] = std::move(src[c]);
                }

                for (size_type c = min_cols; c < new_cols; ++c) {
                    dest[c] = T();
                }
            }

            for (size_type r = min_rows; r < new_rows; ++r) {
                T* dest = new_data.get() + r * new_cols;
                for (size_type c = 0; c < new_cols; ++c) dest[c] = T();
            }
        }

        data_.swap(new_data);
        rows_ = new_rows;
        cols_ = new_cols;
    }

};

#endif //INC_3_MATRIX_H
