#pragma once

#include <iterator>
#include <stdexcept>


template <typename T>
class CircularBuffer;

template <typename T>
class LoopIterator {
	friend class CircularBuffer<T>;
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	LoopIterator& operator=(const LoopIterator& other) {
		p = other.p;
		cb_data = other.cb_data;
		cb_capacity = other.cb_capacity;
		return *this;
	}

	bool operator==(LoopIterator const& other) const {
		return p == other.p;
	}

	bool operator!=(LoopIterator const& other) const {
		return !(*this == other); /// p != other.p;
	}

	bool operator<(LoopIterator const& other) const {
		return p < other.p; 
	}

	bool operator<=(LoopIterator const& other) const {
		return *this < other ||  *this == other;
	}

	bool operator>(LoopIterator const& other) const {
		return !(*this <= other);
	}

	bool operator>=(LoopIterator const& other) const {
		return !(*this < other);
	}

	reference operator*() const {
		return *p;
	}
	/// Node: post inc, post dec
	LoopIterator& operator++() {
		++p;
		if (p == cb_data + cb_capacity) {
			p = cb_data;
		}
		return *this;
	}

	LoopIterator& operator--() {
		if (p == cb_data) {
			p = cb_data + cb_capacity - 1;
		}
		--p;
		return *this;
	}

	difference_type operator-(LoopIterator const& other) const {
		if (*this < other)
			return (p + cb_capacity - other.p);
		return p - other.p;
	}

	LoopIterator& operator+=(int n) {
		n = n % cb_capacity;
		if (p + n < cb_data + cb_capacity) {
			p = p + n;
		}
		else {
			p = p + n - cb_capacity;
		}
		return *this;
	}

	LoopIterator& operator-=(int n) {
		n = n % cb_capacity;
		if (p - n < cb_data) {
			p = p - n + cb_capacity;
		}
		else {
			p = p - n;
		}
		return *this;
	}

private:
	LoopIterator(T* p, T* cb_data, int cb_capacity) :
		p(p),
		cb_data(cb_data),
		cb_capacity(cb_capacity) {}

	T* p;
	T* cb_data;
	int cb_capacity;
};

template <typename T>
LoopIterator<T> operator+(const LoopIterator<T>& element, int n) {
	LoopIterator<T> result = element;
	result += n;
	return result;
}

template <typename T>
LoopIterator<T> operator-(const LoopIterator<T>& element, int n) {
	LoopIterator<T> result = element;
	result -= n;
	return result;
}

template <typename T>
class CircularBuffer {
	friend class LoopIterator<T>;
	
private:
	size_t size = 0;
	size_t capacity;
	int begin_ = 0;
	T* data;

public:

	using iterator = LoopIterator<T>;
	CircularBuffer(size_t new_capacity) : capacity(new_capacity + 1) {
		data = new T[capacity];
	}

	~CircularBuffer() {
		delete[] data;
	}

	CircularBuffer& operator=(const CircularBuffer& other) {
		auto ptr = new T[capacity];
		std::swap(size, other.size);
		std::swap(capacity, other.capacity);
		std::swap(begin_, other.begin_);
		for (size_t i = 0; i < capacity; ++i) {
			try {
				ptr[i] = other.data[i];
			} catch(...) {
				delete[] ptr;
				throw;
			}
		}
		delete[] data;
		data = ptr;
		return *this;
	}

	T first() const {
		return data[begin_];
	}

	T last() const {
		return data[(begin_ + size - 1) % capacity];
	}

	T operator [] (size_t i) const {
		if (size == 0 || i >= size) {
			throw std::out_of_range("");
		}
		return data[(begin_ + i) % capacity];
	}

	T& operator [] (size_t i) {
		if (size == 0 || i >= size) {
			throw std::out_of_range("");
		}
		return data[(begin_ + i) % capacity];
	}

	void addFirst(const T & value) {
		if (!size) {
			data[0] = value;
		}
		else {
			begin_ = (begin_ - 1 + capacity) % capacity;
			data[begin_] = value;
		}
		++size;
		if (size == capacity) {
			--size;
		}
	}

	void addLast(const T & value) {
		if (!size) {
			data[0] = value;
		}
		else {
			data[(begin_ + size) % capacity] = value;
		}
		++size;
		if (size == capacity) {
			--size;
		}
	}

	void delFirst() {
		if (size == 1) {
			begin_ = 0;
		}
		else {
			begin_ = (begin_ + 1) % capacity;
		}
		--size;
	}

	void delLast() {
		if (size == 1) {
			begin_ = 0;
		}
		--size;
	}

	/// note make private
	void changeCapacity(size_t new_capacity) {
		T* new_data = new T[new_capacity + 1];
		for (int i = 0; i < capacity; ++i) {
			try {
				new_data[i] = data[i];
			} catch(...) {
				delete[] new_data;
			}
		}
		delete[] data;
		capacity = new_capacity + 1;
		data = new_data;
	}

	iterator begin() const {
		return iterator(data + begin_, data, capacity);
	}

	iterator end() const {
		return iterator(data + (begin_ + size) % capacity, data, capacity);
	}
};
