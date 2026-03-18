#pragma once

#include <functional>
#include <unordered_map>

namespace Physics::Utils {
template <typename TLeft, typename TRight> class BiMap {
  public:
    BiMap() = default;
    ~BiMap() = default;

    BiMap(const BiMap &) = delete;
    BiMap &operator=(const BiMap &) = delete;
    BiMap(BiMap &&) = default;
    BiMap &operator=(BiMap &&) = default;

    void Add(const TLeft &left, const TRight &right)
    {
        auto existingIt = this->_leftToRight.find(left);
        if (existingIt != this->_leftToRight.end())
        {
            this->_rightToLeft.erase(*(existingIt->second));
        }
        auto existingRightIt = this->_rightToLeft.find(right);
        if (existingRightIt != this->_rightToLeft.end())
        {
            this->_leftToRight.erase(*(existingRightIt->second));
        }
        auto aitr = this->_leftToRight.insert_or_assign(left, nullptr);
        auto bp = &((*aitr.first).first);
        auto bitr = this->_rightToLeft.insert_or_assign(right, bp).first;
        auto ap = &((*bitr).first);
        (*aitr.first).second = ap;
    }
    void Remove(const TLeft &left)
    {
        auto it = this->_leftToRight.find(left);
        if (it != this->_leftToRight.end())
        {
            this->_rightToLeft.erase(*(it->second));
            this->_leftToRight.erase(it);
        }
    }
    void Remove(const TRight &right)
    {
        auto it = this->_rightToLeft.find(right);
        if (it != this->_rightToLeft.end())
        {
            this->_leftToRight.erase(*(it->second));
            this->_rightToLeft.erase(it);
        }
    }
    std::uint32_t Size() const { return static_cast<std::uint32_t>(this->_leftToRight.size()); }
    const TRight &Get(const TLeft &left) const { return *this->_leftToRight.at(left); }
    const TLeft &Get(const TRight &right) const { return *this->_rightToLeft.at(right); }
    bool Contains(const TLeft &left) const { return this->_leftToRight.contains(left); }
    bool Contains(const TRight &right) const { return this->_rightToLeft.contains(right); }

  private:
    std::unordered_map<TLeft, const TRight *> _leftToRight;
    std::unordered_map<TRight, const TLeft *> _rightToLeft;
};
} // namespace Physics::Utils
