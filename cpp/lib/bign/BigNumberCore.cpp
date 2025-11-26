#include "BigNumberCore.h"
#include "BigNumberNaive.h"
#include "BigDigit.h"
#include <stdexcept>

namespace bign {

BigNumVariant BigNumberCore::createBest() {
  return std::make_unique<BigNumberNaive>();
}

BigNumVariant BigNumberCore::create(BigImpl type) {
  if(type == BIG_AUTO) return createBest();
  
  switch(type) {
    case BIG_NAIVE:
      return std::make_unique<BigNumberNaive>();
    default:
      return createBest();
  }
}

BigNumberCore::BigNumberCore() : impl_(createBest()), active_type_(BIG_AUTO), digits_(1, BigDigit(0)) {}

BigNumberCore::BigNumberCore(BigImpl type) : impl_(create(type)), active_type_(type), digits_(1, BigDigit(0)) {}

BigNumberCore::BigNumberCore(const BigNumberCore& other) 
  : impl_(create(other.active_type_)), active_type_(other.active_type_), digits_(other.digits_) {}

BigNumberCore::BigNumberCore(BigNumberCore&& other) noexcept
  : impl_(std::move(other.impl_)), active_type_(other.active_type_), digits_(std::move(other.digits_)) {
  other.active_type_ = BIG_AUTO;
  other.digits_ = std::vector<BigDigit>(1, BigDigit(0));
}

BigNumberCore& BigNumberCore::operator=(const BigNumberCore& other) {
  if(this != &other) {
    impl_ = create(other.active_type_);
    active_type_ = other.active_type_;
    digits_ = other.digits_;
  }
  return *this;
}

BigNumberCore& BigNumberCore::operator=(BigNumberCore&& other) noexcept {
  if(this != &other) {
    impl_ = std::move(other.impl_);
    active_type_ = other.active_type_;
    digits_ = std::move(other.digits_);
    other.active_type_ = BIG_AUTO;
    other.digits_ = std::vector<BigDigit>(1, BigDigit(0));
  }
  return *this;
}

void BigNumberCore::setImplementation(BigImpl type) {
  impl_ = create(type);
  active_type_ = type;
}

const char* BigNumberCore::getImplementationName() const {
  return std::visit([](auto& impl_ptr) -> const char* {
    return impl_ptr ? impl_ptr->name() : "Unknown";
  }, impl_);
}

void BigNumberCore::add(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.add(digits_, n.digits_);
  });
}

void BigNumberCore::subtract(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.subtract(digits_, n.digits_);
  });
}

void BigNumberCore::multiply(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.multiply(digits_, n.digits_);
  });
}

void BigNumberCore::divide(const BigNumberCore& n, BigNumberCore& quotient, BigNumberCore& remainder) const {
  std::visit([&](auto& impl_ptr) {
    if(impl_ptr) {
      impl_ptr->divide(digits_, n.digits_, quotient.digits_, remainder.digits_);
    }
  }, impl_);
}

int BigNumberCore::compare(const BigNumberCore& n) const {
  int result = 0;
  const_cast<BigNumberCore*>(this)->visitImpl([&](auto& impl) {
    result = impl.compare(digits_, n.digits_);
  });
  return result;
}

void BigNumberCore::bitwiseAnd(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.bitwiseAnd(digits_, n.digits_);
  });
}

void BigNumberCore::bitwiseOr(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.bitwiseOr(digits_, n.digits_);
  });
}

void BigNumberCore::bitwiseXor(const BigNumberCore& n) {
  visitImpl([&](auto& impl) {
    impl.bitwiseXor(digits_, n.digits_);
  });
}

void BigNumberCore::bitwiseNot() {
  visitImpl([&](auto& impl) {
    impl.bitwiseNot(digits_);
  });
}

void BigNumberCore::shiftLeft(int bits) {
  visitImpl([&](auto& impl) {
    impl.shiftLeft(digits_, bits);
  });
}

void BigNumberCore::shiftRight(int bits) {
  visitImpl([&](auto& impl) {
    impl.shiftRight(digits_, bits);
  });
}

} // namespace bign
