#ifndef ARENA_H
#define ARENA_H

#include <memory>

class Arena {
public:
  template<typename T>
  T *allocate(size_t count) {
    if (this->current >= NODE_CAPACITY) {
      std::unique_ptr<ArenaNode> new_node = std::make_unique<ArenaNode>();
      new_node->next = std::move(this->head);
      this->head = std::move(new_node);
      this->current = 0;
    }

    static_assert((alignof(T) & (alignof(T) - 1)) == 0);
    void *ptr = (void*)(this->head->data + this->current);
    this->current += -(uintptr_t)(ptr) & (alignof(T) - 1);
    T *data = (T*)(this->head->data + this->current);
    this->current += sizeof(T) * count;
    return data;
  }

  Arena() : head{nullptr}, current{NODE_CAPACITY} {}
private:
  static constexpr int NODE_CAPACITY = 1 << 22;
  struct ArenaNode {
    char data[NODE_CAPACITY];
    std::unique_ptr<ArenaNode> next;
  };

  std::unique_ptr<ArenaNode> head;
  int current;
};

#endif
