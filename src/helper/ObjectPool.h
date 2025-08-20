#ifndef MB_OBJECT_POOL_H
#define MB_OBJECT_POOL_H

#include <vector>
#include <iostream>
#include <mutex>

namespace mb {
namespace helpers {

template <typename T>
class ObjectPool {
public:
    ObjectPool(size_t pool_size) { 
        m_pool.resize(pool_size);
        for (auto& item : m_pool) {
            item.object = new T();
        } 
    }
    ~ObjectPool() {
        for (auto& item : m_pool) {
            if (item.object) delete item.object;
        }
    }

    T* acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& item : m_pool) {
            if (!item.active) {
                item.active = true;
                return item.object;
            }
        }
        return nullptr;
    }

    void release(T* obj) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& item : m_pool) {
            if (item.object == obj) {
                item.active = false;
                break;
            }
        }
    }

private:
    struct PoolItem {
        T* object;
        bool active = false;
    };

    std::vector<PoolItem> m_pool;
    std::mutex m_mutex;
};

} // helpers
} // mb

#endif // MB_OBJECT_POOL_H

// Пример использования
// class Particle {
// public:
//     void init(int x, int y) {
//         this->x = x;
//         this->y = y;
//     }

//     void display() const {
//         std::cout << "Particle at (" << x << ", " << y << ")" << std::endl;
//     }

// private:
//     int x = 0, y = 0;
// };

// int main() {
//     ObjectPool<Particle> particlePool(5);

//     // Получаем объекты из пула
//     Particle* p1 = particlePool.acquire();
//     Particle* p2 = particlePool.acquire();

//     if (p1) p1->init(10, 20);
//     if (p2) p2->init(30, 40);

//     particlePool.debug();

//     if (p1) p1->display();
//     if (p2) p2->display();

//     // Возвращаем объекты в пул
//     particlePool.release(p1);
//     particlePool.release(p2);

//     particlePool.debug();

//     return 0;
// }
