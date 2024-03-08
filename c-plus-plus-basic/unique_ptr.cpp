#include <iostream>
#include <memory>

int main() {
    std::size_t size = 5;
    std::unique_ptr<int[]> ptr = std::make_unique<int[]>(size);
    
    for (std::size_t i = 0; i < size; ++i) {
        ptr[i] = i + 1;
    }
    
    std::cout << "Array: ";
    for (std::size_t i = 0; i < size; ++i) {
        std::cout << ptr[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}

